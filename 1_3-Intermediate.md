# Intermediate (WiP)
[Intermediate](https://docs.ros.org/en/jazzy/Tutorials/Intermediate.html)

## Managing Dependencies with rosdep (30')
### What is `rosdep`?
- Open-source command-line utility for identifying and installing dependencies to build or install packages
- Use `apt` (Debian/Ubuntu) or `dnf` (Fedora/RHEL) to install dependencies

### About `package.xml` files
- `package.xml` is the file where `rosdep` find the set of dependencies
- Dependencies in `package.xml` generally referred as `rosped keys`
- Some relevant tags:
    - `<depend>`
        - Dependencies for build and run time
        - Notes:
            - `C++` packages: If doubt, use this tag for dependencies
            - `Python` packages: Never use this tag, use `<exec_depend>`
    - `<build_depend>`:
        - Dependencies only required for build, not for executions
        - Can create problems if package exports a header that include header from this dependency. In this case use `<build_export_depend>`
    - `<build_export_depend>`:
        - If export a header that includes a header from a dependency, it will be needed by other packages that `<build_depend>` on your package
        - Library packages referenced by libraries you export should normally specify `<depend>`, because they are also needed at execution time
    - `<exec_depend>`:
        - Dependencies while running package for *shared libraries*, *excutables*, *Python modules*, *launch scripts* and others
    - `<test_depend>`:
        - Dependencies needed **only** by test.

### How does `rosdep` work?
1) `rosdep` check `package.xml` files in current path or specific package to find `rosdep keys` stored within
2) Find appropiate ROS packages or software library in various package managers 
3) Install its
- Central index is known as `rosdistro`

### What keys to put in my `package.xml`?
- If package is **ROS-Based** AND released into ROS ecosystem
    - Just use name of the package
    - * List of all released ROS packages https://github.com/ros/rosdistro in your given ROS distribution (*`<distro>/distribution.yaml`*)
- If package is **non-ROS Based**
    - System dependencies need to find keys for a particular library
    - In teneral, 2 files of interes:
        - [rosdep/base.yaml](https://github.com/ros/rosdistro/blob/master/rosdep/base.yaml): ´apt´ system dependencies
        - [rosdep/python.yaml](https://github.com/ros/rosdistro/blob/master/rosdep/python.yaml): Python dependencies
- Steps:
1) Search for your library key in these files and find the name
2) Put key in a `package.xml` file

### What if library is not in `rosdistro`?
- You can add it by Pull request to rosdistro
- [Instructions to rosdistro contributions](https://github.com/ros/rosdistro/blob/master/CONTRIBUTING.md#rosdep-rules-contributions)

### How use `rosdep` tool?
- `rosdep` installation:
    - `rosdep` is packaged along ROS. Add package to system (recommended): `apt-get install python3-rosdep`
        ```bash
        # Init workspace and move to root
        cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws
        # Install rosdep package
        sudo apt-get install python3-rosdep
        ```
    - To use `rosdep` outside ROS. Install it directly:  `pip install rosdep`
- `rosdep` operation:
    - First time:
    ```bash
    # Initialize rosdep
    sudo rosdep init
    # Update local cached rosdistro index
    rosdep update
    ```
    - Run `rosdep` to install dependencies:
        ```bash
        rosdep install --from-paths src -y --ignore-src
        ```
## Creating an action (10')
- Steps to define and build an action that can be used with action server/client
- Creating interface package `custom_action_interfaces`
    ```bash
    cd ~/learnRobotic/ && source ros2_env_conf.sh
    mkdir -p ~/learnRobotic/ros2_ws/src # you can reuse an existing workspace with this naming convention
    cd ~/learnRobotic/ros2_ws/src
    ros2 pkg create --build-type ament_cmake --license Apache-2.0 custom_action_interfaces
    ```

- Defining an action
    - Action definition is made upt 3 message definitions:
        - *request*: Initiating new goal
        - *result*: goal result
        - *Feedback*: Periodically semt with update about a goal
    - Create `action/Fibonacci.action`:
    ```bash
    cd ~/learnRobotic/ros2_ws/src/custom_action_interfaces
    # Create action interface
    mkdir action
    touch action/Fibonacci.action
    ```
    - Update interface into `action/Fibonacci.action`:
     ```
     int32 order
    ---
    int32[] sequence
    ---
    int32[] partial_sequence
     ```

- Building an action:
    - Before being able to use this Fibonacci action type, it is need pass definition to `rosidl` code generation pipeline
    - Add to `CMakeLists.txt`
        ```
        find_package(rosidl_default_generators REQUIRED)

        rosidl_generate_interfaces(${PROJECT_NAME}
            "action/Fibonacci.action"
        )
        ```
    - Add dependencies to `package.xml`
        ```
        <version>0.0.1</version>
        <description>Custom action interfaces for ROS 2</description>
        <buildtool_depend>rosidl_default_generators</buildtool_depend>

        <member_of_group>rosidl_interface_packages</member_of_group>
        ```
    - Build package `Fibonacci`
        ```bash
        cd ~/learnRobotic/ros2_ws # Change to the root of the workspace
        colcon build --packages-select custom_action_interfaces
        ```

## Writing an action server and client - C++ (2h)

### Creating `custom_action_cpp` package
- Create package `custom_action_cpp`:
    ```bash
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws/src
    ros2 pkg create --dependencies custom_action_interfaces rclcpp rclcpp_action rclcpp_components --license Apache-2.0 -- custom_action_cpp
    ```
- Add visibility control header (to work and compile in Windows): `~/learnRobotic/ros2_ws/src/custom_action_cpp/include/custom_action_cpp/visibility_control.hpp`


### Writing action server and client
- Create action server source: `~/learnRobotic/ros2_ws/src/custom_action_cpp/src/fibonacci_action_server.cpp`
- Create action client source: `~/learnRobotic/ros2_ws/src/custom_action_cpp/src/fibonacci_action_client.cpp`


### Configure compilation and build
- Configure compilation. Add to `CMakeLists.txt`:
    ```
    add_library(action_server SHARED src/fibonacci_action_server.cpp)
    target_include_directories(action_server PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>)
    target_compile_definitions(action_server PRIVATE "CUSTOM_ACTION_CPP_BUILDING_DLL")
    ament_target_dependencies(action_server "custom_action_interfaces" "rclcpp" "rclcpp_action" "rclcpp_components")
    rclcpp_components_register_node(action_server PLUGIN "custom_action_cpp::FibonacciActionServer" EXECUTABLE fibonacci_action_server)
    install(TARGETS action_server ARCHIVE DESTINATION lib LIBRARY DESTINATION lib RUNTIME DESTINATION bin)

    add_library(action_client SHARED src/fibonacci_action_client.cpp) 
    target_include_directories(action_client PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>)
    target_compile_definitions(action_client PRIVATE "CUSTOM_ACTION_CPP_BUILDING_DLL")
    ament_target_dependencies(action_client "custom_action_interfaces" "rclcpp" "rclcpp_action" "rclcpp_components")
    rclcpp_components_register_node(action_client PLUGIN "custom_action_cpp::FibonacciActionClient" EXECUTABLE fibonacci_action_client)
    install(TARGETS action_client ARCHIVE DESTINATION lib LIBRARY DESTINATION lib RUNTIME DESTINATION bin)
    ```
- Build package `custom_action_cpp`:
    ```bash
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws
    colcon build --packages-select custom_action_cpp
    ```

### Run
- Run action server. New terminal:
    ```bash
    # Source environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash
    ros2 run custom_action_cpp fibonacci_action_server
    ```

- Run action server. New terminal:
    ```bash
    # Source environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash
    ros2 run custom_action_cpp fibonacci_action_client
    ```

## Writing an action server and client - Python (2h)

- Elaborate asyncronous action server an client un python
- Add progressively complexity to the implementation: goal_handler, result resturn, feedback

### Sources
- Action server: ~/learnRobotic/ros2_ws/src/py_server_client/fibonacci_action_server.py
- Action client: ~/learnRobotic/ros2_ws/src/py_server_client/fibonacci_action_client.py


## Writing an Composable Node (C+) - (30')
- Case: 
    - You have a regular `rclcpp::Node` executable 
    - You need to runt it in the same process as other nodes to enable more efficient communication

### Class of Node

- New Node Class
    - Include argument in constructor 'NodeOptions' 
    - Not main. Invocation to macro `RCLCPP_COMPONENTS_REGISTER_NODE`
    ```cpp
    namespace palomino
    {
        /* ATTENTION: Class Constructor takes `NodeOptions` argument */
        class VincentDriver(const rclcpp::NodeOptions & options) : Node("vincent_driver", options) : public rclcpp::Node
        {
            // ...
        };
    }
    /* ATTENTION: Replace 'main' with a pluginlib-style macro invocation. */
    #include <rclcpp_components/register_node_macro.hpp>
    RCLCPP_COMPONENTS_REGISTER_NODE(palomino::VincentDriver)
    //    int main(int argc, char * argv[])
    //    {
    //        rclcpp::init(argc, argv);
    //        rclcpp::spin(std::make_shared<palomino::VincentDriver>());
    //        rclcpp::shutdown();
    //        return 0;
    //    }  
    ```
- Adjust your Python launch file
    - Need to import `ComposableNodeContainer` and `ComposableNode`
    - Add the `CompostableNodeContainer` action to the launcher
    ```py
    # REPLACE THIS:
    ##from launch_ros.actions import Node
    # ..

    ##ld.add_action(Node(
    ##    package='palomino',
    ##    executable='vincent_driver',
        # ..
    ##))
    # WITH THIS:
    from launch_ros.actions import ComposableNodeContainer
    from launch_ros.descriptions import ComposableNode

    # ..
    ld.add_action(ComposableNodeContainer(
        name='a_buncha_nodes',
        namespace='',
        package='rclcpp_components',
        executable='component_container',
        composable_node_descriptions=[
            ComposableNode(
                package='palomino',
                plugin='palomino::VincentDriver',
                name='vincent_driver',
                # ..
                extra_arguments=[{'use_intra_process_comms': True}],
            ),
        ]
    ))
    ```

### Configure package
- `CMakeLists.txt`
    1. Add `rclcpp_components` as dependency
        ´find_package(rclcpp_components REQUIRED)`
    2. Remove typical executable configurations:
        - `add_executable`: `add_executable(vincent_driver src/vincent_driver.cpp)`
        - `install`:        `install(TARGETS vincent_driver DESTINATION lib/${PROJECT_NAME}`
    3. Add library resource: `add_library(vincent_driver_component SHARED src/vincent_driver.cpp)`
    4. Replace other build commands to the new tarjet. Example
        `ament_target_dependencies(vincent_driver ...)` becomes `ament_target_dependencies(vincent_driver_component "rclcpp_components" ...)`
    5. Declare your component
        `rclcpp_components_register_node(
            vincent_driver_component
            PLUGIN "palomino::VincentDriver"
            EXECUTABLE vincent_driver
        )`
    6. Change installation commands in CMake to install as library in spite of old executable way
        - Do not install target into `lib/${PROJECT_NAME}` but library installation
        ```
        ament_export_targets(export_vincent_driver_component)
        install(TARGETS vincent_driver_component
                EXPORT export_vincent_driver_component
                ARCHIVE DESTINATION lib
                LIBRARY DESTINATION lib
                RUNTIME DESTINATION bin
        )
        ```
- `package.xml`
    - Add dependencies for both build and execute`rclcpp_components`: `<depend>rclcpp_components</depend>`
    - Add more specific dependencies just for build, exec or debug if need

## Composing multiples Nodes in a single process (WiP)
### Run demos
- Discover available components:
```bash
    # Should be at least:
    #composition
    #   composition::Talker
    #   composition::Listener
    #   composition::NodeLikeListener
    #   composition::Server
    #   composition::Client
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash
    ros2 component types
```

#### Run-time composition using ROS services with a publisher and subcriber
- We use 2 shells:
    - SHELL 1. Start component Container. Run composed Nodes:
        `ros2 run rclcpp_components component_container`
    - SHELL 2. Check and launch Nodes into component container running on SHELL 1:
```bash
    # Verify that '/ComponentManager' container is running
    ros2 component list
    # Launch Talker. Will run in SHELL 1
    ros2 component load /ComponentManager composition composition::Talker
    # Launch Listener. Wwill run in SHELL 1
    ros2 component load /ComponentManager composition composition::Listener
    # Inspect state of container. Should show:
    # /ComponentManager
    #   1   /talker
    #   2   /listener
    ros2 component list
```
#### Run-time composition using ROS services with a server and client
- We use 2 shells:
    - SHELL 1. Start component Container. Run composed Nodes:
        `ros2 run rclcpp_components component_container`
    - SHELL 2. Check and launch Nodes into component container running on SHELL 1:
```bash
    # Launch Server. Will run in SHELL 1
    ros2 component load /ComponentManager composition composition::Server
    # Launch Client. Will run in SHELL 1
    ros2 component load /ComponentManager composition composition::Client
```
#### Compile-time composition with hardcoded nodes
....Pending to complete or leave without complete



## Using the Nodes Interfaces Template Class - C++ (30')
- `rclcpp::NodeInterfaces<>`:
    - Reliable and compact interface for all ROS node types
    - Template class that provides a compact and efficient way to manage Node Interfaces

### Accesing Node Information with a SharedPtr
- Example with simple `Node`. 
    - Include function that accepts `SharedPtr`
    - Create a Object of type `Node` and print nodeName
```cpp
    #include <memory>
    #include "rclcpp/rclcpp.hpp"

    void node_info(rclcpp::Node::SharedPtr node)
    {
    RCLCPP_INFO(node->get_logger(), "Node name: %s", node->get_name());
    }

    class SimpleNode : public rclcpp::Node
    {
    public:
    SimpleNode(const std::string & node_name)
    : Node(node_name)
    {
    }
    };

    int main(int argc, char * argv[])
    {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<SimpleNode>("Simple_Node");
    node_info(node);
    }
```
### Explicitly pass `rclcpp::node_interfaces`
- To make code more flexible and compatible with different node types, we use `rclcpp::NodeInterfaces`
- Example: Create different types of Node and pass their interfaces using `rclcpp::node_interfaces`. Access to properties of the Nodes:
```cpp
    void node_info(std::shared_ptr<rclcpp::node_interfaces::NodeBaseInterface> base_interface,
               std::shared_ptr<rclcpp::node_interfaces::NodeLoggingInterface> logging_interface)
    {
    RCLCPP_INFO(logging_interface->get_logger(), "Node name: %s", base_interface->get_name());
    }

    class SimpleNode : public rclcpp::Node
    {
    public:
    SimpleNode(const std::string & node_name)
    : Node(node_name)
    {
    }
    };

    class LifecycleTalker : public rclcpp_lifecycle::LifecycleNode
    {
    public:
    explicit LifecycleTalker(const std::string & node_name, bool intra_process_comms = false)
    : rclcpp_lifecycle::LifecycleNode(node_name,
        rclcpp::NodeOptions().use_intra_process_comms(intra_process_comms))
    {}
    }

    int main(int argc, char * argv[])
    {
    rclcpp::init(argc, argv);
    rclcpp::executors::SingleThreadedExecutor exe;
    auto node = std::make_shared<SimpleNode>("Simple_Node");
    auto lc_node = std::make_shared<LifecycleTalker>("Simple_LifeCycle_Node");
    node_info(node->get_node_base_interface(),node->get_node_logging_interface());
    node_info(lc_node->get_node_base_interface(),lc_node->get_node_logging_interface());
    }
```

### Using rclcpp::NodeInterfaces
- `rclcpp::NodeInterfaces`: Recommended way of accesing a `Node` type's
- - Example: Create different types of Node and pass their interfaces using `rclcpp::node_interfaces`. Access to Types of the Nodes:
```cpp
    #include <memory>
    #include <string>
    #include <thread>
    #include "lifecycle_msgs/msg/transition.hpp"
    #include "rclcpp/rclcpp.hpp"
    #include "rclcpp_lifecycle/lifecycle_node.hpp"
    #include "rclcpp_lifecycle/lifecycle_publisher.hpp"
    #include "rclcpp/node_interfaces/node_interfaces.hpp"

    using MyNodeInterfaces =
    rclcpp::node_interfaces::NodeInterfaces<rclcpp::node_interfaces::NodeBaseInterface, rclcpp::node_interfaces::NodeLoggingInterface>;

    void node_info(MyNodeInterfaces interfaces)
    {
    auto base_interface = interfaces.get_node_base_interface();
    auto logging_interface = interfaces.get_node_logging_interface();
    RCLCPP_INFO(logging_interface->get_logger(), "Node name: %s", base_interface->get_name());
    }

    class SimpleNode : public rclcpp::Node
    {
    public:
    SimpleNode(const std::string & node_name)
    : Node(node_name)
    {
    }
    };

    class LifecycleTalker : public rclcpp_lifecycle::LifecycleNode
    {
    public:
    explicit LifecycleTalker(const std::string & node_name, bool intra_process_comms = false)
    : rclcpp_lifecycle::LifecycleNode(node_name,
        rclcpp::NodeOptions().use_intra_process_comms(intra_process_comms))
    {}
    };

    int main(int argc, char * argv[])
    {
    rclcpp::init(argc, argv);
    rclcpp::executors::SingleThreadedExecutor exe;
    auto node = std::make_shared<SimpleNode>("Simple_Node");
    auto lc_node = std::make_shared<LifecycleTalker>("Simple_LifeCycle_Node");
    node_info(*node);
    node_info(*lc_node);
    }
```
