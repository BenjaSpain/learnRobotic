# Client libraries (WIP)
[Beginner: Client libraries](https://docs.ros.org/en/jazzy/Tutorials/Beginner-Client-Libraries/Colcon-Tutorial.html)

## Using `colcon` to build packages (1h15')
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

### Creating your own package
- `colon` uses `package.xml` specifications
- Recommended build types are: ament_cmake, ament_python. Also suported cmake
- To create package based on template can use: `ros2 pkg create`

### Setup `colcon_cd`
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

## Creating a workspace (1h15')
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

## Creating a package (1h)
- Package: Organizational unit for ROS2 source
- Package creation in ROS2:
    - `ament`: System of Compilation
    - `colcon`: Toold of building
    - `CMake` or `Python` officials for creating packages
### Package minimum requirement content
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

### Packages in a workspace
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

## Writing a simple publisher and subscriber (C++) (4h)
- Example of minimal publisher and subscriber source: [HERE](https://github.com/ros2/examples/tree/jazzy/rclcpp/topics)

### Publisher Node
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

### Subscriber Node
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

### Run Subscriber and Publisher Node
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
## Writing a simple publisher and subscriber (Python) (1h)

### Create Package py_pubsub
- In new terminal create Package:
```bash
# Source ROS2 environment
cd ~/learnRobotic/
source ros2_env_conf.sh

# Create Package: py_pubsub
cd ~/learnRobotic/ros2_ws/src
ros2 pkg create --build-type ament_python --license Apache-2.0 py_pubsub
```

### Write publisher Node
```bash
# Source ROS2 environment
cd ~/learnRobotic/
source ros2_env_conf.sh

# Download sources of Publisher
cd ~/learnRobotic/ros2_ws/src/py_pubsub/py_pubsub
wget https://raw.githubusercontent.com/ros2/examples/jazzy/rclpy/topics/minimal_publisher/examples_rclpy_minimal_publisher/publisher_member_function.py
```

### Write subscriber Node
```bash
# Download subscriber source
cd ~/learnRobotic/ros2_ws/src/py_pubsub/py_pubsub
wget https://raw.githubusercontent.com/ros2/examples/jazzy/rclpy/topics/minimal_subscriber/examples_rclpy_minimal_subscriber/subscriber_member_function.py
```

### Configure Package
- Configure metadata:`~/learnRobotic/ros2_ws/src/py_pubsub/package.xml`
    - Modify description:  
        `<description>Examples of minimal publisher/subscriber using rclpy</description>`
    - Add dependencies:  
        `<exec_depend>rclpy</exec_depend>
        <exec_depend>std_msgs</exec_depend>`
- Configure:`~/learnRobotic/ros2_ws/src/py_pubsub/setup.py`  
    `from setuptools import find_packages, setup

    package_name = 'py_pubsub'

    setup(
        name=package_name,
        version='0.0.1',
        packages=find_packages(exclude=['test']),
        data_files=[
            ('share/ament_index/resource_index/packages',
                ['resource/' + package_name]),
            ('share/' + package_name, ['package.xml']),
        ],
        install_requires=['setuptools'],
        zip_safe=True,
        maintainer='benjamincanton',
        maintainer_email='55950461+BenjaSpain@users.noreply.github.com',
        description='Examples of minimal publisher/subscriber using rclpy',
        license='Apache-2.0',
        extras_require={
            'test': [
                'pytest',
            ],
        },
        entry_points={
            'console_scripts': [
                'talker = py_pubsub.publisher_member_function:main',
                'listener = py_pubsub.subscriber_member_function:main',            
            ],
        },
    )`

### Build and Run
- Build Package:
```bash
# Check dependencies
cd ~/learnRobotic/ros2_ws
rosdep install -i --from-path src --rosdistro jazzy -y
# Build package
colcon build --packages-select py_pubsub
```

- Run Publisher. New Terminal:
```bash
cd ~/learnRobotic && source ros2_env_conf.sh && cd ros2_ws/ && source install/setup.bash
ros2 run py_pubsub talker
```

- Run Subscriber. New Terminal:
```bash
cd ~/learnRobotic
source ros2_env_conf.sh
cd ros2_ws/
source install/setup.bash
ros2 run py_pubsub listener
```


## Writing a simple service and client (CPP) (1h)
### Create Package cpp_srvcli and configure it
- In new terminal create Package:
```bash
# Source ROS2 environment
cd ~/learnRobotic && source ros2_env_conf.sh && cd ros2_ws/ && source install/setup.bash

# Create Package: py_srvcli, adding dependencies
cd ~/learnRobotic/ros2_ws/src
ros2 pkg create --build-type ament_cmake --license Apache-2.0 cpp_srvcli --dependencies rclcpp example_interfaces
```
- Update `package.xml`:  
    ```
    <version>0.0.1</version>
    <description>C++ client server tutorial</description> 
    ```
- Add to `CMakeList.txt` info to build and run server/client
    ```
    # Indicate executable, dependencies and installation for the server/client
    add_executable(server src/add_two_ints_server.cpp)
    add_executable(client src/add_two_ints_client.cpp)
    ament_target_dependencies(server rclcpp example_interfaces)
    ament_target_dependencies(client rclcpp example_interfaces)
    install(TARGETS server client DESTINATION lib/${PROJECT_NAME})
    ```

### Nodes Sources
- Source file: `~/learnRobotic/ros2_ws/src/cpp_srvcli/src/add_two_ints_server.cpp`
- Source file: `~/learnRobotic/ros2_ws/src/cpp_srvcli/src/add_two_ints_client.cpp`

### BUILD and RUN client-service
- Build package
```bash
# Check dependencies
cd ~/learnRobotic/ros2_ws
rosdep install -i --from-path src --rosdistro jazzy -y
# Build package
colcon build --packages-select cpp_srvcli
```
- Run client. New terminal:
```bash
# Source environment
cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash
# Run Client
ros2 run cpp_srvcli client 6 8
```

- Run service. New terminal:
```bash
# Source environment
cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash
# Run Server
ros2 run cpp_srvcli server
```

## Writing a simple service and client (Python) (1h)
### Create Package py_srvcli and configure it
- In new terminal create Package:
```bash
# Source ROS2 environment
cd ~/learnRobotic/
source ros2_env_conf.sh

# Create Package: py_srvcli
#   - --dependencies: Add dependencies to package.xml
cd ~/learnRobotic/ros2_ws/src
ros2 pkg create --build-type ament_python --license Apache-2.0 py_srvcli --dependencies rclpy example_interfaces
```

- Configure:`~/learnRobotic/ros2_ws/src/py_srvcli/resource/py_srvcli`  
    `int64 a`  
    `int64 b`  
    `---`  
    `int64 sum`

- Modify package metadata `~/learnRobotic/ros2_ws/src/py_srvcli/package.xml`. Only need description, dependencies already added on package creation:
    `<version>0.0.1</version>`  
    `<description>Python client server tutorial</description>`

- MOdify package setup `~/learnRobotic/ros2_ws/src/py_srvcli\setup.py`.
    `version='0.0.1',`  
    `description='Python client server tutorial',`  
    `entry_points={`  
        `'console_scripts': [`  
            `'service = py_srvcli.service_member_function:main',`  
            `'client = py_srvcli.client_member_function:main',`  
        `],`  
    `},`

### Nodes Sources
- Write service: `~/learnRobotic/ros2_ws/src/py_srvcli/py_srvcli/service_member_function.py`:
- Write client: `~/learnRobotic/ros2_ws/src/py_srvcli/py_srvcli/client_member_function.py`:


### BUILD and RUN client-service
- Build package
```bash
# Check dependencies
cd ~/learnRobotic/ros2_ws
rosdep install -i --from-path src --rosdistro jazzy -y
# Build package
colcon build --packages-select py_srvcli
```
- Run client. New terminal:
```bash
# Source environment
cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash
ros2 run py_srvcli client 6 8
```

- Run service. New terminal:
```bash
# Source environment
cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash
ros2 run py_srvcli service
```

## Creating custom msg and srv files (2h)
### Create Package and definitions
```bash
# Source environment
cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash

# Create package
cd ~/learnRobotic/ros2_ws/src/
ros2 pkg create --build-type ament_cmake --license Apache-2.0 tutorial_interfaces

# msg definition
cd tutorial_interfaces/
mkdir msg srv
cd msg
touch Num.msg
# Content of Num.msg:
##  int64 num
touch Sphere.msg
# Content of Sphere.msg:
##  geometry_msgs/Point center
##  float64 radius

# srv definition
cd ../srv
touch AddThreeInts.srv
# Content of Num.msg:
##  int64 a 
## int64 b
## int64 c
## ---
## int64 sum
```
- Add to `CMakelist.txt` to convert custom interfaces into language-specific code (C++, Python):  
    ```find_package(geometry_msgs REQUIRED)
    find_package(rosidl_default_generators REQUIRED)

    rosidl_generate_interfaces(${PROJECT_NAME}
    "msg/Num.msg"
    "msg/Sphere.msg"
    "srv/AddThreeInts.srv"
    DEPENDENCIES geometry_msgs # Add packages that above messages depend on, in this case geometry_msgs for Sphere.msg
    )
    ```
- Add to `package.xml` dependencies:
    ```<depend>geometry_msgs</depend>
    <buildtool_depend>rosidl_default_generators</buildtool_depend>
    <exec_depend>rosidl_default_runtime</exec_depend>
    <member_of_group>rosidl_interface_packages</member_of_group>
    ```
    
### Build package
```bash
# Create environment
cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash
cd ~/learnRobotic/ros2_ws
colcon build --packages-select tutorial_interfaces
```

### Confirm creation of interfaces
- New terminal:
```bash
# Create environment
cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash
ros2 interface show tutorial_interfaces/msg/Num
ros2 interface show tutorial_interfaces/msg/Sphere
ros2 interface show tutorial_interfaces/srv/AddThreeInts
```

### Testing Num.msg with pub/sub - CPP
- `cpp_pubsub` package adapted to use our custom interface `Num.msg`:
    - Specific sources using `Num.msg` in spite of `<String>`
        - `publisher_lambda_function_interf.cpp`
        - `subscriber_lambda_function_interf.cpp`
    - Add to `CMakeLists.txt`:
        ```
        find_package(tutorial_interfaces REQUIRED)                      # CHANGE
        add_executable(talker_interfaces src/publisher_lambda_function_interf.cpp)       # CHANGE
        add_executable(listener_interfaces src/subscriber_lambda_function_interf.cpp)    # CHANGE
        ament_target_dependencies(talker_interfaces rclcpp tutorial_interfaces)          # CHANGE
        ament_target_dependencies(listener_interfaces rclcpp tutorial_interfaces)        # CHANGE
        ```
    - Add to `package.xml`:  
        `<depend>tutorial_interfaces</depend>`

### Testing Num.msg with pub/sub - Python
- `py_pubsub` package adapted to use our custom interface `Num.msg`:
    - Specific sources using `Num.msg` in spite of `<String>`
        - Specific sources using `Num.msg` in spite of `<String>`
        - `publisher_member_function_interf.cpp`
        - `subscriber_member_function_interf.cpp`
    - Add to `package.xml`:
        ``` 
        <version>0.0.2</version>
        <description>Examples of minimal publisher/subscriber using rclpy + interface msg.Num</description>
        <exec_depend>tutorial_interfaces</exec_depend> 
        ```

    - Add/modify to `setup.py`:  
        ```  
        version='0.0.2  ',
        description='Examples of minimal publisher/subscriber using rclpy and tutorial_interfaces.msg.Num',
        entry_points={
            'console_scripts': [
                publisher_member_function_interf:main',
                'listener_interf = py_pubsub.subscriber_member_function_interf:main',          
            ],
        },
        ```

### Testing `AddThreeInts.srv` with service/client - CPP
#### Sources
- Source file: `~/learnRobotic/ros2_ws/src/cpp_srvcli/src/add_two_ints_server_interface.cpp`
- Source file: `~/learnRobotic/ros2_ws/src/cpp_srvcli/src/add_two_ints_client_interface.cpp`

#### Update package configuration
- Add/Update `CMakeLists.txt`:
    ```
    find_package(tutorial_interfaces REQUIRED)                                                          # CHANGE
    # Indicate executable, dependencies and installation for the server/client using interfaces
    add_executable(server_interface src/add_two_ints_server_interface.cpp)                              # CHANGE
    add_executable(client_interface src/add_two_ints_client_interface.cpp)                              # CHANGE
    ament_target_dependencies(server_interface rclcpp tutorial_interfaces)                              # CHANGE
    ament_target_dependencies(client_interface rclcpp tutorial_interfaces)                              # CHANGE

    install(TARGETS server client server_interface client_interface DESTINATION lib/${PROJECT_NAME})    # CHANGE
    ```
- Add `package.xml`:  
    ```<depend>tutorial_interfaces</depend>```

#### Build and RUN client_interface , server_interface
```bash
# Build
cd ~/learnRobotic/ros2_ws
colcon build --packages-select cpp_srvcli
```
- Run server_interface. New Terminal:
```bash
cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash
ros2 run cpp_srvcli server_interface
```
- Run client_interface. New Terminal:
```bash
cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash
ros2 run cpp_srvcli client_interface
```
## Implementing custom interfaces (1h)
### Create and configure package
```bash
cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash
cd src
ros2 pkg create --build-type ament_cmake --license Apache-2.0 more_interfaces
# Create 'msg' interface folder
mkdir more_interfaces/msg
# Create msg file
touch more_interfaces/msg/AddressBook.msg
```

- Add content to `AddressBook.msg`
    ```
    uint8 PHONE_TYPE_HOME=0
    uint8 PHONE_TYPE_WORK=1
    uint8 PHONE_TYPE_MOBILE=2

    string first_name
    string last_name
    string phone_number
    uint8 phone_type
    ```

- Add to `package.xml`:
    ```
    <buildtool_depend>rosidl_default_generators</buildtool_depend>

    <exec_depend>rosidl_default_runtime</exec_depend>

    <member_of_group>rosidl_interface_packages</member_of_group>
    ```

- Add to `CMakeLists.txt`
    ```
    find_package(rclcpp REQUIRED)
    # uncomment the following section in order to fill in
    # further dependencies manually.
    # find_package(<dependency> REQUIRED)

    # List of messages to generate
    set(msg_files
    "msg/AddressBook.msg"
    )
    # Generate messages
    rosidl_generate_interfaces(${PROJECT_NAME}
    ${msg_files}
    )

    # Export message runtime dependencies
    ament_export_dependencies(rosidl_default_runtime)

    # Add executable and dependencies
    add_executable(publish_address_book src/publish_address_book.cpp)
    ament_target_dependencies(publish_address_book rclcpp)
    install(TARGETS publish_address_book DESTINATION lib/${PROJECT_NAME})

    # Allow to use messages interfaces generated inside this package
    rosidl_get_typesupport_target(cpp_typesupport_target
    ${PROJECT_NAME} rosidl_typesupport_cpp)

    target_link_libraries(publish_address_book "${cpp_typesupport_target}")
    ```

### Source
- Publisher: `~/learnRobotic/ros2_ws/src/more_interfaces/src/publish_address_book.cpp`


### Build and run
```bash
# Build package
cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash
colcon build --packages-up-to more_interfaces
# RUN publisher
ros2 run more_interfaces publish_address_book
```

- Check that publisher is working. New Terminal:
```bash
# Build package
cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash
# RUN publisher
ros2 topic echo /address_book
```

## Using parameters ina a class - C++ (30')
- Nodes may need to have parameters that can be set from the launch file
### Create and configure Package
- Create package `cpp_parameters`:
    ```bash
    # Build package
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash
    cd src
    ros2 pkg create --build-type ament_cmake --license Apache-2.0 cpp_parameters --dependencies rclcpp
    ```

- Modify `package.xml`:
    ```
    <version>0.0.1</version>`
    <description>C++ parameter tutorial</description>
    ```

- Modify `CMakeList.txt`:
    ```
    # Add source linked to executable and dependedncies
    add_executable(minimal_param_node src/cpp_parameters_node.cpp)
    ament_target_dependencies(minimal_param_node rclcpp)
    
    # Add executable
    install(TARGETS minimal_param_node DESTINATION lib/${PROJECT_NAME}
    )
    ```
### Source
- Node with parameters: `~/learnRobotic/ros2_ws/src/cpp_parameters/src/cpp_parameters_node.cpp`

### Build and Run
- Build package `cpp_parameters`
```bash
# Check dependencies
cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws
rosdep install -i --from-path src --rosdistro jazzy -y
# Build package 'cpp_parameters'
colcon build --packages-select cpp_parameters
```

- RUN execution. New Terminal:
```bash
# Init environment
cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash
# Run 'minimal_param_node'
ros2 run cpp_parameters minimal_param_node
```

### Use `ros2 param` command on running Node
- Meanwhile running Node `minimal_param_node`, check parameter. New terminal:
```bash
# Init environment
cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash
# List parameters of running nodes
ros2 param list
# Check parameter 'my_parameter' Type, Description and Contraints
ros2 param describe /minimal_param_node my_parameter
# Change param 'my_parameter' value to 'earth'
ros2 param set /minimal_param_node my_parameter earth
```

### Change Parameter via launch file
- Create source of launcher with custom parameter value `~/learnRobotic/os2_ws/src/cpp_parameters/launch/cpp_parameters_launch.py`

- Add launch installation to `CMakeLists.txt`
    ```
    install(
    DIRECTORY launch
    DESTINATION share/${PROJECT_NAME}
    )
    ```
- Build project. New terminal
    ```bash
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws
    colcon build --packages-select cpp_parameters
    ```
- Run launcher. New Terminal:
    ```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash
    ros2 launch cpp_parameters cpp_parameters_launch.py
    ```
    - `launch` is executed which launches `minimal_param_node` with custom parameters. First message is showed with the custom parameters values indicated by `launch`, next logs are printed with parameters values forced in `minimal_param_node`
    
## Using parameters in a class - Python (30')
- Set parameters from launch file. Workflow for Python.

### Create package
- Create package `python_parameters`:
    ```bash
    # Build package
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash
    cd src
    ros2 pkg create --build-type ament_python --license Apache-2.0 python_parameters --dependencies rclpy
    ```
- Update `package.xml`:
    ```
    <version>0.0.1</version>
    
    <description>Python parameter tutorial</description>
    ```
- Add entry point, `setup.py`:
    ```
    version='0.0.1',
    description='Python parameter tutorial',

    entry_points={
        'console_scripts': [
            'minimal_param_node = python_parameters.python_parameters_node:main',
        ],
    },
    ```

### Create Sources
- Create `python_parameters_node.py`: `~/learnRobotic/ros2_ws/src/python_parameters/python_parameters/python_parameters_node.py`

### Build and Run
- Check dependencies:
    ```bash
    # Build package
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws
    rosdep install -i --from-path src --rosdistro jazzy -y
    ```
- Build package:
    ```bash
    # Build package
    colcon build --packages-select python_parameters
    ```

- Run Node. New terminal:
    ```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash
    # Run 'minimal_param_node'
    ros2 run python_parameters minimal_param_node
    ```