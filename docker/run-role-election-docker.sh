#!/bin/bash

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT_DIR"

# Recria a imagem para garantir que está usando o código mais recente
docker build -t turtle-controller -f docker/Dockerfile .

# Executa todos os nós dentro do mesmo container
docker run --rm -it \
    --network host \
    turtle-controller \
    bash -c '
        source /opt/ros/jazzy/setup.bash
        source /ros2_ws/install/setup.bash

        ros2 run role_election detector \
            --ros-args -p robot_id:=1 -r __node:=detector_1 &
        PID1=$!

        ros2 run role_election detector \
            --ros-args -p robot_id:=2 -r __node:=detector_2 &
        PID2=$!

        ros2 run role_election detector \
            --ros-args -p robot_id:=3 -r __node:=detector_3 &
        PID3=$!

        ros2 run role_election decisor &
        PID4=$!

        trap "kill $PID1 $PID2 $PID3 $PID4 2>/dev/null" EXIT

        ros2 run role_election monitor
    '