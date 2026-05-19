# RO2S Basics

## Command line tools and fundamental concepts (5h45')
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

## Client libraries (WIP)
[Beginner: Client libraries](https://docs.ros.org/en/jazzy/Tutorials/Beginner-Client-Libraries/Colcon-Tutorial.html)

### Using `colcon` to build packages (1h15')
- `colcon` is an iteration on the ROS build tools: `catkin_make`, `catkin_make_isolated`, `catkin_tools` and `ament_tools`

- Minimal folder structure managed by colcon:
   - `src`: ROS sources will be located here
   - `build`: Intermediate files located here. A subfolder for each package
   - `install`: Each package installed here. A subfolder for each package
   - `log`: logging information about `colcon` process

```bash
## USE COLCON WITH PREPARED EXAMPLES
# install colcon
sudo apt update && sudo apt install python3-colcon-common-extensions

# Create folder for workspace
mkdir -p ~/learnRobotic/ros2_ws/src
cd ros2_ws

# Clone example
git clone https://github.com/ros2/examples src/examples -b jazzy

# Build packages (build all in parallel)
colcon build --symlink-install
# Build packages (one-by-one if your hosts has limited resources)
# colcon build --symlink-install --executor sequential

# RUN tests
colcon test

# Source environment to run binaries of examples
source install/setup.bash

## RUN example use case: publisher-subscriber
ros2 run examples_rclcpp_minimal_subscriber subscriber_member_function
ros2 run examples_rclcpp_minimal_publisher publisher_member_function
```

#### Create your own package
- `colon` uses `package.xml` specifications
- Recommended build types are: ament_cmake, ament_python. Also suported cmake
- To create package based on template can use: `ros2 pkg create`

#### Setup `colcon_cd`
- `colcon_cd` allows to change current working directory of shell to the directory of a package
- Here some configurations added to `ros2_env_conf.sh` to source `colcon` and `colcon_cd`:
```bash
### COLCON ###
# Source colcon workspace setup script to overlay on top of the ROS2 installation
source src/install/setup.bash

# Setup colcon_cd for ROS2 workspace
source /usr/share/colcon_cd/function/colcon_cd.sh
# Set COLCON_PREFIX_PATH to the ROS2 workspace install directory
export _colcon_cd_root=/opt/ros/jazzy/
### END COLCON ###
```

### Creating a workspace (1h15')
- Source `overlays` over `underlays` is recommended for working on small number of packages, it avoid to have all in the same workspace and rebuild a huge workspace on every iteration.
- In this process it is cloned a repository for `turtlesim` and build as overlay on top of current `ros2` basic `underlay`

```bash
# From folder: ~/learnRobotic/ros2_ws/src/ , clone tutorial repo
cd ~/learnRobotic/ros2_ws/src/
git clone https://github.com/ros/ros_tutorials.git -b jazzy

# Build just `turtlesim` package from cloned repo
colcon build --packages-up-to=turtlesim
```

* Source Overlay:
   - Sourcing `local_setup` overlay will add the packages of the overlay on the top of your `underlay`.
   - It allows modify and rebuild packages in the overlay separately from the underlay.

- Open a new terminal and source the overlay for turtlesim
```bash
cd ~/learnRobotic/
source ros2_env_conf.sh
cd ~/learnRobotic/ros2_ws
source src/install/local_setup.bash
```
- Here we can modify sources + build -> Changes will apply to our overlay terminal, not to underlay terminal

### Creating a package (1h)
- Package: Organizational unit for ROS2 source
- Package creation in ROS2:
    - `ament`: System of Compilation
    - `colcon`: Toold of building
    - `CMake` or `Python` officials for creating packages
#### Package minimum requirement content
**CMAKE**:
- `CMakeLists.txt`: File. Describes how to build the code
- `include/<package_name>`: Folder. Containing public headers
- `package.xml`: File. Containing meta information
- `src`: Folder. Contains source code
**Python**:
- `package.xml`: File. Meta information
- `resource/<package_name>`: File. Marker for the package
- `setup.cfg`: File. Required when a package has executables, so ros2 run can find them
- `setup.py`: File. Instructions for how to install the package
- `<package_name>`: Folder. Same name as your package, used by ROS 2 tools to find your package, contains __init__.py

#### Packages in a workspace
- Workspace can contain many packages of same or different types (CMake, Python, etc...)
- Not allowed nested packages
- Best practice: 
    - `src` folder inside workspace 
    - Packages folders inside ´src´

Examples. In new terminal:
```bash
# Init environment
cd ~/learnRobotic/
source ros2_env_conf.sh
cd ~/learnRobotic/ros2_ws/src
# Sintaxis to create packages:
#   CPP:    ros2 pkg create --build-type ament_cmake --license Apache-2.0 <package_name>
#   PYTON:  ros2 pkg create --build-type ament_python --license Apache-2.0 <package_name>
# Create CPP package with node/executable 'my_node'
ros2 pkg create --build-type ament_cmake --license Apache-2.0 --node-name my_node my_package_cpp
# Create Python package with node/executable 'my_node'
ros2 pkg create --build-type ament_python --license Apache-2.0 --node-name my_node my_package_py
# Build both packages
colcon build --packages-select my_package_py my_package
```

- RUN Built Packages in a new Terminal:
```bash
# Source ROS2 environment
cd ~/learnRobotic/
source ros2_env_conf.sh
# Source overlay workspace
cd ~/learnRobotic/ros2_ws/
source install/local_setup.bash
# Run nodes
ros2 run my_package_py my_node
ros2 run my_package my_node
```

### Writing a simple publisher and subscriber (C++) (2h + WIP)
- Example of minimal publisher and subscriber source: [HERE](https://github.com/ros2/examples/tree/jazzy/rclcpp/topics)

**Publisher Node**
- In new terminal create Package:
```bash
# Source ROS2 environment
cd ~/learnRobotic/
source ros2_env_conf.sh

# Create Package: cpp_pubsub
cd ~/learnRobotic/ros2_ws/src
ros2 pkg create --build-type ament_cmake --license Apache-2.0 cpp_pubsub
```
- Download subscriber source:
```bash
# Download talker example source
cd ~/learnRobotic/ros2_ws/src/cpp_pubsub/src
wget -O publisher_lambda_function.cpp https://raw.githubusercontent.com/ros2/examples/jazzy/rclcpp/topics/minimal_publisher/lambda.cpp
# Navegate to new package and configure it
cd ..
```
- Modify `package.xml`
    - Modify `description` 
    - Add dependencies: </br>
        `<depend>rclcpp</depend> 
        <depend>std_msgs</depend>`
- Modify `CMakeList.txt`
    - Find dependencies: </br>
       `find_package(rclcpp REQUIRED)
        find_package(std_msgs REQUIRED)`
    - Add executable and dependencies: </br>
        `add_executable(talker src/publisher_lambda_function.cpp)
        ament_target_dependencies(talker rclcpp std_msgs)`
    - Add install section
        `install(TARGETS talker DESTINATION lib/${PROJECT_NAME})`

**Subscriber Node**
- Download subscriber source. New Terminal:
```bash
# Source ROS2 environment
cd ~/learnRobotic/
source ros2_env_conf.sh
# Download listener example source
cd ~/learnRobotic/ros2_ws/src/cpp_pubsub/src
wget -O subscriber_lambda_function.cpp https://raw.githubusercontent.com/ros2/examples/jazzy/rclcpp/topics/minimal_subscriber/lambda.cpp
# Navegate to new package and configure it
cd ..
```
- Modify `CMakeList.txt` to add subscriber:
    - Add executable and dependencies: </br>
        `add_executable(listener src/subscriber_lambda_function.cpp)
        ament_target_dependencies(listener rclcpp std_msgs)`
    - Add install section
        `install(TARGETS talker listener DESTINATION lib/${PROJECT_NAME})`

**Run Subscriber and Publisher Node**
- Build package.New terminal:
```bash
# Check dependencies
cd ~/learnRobotic/ros2_ws/
source ros2_env_conf.sh
rosdep install -i --from-path src --rosdistro jazzy -y
# Build package cpp_pubsub
colcon build --packages-select cpp_pubsub
```

- RUN Subscriber. New terminal:
```bash
# ROS2 environment
cd ~/learnRobotic/
source ros2_env_conf.sh
# workspace overlay
cd ros2_ws
. install/setup.bash
# RUN talker
ros2 run cpp_pubsub talker
```

- RUN Publisher. New terminal:
```bash
# ROS2 environment
cd ~/learnRobotic/
source ros2_env_conf.sh
# workspace overlay
cd ros2_ws
. install/setup.bash
# RUN listener
ros2 run cpp_pubsub listener
```
