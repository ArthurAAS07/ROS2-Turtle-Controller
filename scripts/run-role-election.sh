#!/bin/bash

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"

source /opt/ros/humble/setup.bash
source "$ROOT_DIR/install/setup.bash"

ros2 run role_election detector \
  --ros-args \
  -p robot_id:=1 \
  -r __node:=detector_1 &

PID1=$!

ros2 run role_election detector \
  --ros-args \
  -p robot_id:=2 \
  -r __node:=detector_2 &

PID2=$!

ros2 run role_election detector \
  --ros-args \
  -p robot_id:=3 \
  -r __node:=detector_3 &

PID3=$!

ros2 run role_election decisor &
PID4=$!

trap "kill $PID1 $PID2 $PID3 $PID4 2>/dev/null" EXIT

ros2 run role_election monitor