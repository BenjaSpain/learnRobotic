#!/usr/bin/env bash

# ros2_env_conf.sh
#    - To current Terminal keep sourced and apply changes.
#    - RUN as: source ros2_env_conf.sh

# Source ROS2 environment into current terminal
source /opt/ros/jazzy/setup.bash

#Define ROS_DOMAIN_ID = 10
export ROS_DOMAIN_ID=10
#echo "export ROS_DOMAIN_ID=10" >> ~/.bashrc

#Middleware implementation identifier: rmw_fastrtps_cpp
export RMW_IMPLEMENTATION=rmw_fastrtps_cpp