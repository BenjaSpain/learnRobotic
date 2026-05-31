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

## Composing multiples Nodes in a single process (2h)
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
....¡¡¡¡Pending to complete or leave without complete!!!



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

## Monitoring for parameter changes - C++ (1h30')
- `ParameterEventHandler` class that lets to monitor and respond to own Node's parameter changes

### Create and configure Package
- Create package `cpp_parameter_event_handler` with dependencies
```bash
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws/src
    ros2 pkg create --build-type ament_cmake --license Apache-2.0 cpp_parameter_event_handler --dependencies rclcpp
```

- Update `package.xml`
    ```
    <version>0.0.1</version>
    <description>C++ parameter events client tutorial</description>

    ```
- Update `CMakeLists.txt` to find files for executable and add dependencies
    ```
    add_executable(parameter_event_handler src/parameter_event_handler.cpp)
    ament_target_dependencies(parameter_event_handler rclcpp)

    install(TARGETS parameter_event_handler DESTINATION lib/${PROJECT_NAME}
    )
    ```

### Node source
- Node Source: `~/learnRobotic/ros2_ws/src/cpp_parameter_event_handler/src/parameter_event_handler.cpp`

### Build and Run
- Build `cpp_parameter_event_handler`:
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws
    # Check dependencies
    rosdep install -i --from-path src --rosdistro $ROS_DISTRO -y
    # Build package
    colcon build --packages-select cpp_parameter_event_handler
```
- Run Node. New terminal
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash
    # Run Node
    ros2 run cpp_parameter_event_handler parameter_event_handler
```

- Change Parameter of Running Node. New terminal
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash
    # Run Node
    ros2 param set node_with_parameters an_int_param 43
```

### Extensions
#### Use Case: Monitor Changes to another node's parameters
- From a Node you can check parameter changes of another Node

- On original source: `~/learnRobotic/ros2_ws/src/cpp_parameter_event_handler/src/parameter_event_handler.cpp`
    - Add to Constructor `cb_handler2` remote nodes params monitoring:
```cpp
    // Now, add a callback to monitor any changes to the remote node's parameter. In this
    // case, we supply the remote node name.
    auto cb2 = [this](const rclcpp::Parameter & p) {
        RCLCPP_INFO(
        this->get_logger(), "cb2: Received an update to parameter \"%s\" of type: %s: \"%.02lf\"",
        p.get_name().c_str(),
        p.get_type_name().c_str(),
        p.as_double());
    };
    auto remote_node_name = std::string("parameter_blackboard");
    auto remote_param_name = std::string("a_double_param");
    cb_handle2_ = param_subscriber_->add_parameter_callback(remote_param_name, cb2, remote_node_name);
```
    - Add new callback declaration
```cpp
    private:
    std::shared_ptr<rclcpp::ParameterEventHandler> param_subscriber_;
    std::shared_ptr<rclcpp::ParameterCallbackHandle> cb_handle_;
    std::shared_ptr<rclcpp::ParameterCallbackHandle> cb_handle2_;  // Add this
    };
```
- Rebuild package `cpp_parameter_event_handler`:
    ```bash
        # Init environment
        cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws
        # Run Node
        colcon build --packages-select cpp_parameter_event_handler
    ```

- Run:
    - Run rebuilt executable. New terminal:
    ```bash
        # Init environment
        cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash
        # Run Node
        ros2 run cpp_parameter_event_handler parameter_event_handler
    ```
    - Run the `parameter_blackboard` demo application. New Terminal:
    ```bash
        # Init environment
        cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash
        # Run Node
        ros2 run demo_nodes_cpp parameter_blackboard
    ```
    - Set parameter on `parameter_blackboard`
    ```bash
        # Init environment
        cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws
        # Run Node
        ros2 param set parameter_blackboard a_double_param 3.45
    ```
#### Use Case: Monitor all nodes parameters simultaneously
- To monitor multiple nodes or parameters at the same time, it would be cumbersome to call `add_parameter_callback` once for each of them
- `add_parameter_event_callback` register a single callback that fires when any parameters of any nodes change.

- On original source: `~/learnRobotic/ros2_ws/src/cpp_parameter_event_handler/src/parameter_event_handler.cpp`
    - Add event parameter monitoring and callback
    ```cpp
        this->declare_parameter("another_double_param", 0.0);

        ...

        auto event_cb = [this](const rcl_interfaces::msg::ParameterEvent & parameter_event) {
            RCLCPP_INFO(
            this->get_logger(), "Received parameter event from node \"%s\"",
            parameter_event.node.c_str());

            for (const auto& p : parameter_event.changed_parameters) {
            RCLCPP_INFO(
                this->get_logger(), "Inside event: \"%s\" changed to %s",
                p.name.c_str(),
                rclcpp::Parameter::from_parameter_msg(p).value_to_string().c_str());
            };
        };

        event_cb_handle_ = param_subscriber_->add_parameter_event_callback(event_cb);
    ```

    - Add new callback declaration
    ```cpp
        private:
        ...
        std::shared_ptr<rclcpp::ParameterEventCallbackHandle> event_cb_handle_;
    ```

- Rebuild package `cpp_parameter_event_handler`:
    ```bash
        # Init environment
        cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws
        # Run Node
        colcon build --packages-select cpp_parameter_event_handler
    ```
- Run:
    - Run rebuilt executable. New terminal:
    ```bash
        # Init environment
        cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash
        # Run Node
        ros2 run cpp_parameter_event_handler parameter_event_handler
    ```

    - Set parameters `an_int_param` and `another_double_param`. Check handlers used in each case
    ```bash
        # Init environment
        cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws
        # Set an_int_param to int -> Original CB is launched
        ros2 param set node_with_parameters an_int_param 44
        # Set an_int_param to double -> New param events CB is launched
        ros2 param set node_with_parameters another_double_param 4.4
    ```

## Monitoring for parameter changes - Python (1h)
- Python version of using `ParameterEventHandler` class

### Create and configure Package
```bash
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws/src
    ros2 pkg create --build-type ament_python --license Apache-2.0 python_parameter_event_handler --dependencies rclpy
```

### Configure project
- Update `package.xml`
    ```
    <version>0.0.1</version>
    <description>Python parameter events client tutorial</description>
    ```
- `setup.py`. Add entry point
    ```
    setup(
        ...
        version='0.0.1',
        ...
        description='Python parameter tutorial',
        ...
        entry_points={
            'console_scripts': [
                'node_with_parameters = python_parameter_event_handler.parameter_event_handler:main',
            ],
        },
    )
    ```

### Node source
- Node Source: `~/learnRobotic/ros2_ws/src/python_parameter_event_handler/python_parameter_event_handler/parameter_event_handler.py`


### Build and Run
- Build `python_parameter_event_handler`:
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws
    # Check dependencies:
    rosdep install -i --from-path src --rosdistro $ROS_DISTRO -y
    # Build
    colcon build --packages-select python_parameter_event_handler
```

- Run Node `node_with_parameters`. New Terminal
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash
    # Run Node
    ros2 run python_parameter_event_handler node_with_parameters
```

- Change parameter `an_int_param` value. New Terminal
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws
    # Run Node
    ros2 param set node_with_parameters an_int_param 43
```

### Extensions

#### Use Case: Monitor changes to another node’s parameters
- Objective: Use `ParameterEventHandler` to monitor parameters changes on another Node.

- Update `parameter_event_handler.py/SampleNodeWithParameters` to monitor other node's parameter


- reBuild package. New Terminal
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws
    # Run Node
    colcon build --packages-select python_parameter_event_handler
```
- Run new executable. New Terminal
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash
    # Run Node
    ros2 run python_parameter_event_handler node_with_parameters
```

- Run `parameter_blackboard` application. New Terminal
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash
    # Run Node parameter_blackboard
    ros2 run demo_nodes_cpp parameter_blackboard
```

- Set parameter on the `parameter_blackboard` node. New Terminal
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws
    # Run Change parameter
    ros2 param set parameter_blackboard a_double_param 3.45
```

#### Use Case: Monitor all node parameters simultaneously
- Objective: Use `add_parameter_event_callback` to monitor Multiple parameters simultaneously

- Update `parameter_event_handler.py/SampleNodeWithParameters` to monitor multiple node's  parameter


- reBuild package. New Terminal
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws
    # Run Node
    colcon build --packages-select python_parameter_event_handler
```

- Run new executable with new Even param callback. New Terminal
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash
    # Run Node
    ros2 run python_parameter_event_handler node_with_parameters
```

- Run `parameter_blackboard` application. New Terminal
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash
    # Run Node parameter_blackboard
    ros2 run demo_nodes_cpp parameter_blackboard
```

- Set parameters. New Terminal
    ```bash
        # Init environment
        cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws
        # Set an_int_param
        ros2 param set node_with_parameters an_int_param 44
        # Set another_double_param
        ros2 param set node_with_parameters another_double_param 4.4
    ```

## Launch (4h35')
- Launch files allow to start up and configure multiple executables simultaneously

### Creating a launch file (30')
- Launch file include configurations as programs to run, where to run them, arguments to pass and other ROS-specific conventions
- Launch files as well may monitor state of launched processes, report it and react to changes.
- Tipical formats are:
    - `XML`
    - `YAML`
    - `Python`

- Create work directory inside workspace:
```bash
    cd ~/learnRobotic/ros2_ws && mkdir launch
```

- Write launch file:
    - XML:      `~/learnRobotic/ros2_ws/turtlesim_mimic_launch.xml`
    - YAML:     `~/learnRobotic/ros2_ws/turtlesim_mimic_launch.yaml`
    - Python:   `~/learnRobotic/ros2_ws/turtlesim_mimic_launch.py`

- Run Launch file:
    ```bash
        cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws/launch
        # Choose your launch file format....
        ros2 launch turtlesim_mimic_launch.xml
        #ros2 launch turtlesim_mimic_launch.yaml
        #ros2 launch turtlesim_mimic_launch.py
    ```
    - Launch file in case of provided by a package: 
        - Command: `ros2 launch <package_name> <launch_file_name>`
        - Make sure to add your dependency in the package configuration to avoid missings `launch` resource: `<exec_depend>ros2launch</exec_depend>`

- Check system is launched. New terminal:
```bash
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws
    # Choose your launch file format....
   ros2 topic pub -r 1 /turtlesim1/turtle1/cmd_vel geometry_msgs/msg/Twist "{linear: {x: 2.0, y: 0.0, z: 0.0}, angular: {x: 0.0, y: 0.0, z: -1.8}}"
```

### Integrating launch files into ROS 2 packages (30')
- How add launch file into existing package
ros2 pkg create --build-type ament_python --license Apache-2.0 py_launch_example

#### Create Package `py_launch_example` structure

```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws/src
    # Create package. Case Python
    ros2 pkg create --build-type ament_python --license Apache-2.0 py_launch_example
    # Case Python
    ##  mkdir py_launch_example/launch
```

- Package structure wich content launch files should be:
    - Python package:
        ```
            src/
                py_launch_example/
                    launch/
                    package.xml
                    py_launch_example/
                    resource/
                    setup.cfg
                    setup.py
                    test/
        ```
        - Update `setup.py` to enable colcon to locate launch files

    - C++ package (Add at end of `CMakeLists.txt`, before `ament_package()`):
    ```
        # Install launch files
        install(DIRECTORY launch DESTINATION share/${PROJECT_NAME}/)
    ```

#### Writing the launch file
- XML:      `~/learnRobotic/ros2_ws/src/py_launch_example/launch/my_script_launch.xml`
- YAML:     `~/learnRobotic/ros2_ws/src/py_launch_example/launch/my_script_launch.yaml`
- Python:   `~/learnRobotic/ros2_ws/src/py_launch_example/launch/my_script_launch.py`

#### Build and Run launch file
- Build executable
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws
    # Build package
    colcon build --packages-select py_launch_example
```

- Run. New Terminal
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash
    # Run Node. Use your launch format case
    ## ros2 launch py_launch_example my_script_launch.xml
    ## ros2 launch py_launch_example my_script_launch.py
    ## ros2 launch py_launch_example my_script_launch.yaml
```

### Using substitutions (1h15')
- Used in `arguments` to provide flexibility on describing reusable launch files
- Are `variables` only evaluated during execution of launch file: `launch configuration`, `environment variables`, `python expresions`, etc

#### Create package
- Create Package
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws/src
    # Create package. Python case
    ros2 pkg create --build-type ament_python --license Apache-2.0 py_launch_tutorial
    # Create package. C++ case
    ros2 pkg create --build-type ament_cmake --license Apache-2.0 cpp_launch_tutorial    
```

- Create launch file
```bash
    # Python
    mkdir py_launch_tutorial/launch
    # C++
    mkdir cpp_launch_tutorial/launch
```

- Configure package to use launch files
    - Python. Add to `setup.py`
        ```py
        import os
        from glob import glob
        from setuptools import find_packages, setup

        package_name = 'py_launch_tutorial'

        setup(
            # Other parameters ...
            data_files=[
                # ... Other data files
                # Include all launch files.
                (os.path.join('share', package_name, 'launch'), glob('launch/*'))
            ]
        )
        ```

    - C++. Add to `CMakeLists.txt`, before `ament_package()`
        ```txt
        install(DIRECTORY
                launch
                DESTINATION share/${PROJECT_NAME}/
        )
        ```
#### Create Launch Files
- Parent Launch File, includes variables and references to Substitution launch file:
    - XML:
        - Python: `~/learnRobotic/ros2_ws/src/py_launch_tutorial/launch/example_main_launch.xml`
        - cpp: `~/learnRobotic/ros2_ws/src/cpp_launch_tutorial/launch/example_main_launch.xml`
    - YAML:
        - Python: `~/learnRobotic/ros2_ws/src/py_launch_tutorial/launch/example_main_launch.yaml`
        - cpp: `~/learnRobotic/ros2_ws/src/cpp_launch_tutorial/launch/example_main_launch.yaml`
    - Python:
        - Python: `~/learnRobotic/ros2_ws/src/py_launch_tutorial/launch/example_main_launch.py`
        - cpp: `~/learnRobotic/ros2_ws/src/cpp_launch_tutorial/launch/example_main_launch.py`

- Substitution Launch File, load values to variables, pass actions to launch file, etc:
    - XML:
        - Python: `~/learnRobotic/ros2_ws/src/py_launch_tutorial/launch/example_substitutions_launch.xml`
        - cpp: `~/learnRobotic/ros2_ws/src/cpp_launch_tutorial/launch/example_substitutions_launch.xml`
    - YAML:
        - Python: `~/learnRobotic/ros2_ws/src/py_launch_tutorial/launch/example_substitutions_launch.yaml`
        - cpp: `~/learnRobotic/ros2_ws/src/cpp_launch_tutorial/launch/example_substitutions_launch.yaml`
    - Python:
        - Python: `~/learnRobotic/ros2_ws/src/py_launch_tutorial/launch/example_substitutions_launch.py`
        - cpp: `~/learnRobotic/ros2_ws/src/cpp_launch_tutorial/launch/example_substitutions_launch.py`

#### Build packages
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws
    # Build Package. Python
    colcon build --packages-select py_launch_tutorial
    # Build Package. C+
    colcon build --packages-select cpp_launch_tutorial
```

#### Launch examples
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash
    # Launch your choosen format
    ##  ros2 launch launch_tutorial example_main_launch.yaml
    ##  ros2 launch launch_tutorial example_main_launch.xml
    ##  ros2 launch launch_tutorial example_main_launch.py
```

#### Modify launch arguments
- To check arguments that may be given to launch file
    ```bash
        # Init environment
        cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash
        # Launch your choosen format
        ##  ros2 launch cpp_launch_tutorial example_substitutions_launch.yaml --show-args
        ##  ros2 launch py_launch_tutorial example_substitutions_launch.yaml --show-args
        ##  ros2 launch cpp_launch_tutorial example_substitutions_launch.xml --show-args
        ##  ros2 launch py_launch_tutorial example_substitutions_launch.xml --show-args
        ##  ros2 launch cpp_launch_tutorial example_substitutions_launch.py --show-args
        ##  ros2 launch py_launch_tutorial example_substitutions_launch.py --show-args    
    ```
    - Should show:
    ```
    Arguments (pass arguments as '<name>:=<value>'):

    'turtlesim_ns':
        no description given
        (default: 'turtlesim1')

    'use_provided_red':
        no description given
        (default: 'False')

    'new_background_r':
        no description given
        (default: '200')
    ```

- Pass desired arguments to launch file
    ```bash
        # Init environment
        cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash
        # Launch your choosen format with desired variables
        ##  ros2 launch cpp_launch_tutorial example_substitutions_launch.yaml turtlesim_ns:='turtlesim3' use_provided_red:='True' new_background_r:=200
        ##ros2 launch py_launch_tutorial example_substitutions_launch.yaml turtlesim_ns:='turtlesim3' use_provided_red:='True' new_background_r:=200
        ##  ros2 launch cpp_launch_tutorial example_substitutions_launch.xml turtlesim_ns:='turtlesim3' use_provided_red:='True' new_background_r:=200
        ##ros2 launch py_launch_tutorial example_substitutions_launch.xml turtlesim_ns:='turtlesim3' use_provided_red:='True' new_background_r:=200
        ##  ros2 launch cpp_launch_tutorial example_substitutions_launch.pyturtlesim_ns:='turtlesim3' use_provided_red:='True' new_background_r:=200
        ##ros2 launch py_launch_tutorial example_substitutions_launch.py turtlesim_ns:='turtlesim3' use_provided_red:='True' new_background_r:=200
    ```

### Using event handlers (20')
- Events handlers cab be registered for specific events by launch files

#### Event handler example launch file
- Launch file: `~/learnRobotic/ros2_ws/src/py_launch_tutorial/launch/example_event_handlers_launch.py`

#### Build and Run
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws
    # Build
    colcon build --packages-select py_launch_tutorial   
    # Init package environment
    source install/setup.bash
    # Launch launch file
    ros2 launch py_launch_tutorial example_event_handlers_launch.py turtlesim_ns:='turtlesim3' use_provided_red:='True' new_background_r:=200
```

### Managing large projects (2h)
- Large project's launch files should be well structured to reuse it as much as possible in different situations
- We may have several nodes with several parameters for each node

#### Top-level organization
- Make launch files as reausable as possible
- Cluster related nodes and configurations in different launch files
- Add top-level launch file dedicated to specific use cases
- Top-level launch files should be short, consist of includes to other files corresponding to subcomponents of the application, and commonly changed parameters
- Create top-level launch file that inside launches other launch files to cover each cluster of workflows:
    - Python:   `~/learnRobotic/ros2_ws/src/py_launch_tutorial/launch/launch_turtlesim_launch.py`
    - XML:      `~/learnRobotic/ros2_ws/src/py_launch_tutorial/launch/launch_turtlesim_launch.xml`
    - YAML:     `~/learnRobotic/ros2_ws/src/py_launch_tutorial/launch/launch_turtlesim_launch.yaml`
 
#### Parameters
- Create launch file `turtlesim_world_1_launch.*` which starts `turtlesim_node` with params
    - Python:   `~/learnRobotic/ros2_ws/src/py_launch_tutorial/launch/turtlesim_world_1_launch.py`
    - XML:      `~/learnRobotic/ros2_ws/src/py_launch_tutorial/launch/turtlesim_world_1_launch.xml`
    - YAML:     `~/learnRobotic/ros2_ws/src/py_launch_tutorial/launch/turtlesim_world_1_launch.yaml`

- Create second launch file, `turtlesim_world_2_launch.*`, which starts other turtlesim with parameter values from a YAML configuration file
    - Python:   `~/learnRobotic/ros2_ws/src/py_launch_tutorial/launch/turtlesim_world_2_launch.py`
    - XML:      `~/learnRobotic/ros2_ws/src/py_launch_tutorial/launch/turtlesim_world_2_launch.xml`
    - YAML:     `~/learnRobotic/ros2_ws/src/py_launch_tutorial/launch/turtlesim_world_2_launch.yaml`

- Add the YAML configuration file `turtlesim.yaml` in `/config`: `~/learnRobotic/ros2_ws/src/py_launch_tutorial/config/turtlesim.yaml`

#### Using wildcards in YAML files
- Wildcard characters acts as substitutions for unknown characters in a text value
- It lets to apply same parameters to several different nodes.
- Create launch file `turtlesim_world_3_launch.*` which include one more node `turtlesim_node` in a new namespace `turtlesim3`:
    - Python:   `~/learnRobotic/ros2_ws/src/py_launch_tutorial/launch/turtlesim_world_3_launch.py`
    - XML:      `~/learnRobotic/ros2_ws/src/py_launch_tutorial/launch/turtlesim_world_3_launch.xml`
    - YAML:     `~/learnRobotic/ros2_ws/src/py_launch_tutorial/launch/turtlesim_world_3_launch.yaml`
- To use an unique config file for any node and namespace `~/learnRobotic/ros2_ws/src/py_launch_tutorial/config/tourtlesim.yaml`, we use wildcard:
    - Original config file
    ```yaml
        /turtlesim2/sim:
            ros__parameters:
                background_b: 255
                background_g: 86
                background_r: 150
    ```
    - Updated config file to be usable for all nodes and namespaces:
    ```yaml
        /**:
            ros__parameters:
                background_b: 255
                background_g: 86
                background_r: 150
    ```

#### Namespaces
- `namespaces` allow to start similar nodes without node name or topic name conflicts
- To define large number of namespaces in a launch file we can use `PushROSNamespace`.
- `PushROSNamespace` action:
    - Define global namespace for each launch file; each nested node will inherit global namespaces automatically
    - Hast to be the first action in the list

- Update package `py_launch_tutorial` to use `PushROSNamespace`:
    - Remove `namespace='turtlesim2'` line from launch file:
        - Python:   `~/learnRobotic/ros2_ws/src/py_launch_tutorial/launch/turtlesim_world_2_launch.py`
        - XML:      `~/learnRobotic/ros2_ws/src/py_launch_tutorial/launch/turtlesim_world_2_launch.xml`
        - YAML:     `~/learnRobotic/ros2_ws/src/py_launch_tutorial/launch/turtlesim_world_2_launch.yaml`
    - Create main launch files using namespaces with `PushROSNamespace`
        - Python:   `~/learnRobotic/ros2_ws/src/py_launch_tutorial/launch/launch_turtlesim_PushROSNamespace_launch.py`
        - XML:      `~/learnRobotic/ros2_ws/src/py_launch_tutorial/launch/launch_turtlesim_PushROSNamespace_launch.xml`
        - YAML:     `~/learnRobotic/ros2_ws/src/py_launch_tutorial/launch/launch_turtlesim_PushROSNamespace_launch.yaml`

#### Reusing nodes
...
#### Parameter overrides
...
#### Remapping
...
#### Config files
..
#### Environment variables
...
#### Running launch files
- Update `setup.py` to include launch files, configuration and share folders
    ```py
        import os
        from glob import glob
        from setuptools import setup
        ...

        data_files=[
            ...
            (os.path.join('share', package_name, 'launch'),
                glob('launch/*')),
            (os.path.join('share', package_name, 'config'),
                glob('config/*.yaml')),
            (os.path.join('share', package_name, 'rviz'),
                glob('config/*.rviz')),
        ],
    ```

- Build and Run
    - Build package
    - Run using your choosen launch file format:
        - XML:      `ros2 launch py_launch_tutorial launch_turtlesim_PushROSNamespace_launch.xml`
        - YAML:     `ros2 launch py_launch_tutorial launch_turtlesim_PushROSNamespace_launch.yaml`
        - PYTHON:   `ros2 launch py_launch_tutorial launch_turtlesim_PushROSNamespace_launch.py`

## tf2 (WiP)
### Introducing tf2 (30')
- This demo use `tf2` library to create 3 coordinate frames: `world`, `turtle1` and `turtle2`
- Demo workflow:
    - `tf2 broadcaster`: Publish one turtle coordinate frames
    - `tf2 listener`: Susbcribe to turtle coordiante frame publish, compute difference in the turtle frames
    - Move one turtle to follow the other

- Install need tools
```bash
    sudo apt-get update && sudo apt-get install ros-jazzy-rviz2 ros-jazzy-turtle-tf2-py ros-jazzy-tf2-ros ros-jazzy-tf2-tools ros-jazzy-turtlesim
```
- Run:
    - Run demo `turtle_tf2_py`
```bash
        # Init environment to use ROS2
        cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws
        # Run demo turtle_tf2_py
        ros2 launch turtle_tf2_py turtle_tf2_demo.launch.py
```

    - Run `turtle_teleop_key`. New Terminal:
```bash
        # Init environment to use ROS2
        cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws
        # Run turtle_teleop_key
        ros2 run turtlesim turtle_teleop_key
```

### Using tf tools
- `view_frames`
    - Creates a diagram of the frames being broadcast by tf2 over ROS. ***Only works on Linux***
    - `tf2 listener` listen frames that are broadcasting over ROS and drawing a tree of how the frames are connected
    - Draw tree of broadcasting for running demo: `ros2 run tf2_tools view_frames`
    - It generates diagram in root of workspace: `frames<date_of_creation>.pdf`

- `tf2_echo`
    - Reports transform between any two frames broadcast over ROS
    - Command format: `ros2 run tf2_ros tf2_echo [source_frame] [target_frame]`
    - Use case: Transform of the `turtle2` frame with respect to `turtle1` frame: 
        - Command: `ros2 run tf2_ros tf2_echo turtle2 turtle1`
        - Result: echo valuest that listener receives the frames broadcast over ROS 2

- `riv2` and `tf2`
    - `rviz2` is a visualization tool useful for examining tf2 frames poses
    - Use case: Look our turtle frames using rviz2 using a configuration file (`-d` argument)
        `ros2 run rviz2 rviz2 -d $(ros2 pkg prefix --share turtle_tf2_py)/rviz/turtle_rviz.rviz`

### Writing a static broadcaster - Python (1h)
- Publishing static transforms is usefulto define relationship between a robot base and its sensors or non-moving parts
- In current tutorial:
    1. Publish static transforms to tf2
    2. Use commandline `static_transform_publisher` tool in `tf2_ros`

#### Create package `learning_tf2_py`
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws/src
    # Create package. Python case
    ros2 pkg create --build-type ament_python --license Apache-2.0 -- learning_tf2_py
```

#### Download source of broadcaster node
```bash
    cd learning_tf2_py/learning_tf2_py && wget https://raw.githubusercontent.com/ros/geometry_tutorials/jazzy/turtle_tf2_py/turtle_tf2_py/static_turtle_tf2_broadcaster.py
```

#### Configure package
- Configure `package.xml` with general description and dependencies
```
    <version>0.0.1</vesrion>
    <description>Learning tf2 with rclpy</description>
    ...
    <exec_depend>geometry_msgs</exec_depend>
    <exec_depend>python3-numpy</exec_depend>
    <exec_depend>rclpy</exec_depend>
    <exec_depend>tf2_ros_py</exec_depend>
    <exec_depend>turtlesim</exec_depend>
```

- Configure `setup.py`. Add following line between the `'console_scripts':` brackets: 
    `'static_turtle_tf2_broadcaster = learning_tf2_py.static_turtle_tf2_broadcaster:main',`



#### Build and Run
- Build
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws
    # Check dependencies
    rosdep install -i --from-path src --rosdistro jazzy -y
    # Build
    colcon build --packages-select learning_tf2_py

    # Init package environment
    source install/setup.bash
    # Launch launch file
```
- Run. New terminal
```bash
    # Init environment
    cd ~/learnRobotic/ros2_ws && source install/setup.bash
    # Run executable
    ros2 run learning_tf2_py static_turtle_tf2_broadcaster mystaticturtle 0 0 1 0 0 0
```
- Check that static transform has been published. New terminal
```
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws
    # Check that transform published
    ros2 topic echo /tf_static
```

#### Publish static transforms with `static_transform_publisher`
- `tf2_ros` provides an executable,  `static_transform_publisher` that can be used as a commandline tool or as a node that you can add to your launchfiles. Use cases:
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws
    # Publish static transform to tf2
    ros2 run tf2_ros static_transform_publisher --x 0 --y 0 --z 1 --yaw 0 --pitch 0 --roll 0 --frame-id world --child-frame-id mystaticturtle
    # Publish static transform to tf2, rotation as quaternions
    ros2 run tf2_ros static_transform_publisher --x 0 --y 0 --z 1 --qx 0 --qy 0 --qz 0 --qw 1 --frame-id world --child-frame-id mystaticturtle
```

- `static_transform_publisher` can be used as well within a launc file:
    - Python:   `~/learnRobotic/ros2_ws/src/learning_tf2_py/launch/static_transform_publisher_launch.py`
    - XML:      `~/learnRobotic/ros2_ws/src/learning_tf2_py/launch/static_transform_publisher_launch.xml`
    - YAML:     `~/learnRobotic/ros2_ws/src/learning_tf2_py/launch/static_transform_publisher_launch.yaml`




- Note: ***Note that all arguments except for --frame-id and --child-frame-id are optional; if a particular option isn’t specified, then the identity will be assumed.***

