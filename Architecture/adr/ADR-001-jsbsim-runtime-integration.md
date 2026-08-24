# ADR-001: JSBSim Runtime Integration

Status: Accepted for incremental implementation

## Context

The application currently supports kinematic movement and an optional JSBSim
backend. Qt owns the user interface, Cesium owns visualization, and the
simulation runtime advances entities. Cockpit screens must be able to command
an entity without becoming coupled to JSBSim or taking ownership of simulation
time.

## Decision

- The simulation runtime owns the global clock and calls each dynamics backend
  with simulation time and delta time.
- One JSBSim session is owned per active entity. UI and QML code never own or
  call `FGFDMExec` directly.
- Cockpit screens publish neutral control commands. For the current MVP these
  are heading, altitude, and speed setpoints.
- Internal runtime units are SI. Adapter boundaries perform conversions needed
  by JSBSim. Entity positions remain WGS84 latitude, longitude, and MSL altitude.
- The visual model, dynamics backend, JSBSim aircraft model, control profile,
  systems display profile, and engine count are separate platform properties.
- Platform properties are read from `Data/config3DModel.yaml`. Missing metadata
  uses the existing inference and kinematic fallback behavior.
- A failed or unavailable JSBSim model must not stop the simulation. The entity
  falls back to the kinematic backend and exposes that state through telemetry.
- Direct stick, throttle, or HOTAS input is outside the current scope. Future
  simulated cockpit controls will use the same neutral command boundary.

## Timing

The runtime passes the simulation delta to the backend. JSBSim may use bounded
internal substeps when required, but it cannot advance the global clock or run
an independent real-time loop.

Steady-state JSBSim steps have an 8 ms wall-clock budget. A single slow frame
does not trigger degradation: the runtime latches the kinematic fallback after
three consecutive overruns. Model load is intentionally excluded because it is
a one-time lifecycle cost. Load/step failures and invalid output latch fallback
immediately. The latch is cleared by model replacement, entity release, scenario
load/reset, or mission stop; it is not retried every tick.

Deterministic replay uses the same simulation-owned time contract but disables
wall-clock budget enforcement through `FlightDynamicsExecutionPolicy`. Step
duration remains observable, while host load cannot change backend selection or
entity state. Real-time execution keeps budget enforcement enabled by default.

For the same model, configuration, initial state, simulation times, deltas, and
control setpoints, replay states must remain equivalent at every tick. Current
same-build/platform tests use tolerances of `1e-10` degrees for WGS84 position
and `1e-8` for other floating-point state. This is numerical equivalence, not a
claim of bitwise identity across different JSBSim/compiler/platform versions.
An explicit model reset reloads the JSBSim aircraft before applying initial
conditions because `RunIC` alone does not reset every integrator, propulsion,
and aircraft-system internal state.

## Consequences

- Existing Qt/Cesium integration remains unchanged.
- `IDynamicsModel` now defines the neutral lifecycle, canonical dynamics state,
  and runtime-owned step context used by dynamics backends.
- `KinematicDynamicsModel` is the first implementation and owns the existing
  WGS84 position and altitude propagation.
- `JSBSimDynamicsModel` implements the same lifecycle and owns `FGFDMExec`,
  model loading, bounded substeps, control application, fuel synchronization,
  and engine telemetry.
- `FlightDynamicsEngine` selects and orchestrates the backend, maps neutral
  task setpoints into `DynamicsStepContext`, and retains the kinematic fallback.
- Dynamics instances are keyed by stable entity identity and explicitly
  released when an entity or scenario is removed, stopped, loaded, or reset.
- Backend step duration and fallback reason are runtime diagnostics exposed in
  kinematics/systems telemetry and are not persisted in scenario files.
- Platform configuration can evolve without adding model-name conditionals to
  dialogs or the flight dynamics engine.
- Model-specific control gains and direct-control profiles can be introduced
  incrementally after platform metadata is available.
