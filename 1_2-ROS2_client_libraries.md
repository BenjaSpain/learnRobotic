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
cd ~/learnRobotic
source ros2_env_conf.sh
cd ros2_ws/
source install/setup.bash
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

## Writing a simple service and client (Python) (1h)
### Create Package py_pubsub and configure it
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

### Write service node
- Write service: `~/learnRobotic/ros2_ws/src/py_srvcli/py_srvcli/service_member_function.py`:
```python
from example_interfaces.srv import AddTwoInts

import rclpy
from rclpy.node import Node


class MinimalService(Node):

    def __init__(self):
        super().__init__('minimal_service')
        self.srv = self.create_service(AddTwoInts, 'add_two_ints', self.add_two_ints_callback)

    def add_two_ints_callback(self, request, response):
        response.sum = request.a + request.b
        self.get_logger().info('Incoming request\na: %d b: %d' % (request.a, request.b))

        return response


def main():
    rclpy.init()

    minimal_service = MinimalService()

    rclpy.spin(minimal_service)

    rclpy.shutdown()


if __name__ == '__main__':
    main()
```

### Write client node
- Write client: `~/learnRobotic/ros2_ws/src/py_srvcli/py_srvcli/client_member_function.py`:
```python
import sys

from example_interfaces.srv import AddTwoInts
import rclpy
from rclpy.node import Node


class MinimalClientAsync(Node):

    def __init__(self):
        super().__init__('minimal_client_async')
        self.cli = self.create_client(AddTwoInts, 'add_two_ints')
        while not self.cli.wait_for_service(timeout_sec=1.0):
            self.get_logger().info('service not available, waiting again...')
        self.req = AddTwoInts.Request()

    def send_request(self, a, b):
        self.req.a = a
        self.req.b = b
        return self.cli.call_async(self.req)


def main():
    rclpy.init()

    minimal_client = MinimalClientAsync()
    future = minimal_client.send_request(int(sys.argv[1]), int(sys.argv[2]))
    rclpy.spin_until_future_complete(minimal_client, future)
    response = future.result()
    minimal_client.get_logger().info(
        'Result of add_two_ints: for %d + %d = %d' %
        (int(sys.argv[1]), int(sys.argv[2]), response.sum))

    minimal_client.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()
```

### RUN client-service
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
cd ~/learnRobotic/
source ros2_env_conf.sh
cd ros2_ws
source install/setup.bash
ros2 run py_srvcli client 6 8
```

- Run service. New terminal:
```bash
# Source environment
cd ~/learnRobotic/
source ros2_env_conf.sh
cd ros2_ws
source install/setup.bash
ros2 run py_srvcli service
```