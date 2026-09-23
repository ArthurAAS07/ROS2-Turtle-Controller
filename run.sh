#!/bin/bash

source /opt/ros/$ROS_DISTRO/setup.bash
source ~/ros2_projects/turtle_ws/install/setup.bash

ros2 run turtlesim turtlesim_node &
ros2 run turtle_controller turtle &
ros2 run turtle_controller send_msg
