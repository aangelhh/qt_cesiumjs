# Startup configuration

qttest presents a modal configuration dialog before constructing `MainWindow`.
The dialog separates the primary federation transport from optional output and
telemetry integrations.

## Federation mode

Exactly one mode can be selected:

- `Standalone`: local simulation without a federation transport.
- `DIS`: multicast/unicast endpoint, port, site ID, and application ID.
- `HLA`: RTI backend, local settings designator, federation identity, federate
  identity, optional synchronization point, optional time management, and
  ordered FOM modules.

HLA mode is operational at application startup. Before `MainWindow` opens,
qttest loads the selected backend, connects to the RTI, optionally creates the
federation, loads the configured FOM modules, and joins the federate. Callback
polling then runs from the Qt event loop until application shutdown. A startup
failure returns the operator to this dialog with the RTI error. The DIS
transport adapter remains a separate follow-up integration.

When `Synchronization point` is configured, qttest requests registration after
joining, waits for the RTI announcement, and automatically reports the point as
achieved. The operational log shows registration, announcement, achievement,
and final federation synchronization. Registration rejection and its RTI
reason are also reported instead of leaving startup diagnostics ambiguous.
Leaving the field empty preserves the normal immediate startup flow.

`Enable HLA time management` activates the conservative time-regulating and
time-constrained path. qttest negotiates both services sequentially before the
main window opens. The configured lookahead defaults to `0.01 s`. While the
simulation is running, qttest keeps at most one time advance request pending;
the local scenario advances only to the logical time granted by the RTI.
Entity, munition, radar/emitter, fire, and detonation publications are sent at
the latest grant plus that lookahead. Simulation-control interactions remain
receive-order so an operator can pause or stop immediately.
Leaving this option disabled retains the existing receive-order local clock.

After joining, local entity objects are registered and updated every 100 ms.
Each active munition owns an RPR `PhysicalEntity.Munition` object and emits one
correlated `WeaponFire` interaction. Impacts emit `MunitionDetonation` before
the munition object is removed; enabled radars publish `EmitterSystem` and
`RadarBeam`. Supported remote RPR platforms are reflected into runtime-only,
externally controlled entities. Remote RPR munition objects are mirrored as
visual tracks without local simulation or damage. Play, pause, and stop use the standard
`StartResume` and `StopFreeze` interactions. Owned objects are deleted before
the federate resigns.

OpenRTI defaults to `thread://` for a self-contained local federation. External
nodes can use `rti://host:port`; the legacy Pitch-compatible form
`crcAddress=host:port` is accepted and translated by the OpenRTI compatibility
adapter.

## Independent integrations

These options can be enabled independently of the selected federation mode and
of each other:

- `ROS 2 / DDS`: domain ID and telemetry topic prefix. This option is available
  only in a build configured with `QTTEST_ENABLE_ROS2_TELEMETRY=ON`; its values
  are mirrored to the existing native ROS 2 telemetry publisher settings.
- `CIGI`: image-generator host and port. Configuration is persisted now; the
  CIGI runtime adapter is pending.

The VS Code configuration `Launch qttest` builds and runs
`build-macos-ros2` with native ROS 2/DDS support. `Launch qttest (sin ROS 2)`
uses `build-macos-debug`, where the ROS 2 option is intentionally unavailable.

## Persistence

Values are stored through `QSettings` under the `Startup` group. The selected
ROS 2 state and topic prefix are also mirrored into `Ros2Telemetry`, which is
the settings group consumed by `MainWindow`.

The repository RPR FOM followed by all NETN modules is added automatically in
dependency order. Operator-supplied modules are preserved and defaults are not
duplicated on later starts.
