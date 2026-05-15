# RO2S Basics

## Command line tools and fundamental concepts (5h10')
 [Beginner: CLI tools](https://docs.ros.org/en/jazzy/Tutorials/Beginner-CLI-Tools.html)

### Configuring environment (2h)
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

### Using turtlesim, ros2, and rqt (1h)
- Some useful commands
```bash
# Init turtlesim
ros2 run turtlesim turtlesim_node
# Use turtlesim teleop
ros2 run turtlesim turtle_teleop_key

# List ros2 elements
ros2 node list
ros2 topic list
ros2 action list
ros2 service list

# rqt
rqt
```


### Understanding nodes (20')

### Understanding topics (20')

### Understanding services (40')

### Understanding parameters (15')

### Using rqt_console to view logs (10')

### Launching nodes (5')

### Recording and playing back data (20')