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

## Consequences

- Existing Qt/Cesium integration remains unchanged.
- Platform configuration can evolve without adding model-name conditionals to
  dialogs or the flight dynamics engine.
- A future `IDynamicsModel` can wrap both kinematic and JSBSim implementations
  without changing cockpit or task APIs.
- Model-specific control gains and direct-control profiles can be introduced
  incrementally after platform metadata is available.
