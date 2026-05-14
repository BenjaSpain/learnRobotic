# RO2S Basics

## Command line tools and fundamental concepts
 [Beginner: CLI tools](https://docs.ros.org/en/jazzy/Tutorials/Beginner-CLI-Tools.html)

### Configuring environment 
- 2h: 2026-04-27 to 2026-05-13
- Configuration file: `/home/benjamincanton/ROS2_learn/ros2_env_conf.sh`:

```bash
#!/usr/bin/env bash 
# ros2_env_conf.sh 
#    - To current Terminal keep sourced and apply changes. 
#    - RUN as: source ros2_env_conf.sh 

# Source ROS2 environment into current terminal 
source /opt/ros/jazzy/setup.bash 

#Define ROS_DOMAIN_ID = 10 
export ROS_DOMAIN_ID=10 
#echo "export ROS_DOMAIN_ID=10" >> ~/.bashrc 
```

- Appy configuration:
``` bash
source /home/benjamincanton/ROS2_learn/ros2_env_conf.sh 

#Check that configuration is applied. Should see: 
printenv | grep -i ROS

#ROS_VERSION=2 
#ROS_PYTHON_VERSION=3 
#ROS_DISTRO=jazzy 
```

### Using turtlesim, ros2, and rqt 
- 1h: 2026-04-27 to 2026-04-27

### Understanding nodes 
- 20': 2026-05-13 to 2026-05-13

Understanding topics 
- : 2026-05-14 to WiP

Understanding services 
Understanding parameters 
Using rqt_console to view logs 
Launching nodes 
Recording and playing back data 