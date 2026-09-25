#!/bin/bash

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"

source /opt/ros/$ROS_DISTRO/setup.bash
source "$ROOT_DIR/install/setup.bash"

ros2 run turtlesim turtlesim_node &
ros2 run turtle_controller turtle &
ros2 run turtle_controller send_msg