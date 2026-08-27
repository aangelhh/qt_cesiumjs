# Sensor Model Providers

## Purpose

The sensor runtime supports complementary sensor implementations without
coupling scenario state, plans, or the UI to a particular framework.

`SensorEngine` remains responsible for common geometric gates, contact
lifecycle, track limits, and deterministic simulation timing. An
`ISensorModel` provider decides the probability and outcome of one eligible
sensor observation.

## Runtime Flow

```text
ScenarioState (owns simulation time and provider registry)
  -> SensorEngine (geometry, cadence, contact lifecycle)
    -> SensorModelRegistry (selects modelProviderId)
      -> NativeSensorModel
      -> SharedLibrarySensorModel [MIXR adapter ABI]
      -> StoneSoupSensorModel     [Python sidecar]
      -> SignalLevelSensorAdapter [future]
```

Each `SensorDefinition` persists a `modelProviderId`. Missing, empty, or
unavailable provider identifiers fall back to `native`, preserving legacy
scenarios and allowing optional integrations to be disabled.

Provider declarations live in `Data/sensor_model_providers.json`. The catalog
supports:

- `id` and operator-facing `displayName`;
- `adapterType` (`native`, `shared-library`, or `external-service`);
- `enabled`;
- `defaultForNewSensors`, used as the initial Add Entity selection;
- `libraryPath` for a native plugin loader;
- `endpoint` for the Stone Soup sidecar script;
- provider-specific `options`.

Enabled catalog entries appear in the Add Entity radar configuration. An entry
whose adapter has not been registered is labelled as unavailable and uses the
native fallback at runtime. The requested provider remains persisted so the
same scenario can use it after the adapter is installed.

## Provider Contract

`ISensorModel::evaluate()` receives a read-only `SensorEvaluationContext`:

- scenario seed;
- simulation time and scan index;
- observer, sensor, and target state;
- range already accepted by the common geometric gates.

It returns `SensorEvaluationResult` with probability, deterministic sample,
and detection outcome. Providers can also return optional diagnostics such as
provider version, target signature, SNR, RF range loss, and echo ratio.
Providers must not mutate entities or advance the global clock.

## Runtime Diagnostics

`SensorEngine` records the last eligible evaluation for every sensor in a
runtime-only `SensorRuntimeStatus`. This state is intentionally not persisted
in scenario files. It contains:

- requested and effective provider identifiers;
- provider version and native fallback reason;
- effective detection probability and deterministic sample;
- target signature, SNR, RF range loss, and echo ratio when supplied;
- evaluation latency, scan index, and simulation time;
- cumulative evaluation and detection counters.

Detected and coasting contacts carry the same diagnostics. Track confidence
therefore remains separate from the probability used by the latest model
evaluation.

Double-clicking an entity and selecting **Sensor Information** displays the
configuration, live runtime status, and contact diagnostics. The dialog
refreshes every 500 ms while the entity remains available. Unsupported values
are displayed as unavailable instead of zero.

This structured runtime status is the source for a future dedicated ROS 2
`sensor_diagnostics` topic and historical PlotJuggler export. It must remain
separate from the existing kinematics topic because one entity can own several
sensors and evaluate several targets per scan.

## Provider Roles

### Native

The built-in provider is dependency-free and deterministic. It is the default
and fallback implementation and currently combines configured detection
probability, range degradation, and target signature.

### MIXR / OpenEaagles

The runtime provides a versioned C ABI and dynamic-library loader for MIXR.
The macOS build compiles a pinned upstream revision with a small POSIX patch
and produces a native adapter using MIXR's RF range-loss calculation. MIXR can
later provide richer radar, IR, RF, RWR, or electronic-warfare models without
exposing its types to domain objects. Build it with
`cmake --build build-macos-debug --target qttest_setup_mixr`; the VS Code launch
profiles already set `QTTEST_MIXR_SENSOR_PLUGIN`. Missing or unhealthy plugins
fall back explicitly to `native`. The current adapter exposes the pinned MIXR
revision, one-way RF range loss, echo ratio, effective detection probability,
and evaluation latency.

### Stone Soup

The Stone Soup provider runs as a persistent Python JSON-lines sidecar. The
current adapter calls Stone Soup `AESARadar` for SNR and detection probability,
then applies qttest's deterministic sample for reproducible outcomes. The
sidecar receives simulation time and never owns or advances the global clock.
Requests have a bounded timeout and explicitly fall back to `native` on error.
Provider version, linear SNR, SNR in dB, and request latency are exposed in the
live sensor diagnostics.

Install the isolated environment with:

```bash
cmake --build build-macos-debug --target qttest_setup_stonesoup
```

The application discovers that build-local environment automatically. Set
`QTTEST_STONESOUP_PYTHON` to use another managed Python environment.

### Signal-Level Tools

HermesPy or GNU Radio belong behind a separate signal-level adapter for
selected high-fidelity experiments. They are not part of the regular tactical
tick.

## Incremental Integration

1. Keep `NativeSensorModel` as the reference and fallback provider.
2. Add contract tests for every adapter using fixed scenario time and seed.
3. Extend the MIXR adapter from RF range loss to configurable radar profiles,
   clutter, RWR, and tracking as separate capabilities.
4. Move Stone Soup evaluation to batched asynchronous requests for large runs.
5. Publish `SensorRuntimeStatus` through a versioned ROS 2 sensor-diagnostics
   contract for PlotJuggler and recording/replay analysis.

## Determinism Rules

- `ScenarioState` owns simulation time.
- Providers receive time; they never advance it.
- The same scenario state, provider version, seed, and scan index must produce
  the same result.
- External results must include their source time and provider version.
- A missing or unhealthy optional provider falls back explicitly to `native`.
