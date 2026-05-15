# RO2S Basics

## Command line tools and fundamental concepts (5h10')
 [Beginner: CLI tools](https://docs.ros.org/en/jazzy/Tutorials/Beginner-CLI-Tools.html)

### Configuring environment (2h)

- Appy configuration to each terminal:
``` bash
source ros2_env_conf.sh 

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

# List elements:
# ros2 <node|topic|action|service|param> list <-t: Optional, include 'type'>
ros2 node list
ros2 topic list
ros2 action list
ros2 service list -t
ros2 param list -t

# Element info:
#   ros2 <node|topic|action|serviced> info <element_name> <--verboce: optiona for details>
ros2 node info /my_turtle
ros2 topic info /turtle1/cmd_vel --verbose
ros2 action info /turtle1/rotate_absolute


# List elements of specific 'Type':
#   ros2 <topic|service|action> find <element_type>
ros2 topic find geometry_msgs/msg/Twist
ros2 service find std_srvs/srv/Empty

# Inspect structure of a element type
#   ros2 interface show <message_type| service_type|action_type> 
ros2 interface show geometry_msgs/msg/Twist
ros2 interface show turtlesim/action/RotateAbsolute

# rqt
rqt
```

### Understanding nodes (20')
```bash
# RUN executable: 
#   ros2 run <package_name> <executable_name>
ros2 run turtlesim turtlesim_node
ros2 run turtlesim turtle_teleop_key

# Remap: Reasign Node properties, case rename node
ros2 run turtlesim turtlesim_node --ros-args --remap __node:=my_turtle
```

### Understanding topics (20')
```bash
# RUN rqt_graph
ros2 run rqt_graph rqt_graph

# Show data published on a topic:
#   ros2 topic echo <topic_name>
ros2 topic echo /turtle1/cmd_vel


# Publish on Topic
#   ros2 topic pub <optional_flags> <topic_name> <msg_type> '<args>'
# Loop:
ros2 topic pub /turtle1/cmd_vel geometry_msgs/msg/Twist "{linear: {x: 2.0, y: 0.0, z: 0.0}, angular: {x: 0.0, y: 0.0, z: 1.8}}"
# One time:
ros2 topic pub --once /turtle1/cmd_vel geometry_msgs/msg/Twist "{linear: {x: 2.0, y: 0.0, z: 0.0}, angular: {x: 0.0, y: 0.0, z: 1.8}}"

# Topic frequency, 
ros2 topic hz /turtle1/pose

# Topic Bandwidth
ros2 topic bw /turtle1/pose
```

### Understanding services (40')
```bash
# Find out services of a 'type'
#   ros2 service type <service_name>
ros2 service type /clear

# Call for a service
#   ros2 service call <service_name> <service_type> <arguments>
ros2 service call /clear std_srvs/srv/Empty

# See data between service server and client
#   ros2 service echo <service_name | service_type> <arguments>
ros2 service echo --flow-style /add_two_ints
```

### Understanding parameters (15')
```bash
# Display parameter' type and value
#   ros2 param get <node_name> <parameter_name>
ros2 param get /turtlesim background_g

# Modify parameter' value
#   ros2 param set <node_name> <parameter_name> <value>
ros2 param set /turtlesim background_r 150

# Show/Save all parameter' value of a Node
#   ros2 param dump <node_name> < Optional: > files_to_save >
ros2 param dump /turtlesim > turtlesim.yaml

# Load params from file to a started Node (Read-only parameters not allowed)
#   ros2 param load <node_name> <parameter_file>
ros2 param load /turtlesim turtlesim.yaml

# Start Node using saved parameters (Read-only parameters allowed)
#   ros2 run <package_name> <executable_name> --ros-args --params-file <file_name>
ros2 run turtlesim turtlesim_node --ros-args --params-file turtlesim.yaml
```
### Understanding Actions(30')
- Action: 
    - Parts: Goal, feedback, result
    - Can be Cancelled
    - Client-Server model

```bash
# Send action goal
#   ros2 action send_goal <action_name> <action_type> <values (YAML format)> --feedback (optional for receiving confinue feedback)
ros2 action send_goal /turtle1/rotate_absolute turtlesim/action/RotateAbsolute "{theta: 1.57}" --feedback
```

### Using rqt_console to view logs (10')
- rqt_console: GUI tool used to introspect log messages in ROS 2
```bash
# Start rqt_console
ros2 run rqt_console rqt_console

# Set defaul logger level of started Node. 
#   Logger Level: [FATAL | ERROR | WARN | INFO | DEBUG]
ros2 run turtlesim turtlesim_node --ros-args --log-level WARN
```

### Launching nodes (10')
- Single launch file with command `ros2 launch` will start up your entire system at once.


```bash
# RUN Launch File
#   ros2 launch <package> <file_to_lauch>
ros2 launch turtlesim multisim.launch.py
```

### Recording and playing back data (20')
- `ros bag` record data published on topics and services in a database. Let replay data to reproduce results, test use cases.
- Data saved in a YAML file

```bash
# Record data published to a topic
#   ros2 bag record <topic_name>
ros2 bag record /turtle1/cmd_vel

# Inspect recorded file
#   ros2 bag info <bag_file_name>
ros2 bag info subset

# Play saved file
ros2 bag play subset

# Record Service:
#   Specific: ros2 bag record --service <service_names>
#   All: ros2 bag record --all-services
ros2 bag record --service /add_two_ints
```