FROM osrf/ros:jazzy-desktop

SHELL ["/bin/bash", "-c"]

WORKDIR /ros2_ws

COPY src ./src

RUN apt-get update && \
    apt-get install -y \
    python3-colcon-common-extensions \
    python3-rosdep && \
    rm -rf /var/lib/apt/lists/*

RUN source /opt/ros/jazzy/setup.bash && \
    rosdep update && \
    rosdep install \
    --from-paths src \
    --ignore-src \
    -r \
    -y

RUN source /opt/ros/jazzy/setup.bash && \
    colcon build
