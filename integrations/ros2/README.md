# Native ROS 2 telemetry integration

qttest publishes kinematics telemetry directly through `rclcpp` and the active
ROS 2 RMW/DDS implementation. There is no WebSocket or sidecar between qttest
and ROS 2.

## Architecture

```text
qttest runtime
  -> KinematicsTelemetrySnapshot
  -> Ros2TelemetryPublisher
  -> qttest_interfaces/KinematicsTelemetry
  -> ROS 2 RMW (Fast DDS, Cyclone DDS, ...)
  -> PlotJuggler / analysis subscribers
```

qttest owns simulation time. The publisher is read-only, asynchronous, and
cannot command or mutate entities.

## Build the message package

Create a ROS 2 workspace and link the interface package:

```bash
source /opt/ros/kilted/setup.bash
mkdir -p ~/qttest_ros2_ws/src
ln -s "$PWD/integrations/ros2/qttest_interfaces" \
  ~/qttest_ros2_ws/src/
cd ~/qttest_ros2_ws
rosdep install --from-paths src --ignore-src -y
colcon build --symlink-install --packages-select qttest_interfaces
source install/setup.bash
```

### Local macOS/Pixi setup

On the current development machine, ROS 2 Jazzy is managed by Pixi in
`~/ros2_tactical`. Enter that environment before building or running ROS 2
commands:

```bash
cd /Users/angelconde/mi-proyecto/qttest
/Users/angelconde/.pixi/bin/pixi shell \
  --manifest-path /Users/angelconde/ros2_tactical/pixi.toml

colcon build \
  --base-paths integrations/ros2/qttest_interfaces \
  --build-base build-ros2-workspace/build \
  --install-base build-ros2-workspace/install \
  --log-base build-ros2-workspace/log \
  --packages-select qttest_interfaces
source build-ros2-workspace/install/setup.zsh
```

The Pixi environment must include `ros-jazzy-rosidl-default-generators` to
build the custom messages. The current `~/ros2_tactical` environment already
contains it.

## Build qttest with ROS 2

Configure qttest from a terminal where both ROS 2 and the interface workspace
are sourced:

```bash
source /opt/ros/kilted/setup.bash
source ~/qttest_ros2_ws/install/setup.bash

cmake -S /path/to/qttest -B /path/to/qttest/build-ros2 \
  -DQTTEST_ENABLE_ROS2_TELEMETRY=ON
cmake --build /path/to/qttest/build-ros2 -j2
```

For the local Pixi setup, after sourcing the interface workspace:

```bash
cmake -S . -B build-macos-ros2 \
  -DQTTEST_ENABLE_ROS2_TELEMETRY=ON \
  -DCMAKE_PREFIX_PATH="$CMAKE_PREFIX_PATH:/Users/angelconde/Qt/6.10.2/macos"
cmake --build build-macos-ros2 -j2 --target qttest
```

Without `QTTEST_ENABLE_ROS2_TELEMETRY=ON`, qttest builds normally and the
telemetry dialog reports that ROS 2 support is unavailable.

## Run

Source the same environments before launching qttest:

```bash
source /opt/ros/kilted/setup.bash
source ~/qttest_ros2_ws/install/setup.bash
/path/to/qttest/build-ros2/qttest.app/Contents/MacOS/qttest
```

Local Pixi equivalent:

```bash
cd /Users/angelconde/mi-proyecto/qttest
/Users/angelconde/.pixi/bin/pixi shell \
  --manifest-path /Users/angelconde/ros2_tactical/pixi.toml
source build-ros2-workspace/install/setup.zsh
build-macos-ros2/qttest.app/Contents/MacOS/qttest
```

Open `View -> ROS 2 Telemetry...`, select an entity or all entities, select the
frequency, and enable publication.

## Validate DDS publication

```bash
ros2 interface show qttest_interfaces/msg/KinematicsTelemetry
ros2 topic list | rg /qttest/entities
ros2 topic echo \
  /qttest/entities/<stable_entity_id>/kinematics \
  --qos-reliability best_effort
ros2 topic hz /qttest/entities/<stable_entity_id>/kinematics
ros2 topic info --verbose \
  /qttest/entities/<stable_entity_id>/kinematics
```

Expected QoS is `best effort`, `volatile`, `keep last`, depth 5. Move the
entity and verify that position, heading, pitch, roll, altitude, speed, fuel,
and engine fields change.

The repository also provides a native publisher/subscriber smoke test:

```bash
ctest --test-dir build-macos-ros2 \
  -R Ros2TelemetryNative \
  --output-on-failure
```

This test requires local UDP/multicast access for DDS discovery.

## PlotJuggler

Use PlotJuggler's ROS 2 data-source plugins and subscribe to the kinematics
topics. The PlotJuggler process and qttest must use the same `ROS_DOMAIN_ID` and
compatible RMW implementations.

```bash
export ROS_DOMAIN_ID=0
ros2 run plotjuggler plotjuggler
```

The standalone PlotJuggler under `~/plotjuggler_ws` contains the ROS 2 message
parser but not the ROS 2 live streamer. The local Pixi environment therefore
includes the official `ros-jazzy-plotjuggler-ros` package. Launch that build
from the activated environment:

```bash
source build-ros2-workspace/install/setup.zsh
ros2 run plotjuggler plotjuggler
```

From the repository, the helper below performs both environment setup steps
and avoids launching PlotJuggler without the custom message package:

```bash
integrations/ros2/run_plotjuggler.zsh
```

The same launcher is available in VS Code through
`Tasks: Run Task -> Launch PlotJuggler ROS 2`. Its environment can be checked
without opening the GUI with:

```bash
integrations/ros2/run_plotjuggler.zsh --check
```

In PlotJuggler, select `Streaming -> ROS 2 Topic Subscriber`, refresh the topic
list, select `/qttest/entities/<stable_entity_id>/kinematics`, and start the
stream. The installed plugin library is
`$CONDA_PREFIX/lib/plotjuggler_ros/libDataStreamROS2.dylib`.

If the local PlotJuggler build only includes ROS 2 message parsers, install the
official ROS 2 data-source plugins. An external PlotJuggler bridge can also
adapt DDS for that viewer, but qttest itself remains a native ROS 2 publisher.
