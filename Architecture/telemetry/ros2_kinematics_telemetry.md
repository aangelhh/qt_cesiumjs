# ROS 2 Kinematics Telemetry

## Purpose

qttest can publish read-only `KinematicsTelemetrySnapshot` values directly to
ROS 2/DDS. The publisher does not command entities, modify physics, or retain
references to runtime state.

ROS 2 support is optional at build time. A build without ROS 2 uses an explicit
unavailable backend and keeps the rest of qttest functional. No WebSocket is
used by qttest for ROS 2 telemetry.

## Runtime flow

```text
SimulationEngine
  -> EventKinematicsTelemetryUpdated
  -> Ros2TelemetryPublisher
  -> bounded background queue
  -> rclcpp publisher
  -> ROS 2 RMW / DDS
  -> PlotJuggler or another ROS 2 subscriber
```

The queue drops its oldest sample under sustained load instead of blocking the
simulation. Topics use `best effort`, `volatile`, `keep last`, depth 5 QoS.
The backend owns a worker thread and a dedicated ROS 2 context; neither DDS
discovery nor publication runs on the Qt UI or simulation thread.

## Topics

Each entity is published on a topic derived from its stable internal ID:

```text
/qttest/entities/<stable_entity_id>/kinematics
```

The prefix is configurable from `View -> ROS 2 Telemetry...`. Entity names are
metadata only and are not used as runtime identity when an internal ID exists.

## Data contract

`qttest_interfaces/msg/KinematicsTelemetry` contains:

- WGS84 position and altitude.
- Heading, pitch, and roll.
- Speed and vertical speed.
- Controller targets, errors, and limits.
- Task and dynamics-backend state.
- Simulation time and dynamics-step duration.
- Fuel and per-engine telemetry.

The ROS message is produced from a neutral, unit-tested C++ contract before it
reaches the middleware-specific backend.

## Verification boundaries

The default build uses an unavailable stub so ROS 2 remains optional. Contract,
filtering, rate limiting, queue fallback, and CSV behavior are covered in the
normal unit-test suite. A separate ROS-enabled smoke test creates two native
`rclcpp` participants and verifies that a typed telemetry message crosses DDS.

On macOS/RoboStack, the exported `rclcpp` target includes Python ROSIDL
generator dylibs. The ROS-enabled build therefore links the environment's
Python runtime on Apple platforms so those transitive libraries resolve cleanly
at process startup. The qttest telemetry implementation itself remains C++.

## PlotJuggler

PlotJuggler is a ROS 2 subscriber in this architecture. Use the official ROS 2
plugins for a direct DDS subscription. A PlotJuggler bridge may be used outside
qttest when a particular PlotJuggler installation lacks a ROS 2 data source,
but it is not part of qttest's publication path.

Recommended plots:

- Actual heading, target heading, and signed heading error.
- Actual altitude, target altitude, altitude error, and vertical speed.
- Pitch and roll.
- Actual speed, target speed, speed error, and acceleration limit.
- Dynamics-step duration and simulation delta.
- Fuel remaining, fuel percentage, fuel flow, and engine values.

## CSV recording

The same dialog can record the neutral telemetry stream to CSV. CSV recording
uses the same entity filter and publication frequency and remains independent
from ROS 2 availability.

See [`integrations/ros2/README.md`](../../integrations/ros2/README.md) for build
and validation instructions.
