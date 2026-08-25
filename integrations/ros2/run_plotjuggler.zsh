#!/bin/zsh

set -eo pipefail

readonly SCRIPT_DIR="${0:A:h}"
readonly REPOSITORY_ROOT="${SCRIPT_DIR:h:h}"
readonly ROS2_PREFIX="/Users/angelconde/ros2_tactical/.pixi/envs/default"
readonly INTERFACES_SETUP="${REPOSITORY_ROOT}/build-ros2-workspace/install/setup.sh"

if [[ ! -f "${ROS2_PREFIX}/local_setup.sh" ]]; then
  print -u2 "ROS 2 environment not found at ${ROS2_PREFIX}"
  exit 1
fi

if [[ ! -f "${INTERFACES_SETUP}" ]]; then
  print -u2 "qttest_interfaces is not built. Build the ROS 2 workspace first."
  exit 1
fi

source "${ROS2_PREFIX}/local_setup.sh"
source "${INTERFACES_SETUP}"

export ROS_DOMAIN_ID="${ROS_DOMAIN_ID:-0}"
export RMW_IMPLEMENTATION="${RMW_IMPLEMENTATION:-rmw_fastrtps_cpp}"

if [[ "${1:-}" == "--check" ]]; then
  ros2 pkg prefix qttest_interfaces
  ros2 interface show qttest_interfaces/msg/KinematicsTelemetry >/dev/null
  print "PlotJuggler ROS 2 environment is ready."
  exit 0
fi

exec ros2 run plotjuggler plotjuggler "$@"
