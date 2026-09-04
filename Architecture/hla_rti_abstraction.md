# HLA RTI Abstraction

## Purpose

The HLA integration belongs to the interoperability infrastructure. The
application owns simulation time and domain state. An RTI backend transports
federation operations, object updates, and interactions; received exercise
control is applied explicitly by the application layer.

The abstraction prevents Qt UI, Cesium, tasks, sensors, and entity models from
depending directly on a vendor SDK.

## Location and boundaries

- Neutral runtime: `src/infrastructure/interoperability/hla`
- RTI integrations: `integrations/hla/<backend>`
- Optional built plugins: `build-macos-debug/hla-plugins`

`HlaRuntime` owns the federation lifecycle. `IHlaBackend` is the C++ boundary
used by the application. Dynamic plugins implement the stable C ABI declared
in `HlaBackendPluginApi.h`, avoiding C++ ABI coupling between the application
and a vendor RTI.

## Backend selection

`HlaBackendFactory` accepts a `BackendConfiguration`:

- `Mock` creates the built-in deterministic test backend.
- `SharedLibrary` loads an RTI adapter from `libraryPath`.
- `expectedBackendId` prevents accidentally loading the wrong plugin.

An OpenRTI, Portico, Pitch, or future RTI adapter therefore implements the C
plugin API without changing `HlaRuntime`.

## OpenRTI backend

OpenRTI is included as the `Dependencies/OpenRTI` submodule and built as an
isolated CMake external project. Only its IEEE 1516e interface is enabled. Its
headers and libraries stay private to `qttest_hla_openrti1516e_backend`; the
main executable does not link against OpenRTI.

The dependency is pinned by the submodule commit and may be disabled with:

```bash
cmake -S . -B build-macos-debug \
  -DQTTEST_ENABLE_OPENRTI_HLA_BACKEND=OFF
```

OpenRTI offers LGPL 2.1, LGPL 3.0, or MPL 2.0 licensing. The project currently
uses it under MPL 2.0; release packaging must retain the applicable notices and
source modifications required by that license.

### Legacy NETN compatibility

The upstream RTI remains untouched. `OpenRtiCompatibilityBackend` decorates the
loaded OpenRTI plugin at the qttest infrastructure boundary and provides two
isolated compatibility rules:

- `crcAddress=host:port` is translated to OpenRTI's native
  `rti://host:port` local settings designator.
- Multiple legacy RPR/NETN modules are combined into a temporary FOM before
  federation creation. This fills incomplete parent declarations found in
  older modular FOM sets without modifying the source XML files.

Callbacks use the standard `HLA_EVOKED` model and are driven by
`HlaRuntime::poll`; no RTI-owned callback thread controls qttest execution.
An integration test creates and joins an in-process OpenRTI federation using
all repository RPR/NETN modules, polls callbacks, resigns, and disconnects.

## Pitch pRTI backend

When `/Applications/prti1516e` or `PitchRTI_ROOT` is available, CMake builds
`qttest_hla_pitch1516e_backend`. The adapter uses the standard IEEE 1516e API
behind the neutral plugin contract.

Configure explicitly when needed:

```bash
cmake -S . -B build-macos-debug \
  -DQTTEST_ENABLE_PITCH_HLA_BACKEND=ON \
  -DQTTEST_PITCH_RTI_ROOT=/Applications/prti1516e
```

The local pRTI Free installation is not redistributed. Its license restricts
use to learning and testing and must not be assumed suitable for commercial
deployment. Production packaging must select an RTI with compatible licensing.

## Current scope

Implemented lifecycle, publication, and reception:

1. Connect to RTI.
2. Create the federation when configured.
3. Join the federation.
4. Publish RPR object and interaction classes.
5. Register, update, and delete local entity object instances.
6. Register and update an RPR `Munition` object for every active local munition.
7. Emit one `WeaponFire` interaction per newly launched munition, correlated
   through `MunitionObjectIdentifier`.
8. Publish `MunitionDetonation` once for every missile or bomb impact effect,
   then remove the corresponding `Munition` object.
9. Publish RPR `EmitterSystem` and `RadarBeam` objects for enabled local radars.
10. Subscribe to supported RPR platform classes and reflect remote state.
11. Subscribe to remote RPR `Munition` objects and mirror their visual
    lifecycle without applying local guidance, collision, or damage.
12. Create, update, and remove externally controlled runtime entities.
13. Publish and receive RPR `StartResume` and `StopFreeze` controls.
14. Receive remote `EmitterSystem` / `RadarBeam` lifecycle and attach its
    directional radar state to the externally controlled host platform.
15. Receive and deduplicate remote `WeaponFire` / `MunitionDetonation` events.
16. Publish and receive the object identifiers tracked by each radar beam.
17. Register, announce, achieve, and complete federation synchronization points.
18. Optionally enable time regulation and time constrained execution using
    `HLAfloat64Time`, then advance only after RTI grants.
19. Poll callbacks.
20. Remove owned objects, resign, and disconnect with rollback on failure.

The plugin ABI v6 keeps RTI handles private to each plugin and exposes opaque
object identifiers to qttest. `HlaEntityPublisher` maps domains to RPR platform
classes and publishes `EntityType`, `EntityIdentifier`, `Spatial`,
`DamageState`, `ForceIdentifier`, `LiveEntityMeasuredSpeed`, and `Marking` at
10 Hz. Instance names use stable entity UUIDs, so display names may repeat.
WGS84 positions and local NED attitude are converted to ECEF for `Spatial`.

`HlaWarfarePublisher` owns the outbound RPR `Munition` object lifecycle and
sends `HLAinteractionRoot.WeaponFire` with event ID,
mission index, ECEF firing location and velocity, munition type, quantity,
rate, fuse, warhead, and the registered munition object identifier. It also
sends `MunitionDetonation` from the existing
transient impact effects, exactly once per effect ID. A fire and its matching
detonation reuse the same RPR `EventIdentifier`. `FiringObjectIdentifier` and
`TargetObjectIdentifier` reference the registered platform instance names and
are decoded on reception for diagnostics and future authority decisions.
`MunitionObjectIdentifier` references the active RPR
`PhysicalEntity.Munition` instance for both launch and detonation.
Remote munition objects are rendered as runtime-only Cesium tracks. Their
kinematics remain authoritative in the remote federate, and qttest does not
run them through `MunitionSimulator` or apply damage from their object state.

`HlaSensorPublisher` represents an enabled radar as an `EmitterSystem`. While
the radar is emitting, it also owns a `RadarBeam` with azimuth/elevation scan,
frequency, bandwidth, effective radiated power, and a high-density-track flag.
Stopping emission removes only the beam; disabling/removing the sensor removes
both objects.

Detected contacts are correlated with their registered platform instances and
published through the RPR `RadarBeam.TrackObjectIdentifiers` attribute. The
attribute uses the standard `RTIobjectIdArray` representation and contains only
contacts currently detected by that sensor. On reception, qttest resolves each
identifier against local or externally controlled platforms and populates the
existing sensor-contact model with target ID, marking, range, bearing, and
track state. Removing a beam or emitter also removes its remote contacts.
`HighDensityTrack` remains a beam-level indication; it is not treated as a
substitute for explicit target identifiers.

`HlaInboundAdapter` decodes remote RPR `Spatial`, entity type, force, damage,
speed, and marking. Remote entities carry stable IDs prefixed with `hla:` and
are marked `externallyControlled`; local flight dynamics and outbound HLA
publication skip them, preventing feedback loops and competing writers.
The adapter classifies platform, emitter, and beam objects before decoding, so
an RPR sensor object cannot accidentally become a scenario entity. Remote beam
geometry, RF values, emission state, and high-density-track indication are
attached only to externally controlled entities.

Remote warfare interactions are decoded from ECEF to WGS84 and reflected as
short-lived launch/detonation effects plus operator log entries. They are
deduplicated by RPR `EventIdentifier`. This reception path is observational:
it does not simulate a second local projectile and does not apply damage.
Damage authority and target correlation require explicit ownership semantics
before remote detonations may mutate local entities.

| qttest action | RPR interaction | Receive behavior |
| --- | --- | --- |
| Play / Resume | `HLAinteractionRoot.StartResume` | Starts or resumes local simulation |
| Pause | `HLAinteractionRoot.StopFreeze`, non-terminal reason | Pauses without clearing the scenario |
| Stop | `HLAinteractionRoot.StopFreeze`, terminal reason | Stops the exercise using the normal lifecycle |

Synchronization points are exposed by the neutral runtime and both IEEE 1516e
plugins. At startup, an operator may optionally configure a point such as
`ReadyToRun`. qttest requests its registration, automatically achieves it when
the RTI announces it, and records announcement, achievement, and federation
completion in the operational log. An empty setting leaves startup
unsynchronized. Registration success/failure callbacks, synchronization-set
selection, and multi-phase exercise orchestration remain future hardening.

HLA Time Management is available as an opt-in conservative MVP. qttest enables
time regulation first, waits for its callback, then enables time constrained
execution. The Qt simulation timer requests one logical step at a time and
`ScenarioState` advances only after `timeAdvanceGrant`; duplicate outstanding
requests and regressive logical times are rejected. Receive-order execution
remains the default. When Time Management is active, local entity, munition,
radar/emitter, `WeaponFire`, and `MunitionDetonation` publications use
timestamp order at `granted logical time + configured lookahead`. The
receive-order API remains available for unmanaged sessions and immediate
control interactions. Both IEEE 1516e plugins handle the timestamped callback
overloads for object discovery, attribute reflection, object removal, and
interaction reception; the neutral inbound path therefore receives TSO events
instead of silently discarding them. A two-federate OpenRTI integration test
verifies timestamped entity delivery and logical-time grants. Exposing the
received logical timestamp in diagnostics, next-event requests, asynchronous
delivery, and coordinated fast-time policy remain future work.

Ownership Management, DDM, save/restore, and NETN-ETR task exchange remain
subsequent Feature 19 tasks.

### Graphical combat demo

The VS Code configuration `Launch HLA Combat Demo (Pitch)` starts one graphical
qttest process connected to `qttest-federation`. It bypasses the startup dialog
and creates a reproducible local scenario with two RPR-published aircraft:

- `Blue Mirage 2000`, friendly, rendered with the Mirage 2000 model, with an
  emitting 120 by 60 degree airborne radar and enough missiles to tolerate
  missed shots during the sustained engagement.
- `Red Bandit`, opposing, flying a constant heading, altitude, and speed.

The friendly plan is `Follow Entity -> Attack Until Destroyed`. The simulation
starts automatically, so Pitch Explorer can observe both Aircraft objects, both
`EmitterSystem` / `RadarBeam` pairs, `WeaponFire`, damage updates,
`MunitionDetonation`, and the final destroyed state from a single GUI process.
The demo resets the runtime scenario before creating its entities and therefore
must only be used as a test mode.

The existing opt-in CTest
`HlaBackendPlugin.PitchPublishesAndUpdatesAircraftWhenIntegrationEnabled`
remains the headless backend integration test. It creates the federation when
needed, publishes one synthetic Aircraft, updates and removes it, then exits.

## Adding another RTI

1. Add `integrations/hla/<backend>/<Backend>Plugin.cpp`.
2. Implement every function in `QttestHlaBackendApiV6`, including callback
   registration and object/interaction subscriptions.
3. Keep vendor headers and libraries private to that plugin target.
4. Return backend identity, version, capabilities, and diagnostic errors.
5. Add a load/lifecycle test without linking the SDK into the main executable.

This keeps the HLA contract open while each backend remains independently
buildable and licensable.
