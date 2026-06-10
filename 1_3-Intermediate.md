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

#### Using tf tools
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
- Publishing static transforms is useful to define relationship between a robot base and its sensors or non-moving parts
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
    # Publish static transform to tf2. 1m offset in Z, not rotatio
    ros2 run tf2_ros static_transform_publisher --x 0 --y 0 --z 1 --yaw 0 --pitch 0 --roll 0 --frame-id world --child-frame-id mystaticturtle
    # Publish static transform to tf2, using quaternions. . 1m offset in Z, not rotation
    ros2 run tf2_ros static_transform_publisher --x 0 --y 0 --z 1 --qx 0 --qy 0 --qz 0 --qw 1 --frame-id world --child-frame-id mystaticturtle
```

- `static_transform_publisher` can be used as well within a launc file:
    - Python:   `~/learnRobotic/ros2_ws/src/learning_tf2_py/launch/static_transform_publisher_launch.py`
    - XML:      `~/learnRobotic/ros2_ws/src/learning_tf2_py/launch/static_transform_publisher_launch.xml`
    - YAML:     `~/learnRobotic/ros2_ws/src/learning_tf2_py/launch/static_transform_publisher_launch.yaml`

- Note: ***Note that all arguments except for --frame-id and --child-frame-id are optional; if a particular option isn’t specified, then the identity will be assumed.***

### Writing a static broadcaster - C++ (45')
- IDEM than previous but with C++ in spite of Python

#### Create package `learning_tf2_cpp`
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws/src
    # Create package with dependencies
    ros2 pkg create --build-type ament_cmake --license Apache-2.0 --dependencies geometry_msgs rclcpp tf2 tf2_ros turtlesim -- learning_tf2_cpp
```

#### Download source of broadcaster node
```bash
    cd ~/learnRobotic/ros2_ws/src/learning_tf2_cpp/src
    wget https://raw.githubusercontent.com/ros/geometry_tutorials/jazzy/turtle_tf2_cpp/src/static_turtle_tf2_broadcaster.cpp
```

#### Configure package
- `package.xml`
    - Dependencies shoulbe already there, 
    - Update version and description
      ```xml
        <version>0.0.1</version>
        <description>Learning tf2 with rclcpp</description>
      ```

- `CMkaleLists.txt`
    - Add executable, dependencies of build and install Target. 
    ```txt
        add_executable(static_turtle_tf2_broadcaster src/static_turtle_tf2_broadcaster.cpp)
        ament_target_dependencies(
            static_turtle_tf2_broadcaster
            geometry_msgs
            rclcpp
            tf2
            tf2_ros
        )

        install(TARGETS
            static_turtle_tf2_broadcaster
            DESTINATION lib/${PROJECT_NAME})
    ```

#### Build and Run
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws
    # Check dependencies
    rosdep install -i --from-path src --rosdistro jazzy -y
    # Build
    colcon build --packages-select learning_tf2_cpp
```

- Run. New terminal
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash
    # Run executable
    ros2 run learning_tf2_cpp static_turtle_tf2_broadcaster mystaticturtle 0 0 1 0 0 0
```

- Check that static tf has been broadcasted. New terminal
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws
    # show tf_static topic
    ros2 topic echo /tf_static
```

#### Publish static transforms with `static_transform_publisher`
- In a normal development process, to broadcast static transform, we may use executable `static_transform_publisher` of `tf2_ros`

- From command line:
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws
    # Publish static transform to tf2. 1m offset in Z, not rotatio
    ros2 run tf2_ros static_transform_publisher --x 0 --y 0 --z 1 --yaw 0 --pitch 0 --roll 0 --frame-id world --child-frame-id mystaticturtle
    # Publish static transform to tf2, rotation as quaternions
    ros2 run tf2_ros static_transform_publisher --x 0 --y 0 --z 1 --qx 0 --qy 0 --qz 0 --qw 1 --frame-id world --child-frame-id mystaticturtle
```

- From launch file:
    - Python:   `~/learnRobotic/ros2_ws/src/learning_tf2_cpp/launch/static_transform_publisher_launch.py`
    - XML:      `~/learnRobotic/ros2_ws/src/learning_tf2_cpp/launch/static_transform_publisher_launch.xml`
    - YAML:     `~/learnRobotic/ros2_ws/src/learning_tf2_cpp/launch/static_transform_publisher_launch.yaml`

### Writing a broadcaster - Python (45')
- How to broadcast state of a robot to `tf2`
- We work in this section in package previously created `learning_tf2_py`

#### Download source of broadcaster node
```bash
    # Navigate to folder of py sources
    cd ~/learnRobotic/ros2_ws/src/learning_tf2_py/learning_tf2_py
    # Download source of broadcaster
    wget https://raw.githubusercontent.com/ros/geometry_tutorials/jazzy/turtle_tf2_py/turtle_tf2_py/turtle_tf2_broadcaster.py
```

#### Configure package
- `setup.py`
    - Add entry point to allow run our new node. Add entry point line within`console_scripts`. Result
    ```py
        entry_points={
            'console_scripts': [
                'static_turtle_tf2_broadcaster = learning_tf2_py.static_turtle_tf2_broadcaster:main',
                'turtle_tf2_broadcaster = learning_tf2_py.turtle_tf2_broadcaster:main',            
            ],
        },
    ```

    - Add `/launch` folder so that we can use launch files. 
        - Add `(os.path.join('share', package_name, 'launch'), glob('launch/*'))` to `data_files`. Result:
        ```py
                data_files=[
                ('share/ament_index/resource_index/packages', ['resource/' + package_name]),
                ('share/' + package_name, ['package.xml']),
                (os.path.join('share', package_name, 'launch'), glob('launch/*')),        
            ],
        ```
        - Add related imports.
        ```py
            import os
            from glob import glob
        ```

- `package.xml`
    - Add dependencies:
    ```xml
        <exec_depend>launch</exec_depend>
        <exec_depend>launch_ros</exec_depend>
    ```

#### Build and Run
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws
    # Check dependencies
    rosdep install -i --from-path src --rosdistro jazzy -y
    # Build
    colcon build --packages-select learning_tf2_py
```

- Run using launch file. New terminal
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash
    # Run using launch file
    ros2 launch learning_tf2_py turtle_tf2_demo_launch.xml
    ## ros2 launch learning_tf2_py turtle_tf2_demo_launch.py
    ## ros2 launch learning_tf2_py turtle_tf2_demo_launch.yaml
```

- Run `turle_teleop__key`. New terminal
```bash
    cd ~/learnRobotic/ && source ros2_env_conf.sh
    ros2 run turtlesim turtle_teleop_key
```

- Run `tf2_echo` to check if `turtle1` pose is getting broadcasted to `tf2`. New terminal
```bash
    cd ~/learnRobotic/ && source ros2_env_conf.sh
    ros2 run tf2_ros tf2_echo world turtle1
```

### Writing a broadcaster - C++ (45')
- How to broadcast state of a robot to `tf2`
- We work in this section in package previously created `learning_tf2_cpp`

#### Download source of broadcaster node
```bash
    # Navigate to folder of py sources
    cd ~/learnRobotic/ros2_ws/src/learning_tf2_cpp/src
    # Download source of broadcaster
    wget https://raw.githubusercontent.com/ros/geometry_tutorials/jazzy/turtle_tf2_cpp/src/turtle_tf2_broadcaster.cpp
```

#### Configure package
- `CMakeLists.txt`
    - Add executable, dependencies and Target so that `run2 run` can find it
    ```txt
        add_executable(turtle_tf2_broadcaster src/turtle_tf2_broadcaster.cpp)
        ament_target_dependencies(
            turtle_tf2_broadcaster
            geometry_msgs
            rclcpp
            tf2
            tf2_ros
            turtlesim
        )

        install(TARGETS turtle_tf2_broadcaster DESTINATION lib/${PROJECT_NAME})
    ```
    - Add launch files inside `launch/` folder will be installed
    ```txt
        install(DIRECTORY launch DESTINATION share/${PROJECT_NAME})
    ```

- `package.xml`
    - Add execution dependencies for launch folders
    ```xml
        <exec_depend>launch</exec_depend>
        <exec_depend>launch_ros</exec_depend>
    ```

#### Build and Run
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws
    # Check dependencies
    rosdep install -i --from-path src --rosdistro jazzy -y
    # Build
    colcon build --packages-select learning_tf2_cpp
```

- Run using launch file. New terminal
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash
    # Run using launch file
    ros2 launch learning_tf2_cpp turtle_tf2_demo_launch.xml
    ## ros2 launch learning_tf2_py turtle_tf2_demo_launch.py
    ## ros2 launch learning_tf2_py turtle_tf2_demo_launch.yaml
```

- Run `turle_teleop__key`. New terminal
```bash
    cd ~/learnRobotic/ && source ros2_env_conf.sh
    ros2 run turtlesim turtle_teleop_key
```

- Run `tf2_echo` to check if `turtle1` pose is getting broadcasted to `tf2`. New terminal
```bash
    cd ~/learnRobotic/ && source ros2_env_conf.sh
    ros2 run tf2_ros tf2_echo world turtle1
```

### Writing a listener - Python (30')
- Create tf2 listener, python
- We use already created package `learning_tf2_py`

#### Download source of listener node into package `learning_tf2_py`
```bash
    # Navigate to folder of py sources
    cd ~/learnRobotic/ros2_ws/src/learning_tf2_py/learning_tf2_py
    # Download source
    wget https://raw.githubusercontent.com/ros/geometry_tutorials/jazzy/turtle_tf2_py/turtle_tf2_py/turtle_tf2_listener.py
```

#### Reconfigure package `learning_tf2_py`
- `setup.py`
    - Let `ros2 run` to run our new node by adding an entry point for it
    - Add the following line between the `console_scripts:` brackets:
    ```py
        'turtle_tf2_listener = learning_tf2_py.turtle_tf2_listener:main',
    ```

#### Update launch file
- Declare a `target_frame` launch argument
- Start a broadcaster node for second turtle that we will spawn
- Start a listener node that will subscribe to those transformations
- Update:
    - Python:   `~/learnRobotic/ros2_ws/src/learning_tf2_py/launch/turtle_tf2_demo_launch.py`
    - XML:      `~/learnRobotic/ros2_ws/src/learning_tf2_py/launch/turtle_tf2_demo_launch.xml`
    - YAML:     `~/learnRobotic/ros2_ws/src/learning_tf2_py/launch/turtle_tf2_demo_launch.yaml`

#### Build and Run
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws
    # Check dependencies
    rosdep install -i --from-path src --rosdistro jazzy -y
    # Build
    colcon build --packages-select learning_tf2_py
```

- Run using launch file. New terminal
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash
    # Run using launch file
    ros2 launch learning_tf2_py turtle_tf2_demo_launch.py
    ## ros2 launch learning_tf2_py turtle_tf2_demo_launch.xml
    ## ros2 launch learning_tf2_py turtle_tf2_demo_launch.yaml
```

- Move `turtle1` with teleop and `turtle2` should follow `turtle1`
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws
    # Run teleop
    ros2 run turtlesim turtle_teleop_key
```

### Writing a listener - C++ (20')
- Create tf2 listener, C++
- We use already created package `learning_tf2_cpp`

#### Download source of listener node into package `learning_tf2_cpp`
```bash
    # Navigate to folder of py sources
    cd ~/learnRobotic/ros2_ws/src/learning_tf2_cpp/src
    # Download source
    wget https://raw.githubusercontent.com/ros/geometry_tutorials/jazzy/turtle_tf2_cpp/src/turtle_tf2_listener.cpp
```

#### Reconfigure package `learning_tf2_cpp`
- `CMakelists.txt`
    - Add executable-source and dependencies to let `ros2 run` to locate it
```txt
    add_executable(turtle_tf2_listener src/turtle_tf2_listener.cpp)
    ament_target_dependencies(
        turtle_tf2_listener
        geometry_msgs
        rclcpp
        tf2
        tf2_ros
        turtlesim
    )
```
    - Install your executable inside the build package
```txt
    install(TARGETS
    turtle_tf2_listener
    DESTINATION lib/${PROJECT_NAME})
```

#### Update launch file `turtle_tf2_demo_launch.*`
- Declare a `target_frame` launch argument
- Start a broadcaster node for second turtle that we will spawn
- Start a listener node that will subscribe to those transformations
- Update:
    - Python:   `~/learnRobotic/ros2_ws/src/learning_tf2_cpp/launch/turtle_tf2_demo_launch.py`
    - XML:      `~/learnRobotic/ros2_ws/src/learning_tf2_cpp/launch/turtle_tf2_demo_launch.xml`
    - YAML:     `~/learnRobotic/ros2_ws/src/learning_tf2_cpp/launch/turtle_tf2_demo_launch.yaml`

#### Build and Run
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws
    # Check dependencies
    rosdep install -i --from-path src --rosdistro jazzy -y
    # Build
    colcon build --packages-select learning_tf2_cpp
```

- Run using launch file. New terminal
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash
    # Run using launch file
    ros2 launch learning_tf2_cpp turtle_tf2_demo_launch.py
    ## ros2 launch learning_tf2_cpp turtle_tf2_demo_launch.xml
    ## ros2 launch learning_tf2_cpp turtle_tf2_demo_launch.yaml
```

- Move `turtle1` with teleop and `turtle2` should follow `turtle1`
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws
    # Run teleop
    ros2 run turtlesim turtle_teleop_key
```

### Adding a frame - Python (45')
- How to add fixed and dynamic frames to `tf2` transformation tree
- `tf2` allows to define local frame for each sensor, link or joint of the system
- When transforming from a frame to another,`tf2` takes care of hidden intermediate frame transforamtions
- `tf2 tree`
    - `tf2` build up a tree structure of frames, not closed loop are allowed
    - Each frame
        - Only a parent
        - Multiple children allowed
    - Current tree structure:
        -world
            |-turtle1
            |-turtle2
- If we add a new frame -> It will become child frame of one of the current frames

#### Download source of new frame, `carrot1`, into package `learning_tf2_py`
- New frame `carrot1`:
    - Child of `turtle1`
    - Goal for second turtle
```bash
    # Navigate to folder of py sources
    cd ~/learnRobotic/ros2_ws/src/learning_tf2_py/learning_tf2_py
    # Download source
    wget https://raw.githubusercontent.com/ros/geometry_tutorials/jazzy/turtle_tf2_py/turtle_tf2_py/fixed_frame_tf2_broadcaster.py
```

#### Reconfigure package `learning_tf2_py`
- `setup.py`
    - Add entry point of new executable by adding following line between the `console_scripts:` brackets:
    ```py
        'fixed_frame_tf2_broadcaster = learning_tf2_py.fixed_frame_tf2_broadcaster:main',
    ```

#### Create new launch file `turtle_tf2_fixed_frame_demo_launch.*`
- Import required packages and creates `demo_nodes` variable that will store nodes that we created in previous tutotial's file
    - Python:   `~/learnRobotic/ros2_ws/src/learning_tf2_py/launch/turtle_tf2_fixed_frame_demo_launch.py`
    - XML:      `~/learnRobotic/ros2_ws/src/learning_tf2_py/launch/turtle_tf2_fixed_frame_demo_launch.xml`
    - YAML:     `~/learnRobotic/ros2_ws/src/learning_tf2_py/launch/turtle_tf2_fixed_frame_demo_launch.yaml`

#### Build and Run
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws
    # Check dependencies
    rosdep install -i --from-path src --rosdistro jazzy -y
    # Build
    colcon build --packages-select learning_tf2_py
```

- Run using launch file. New terminal
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash
    # Run using launch file
    ros2 launch learning_tf2_py turtle_tf2_fixed_frame_demo_launch.xml
    ## ros2 launch learning_tf2_py turtle_tf2_fixed_frame_demo_launch.yaml
    ## ros2 launch learning_tf2_py turtle_tf2_fixed_frame_demo_launch.py
```

- Move `turtle1` with teleop and `turtle2` should follow `turtle1`. New Terminal
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws
    # Run teleop
    ros2 run turtlesim turtle_teleop_key
```

- Check how our `turtle2` follow carrot instead of `turtle1`. New Terminal
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash
    # Run teleop
    ros2 launch learning_tf2_py turtle_tf2_fixed_frame_demo_launch.xml target_frame:=carrot1
    ## ros2 launch learning_tf2_py turtle_tf2_fixed_frame_demo_launch.yaml target_frame:=carrot1
    ## ros2 launch learning_tf2_py turtle_tf2_fixed_frame_demo_launch.py target_frame:=carrot1    
```

- While running this examples, generates frame trees to check new frames s  tructure. New Terminal
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws
    # Run teleop
    ros2 run tf2_tools view_frames
```

#### Write a dynamic frame broadcaster
- New frame is a fixed frame, does not change over time in relation to parent frame
- It is possible to publish dynamic frames that will change behaviour over time

##### Dynamic. Download dynamic broadcaster source
```bash
    # Navigate to folder of py sources
    cd ~/learnRobotic/ros2_ws/src/learning_tf2_py/learning_tf2_py
    # Download source
    wget https://raw.githubusercontent.com/ros/geometry_tutorials/jazzy/turtle_tf2_py/turtle_tf2_py/dynamic_frame_tf2_broadcaster.py
```

##### Reconfigure package `learning_tf2_py`
- `setup.py`
    - Add entry point of new executable by adding following line between the `console_scripts:` brackets:
    ```py
        'dynamic_frame_tf2_broadcaster = learning_tf2_py.dynamic_frame_tf2_broadcaster:main',
    ```

##### Create new launch file `turtle_tf2_dynamic_frame_demo_launch.*`
- Import required packages and creates `demo_nodes` variable that will store nodes that we created in previous tutotial's file
    - Python:   `~/learnRobotic/ros2_ws/src/learning_tf2_py/launch/turtle_tf2_dynamic_frame_demo_launch.py`
    - XML:      `~/learnRobotic/ros2_ws/src/learning_tf2_py/launch/turtle_tf2_dynamic_frame_demo_launch.xml`
    - YAML:     `~/learnRobotic/ros2_ws/src/learning_tf2_py/launch/turtle_tf2_dynamic_frame_demo_launch.yaml`

#### Build and Run
- Build package
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws
    # Check dependencies
    rosdep install -i --from-path src --rosdistro jazzy -y
    # Build
    colcon build --packages-select learning_tf2_py
```

- Run using launch file. New terminal
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash
    # Run using launch file
    ros2 launch learning_tf2_py turtle_tf2_dynamic_frame_demo_launch.py
    ## ros2 launch learning_tf2_py turtle_tf2_dynamic_frame_demo_launch.xml
    ## ros2 launch learning_tf2_py turtle_tf2_dynamic_frame_demo_launch.yaml
```

### Adding a frame - C++ (20')
- IDEM than for Python

#### Download source of new frame, `carrot1`, into package `learning_tf2_cpp`
- New frame `carrot1`:
    - Child of `turtle1`
    - Goal for second turtle
```bash
    # Navigate to folder of py sources
    cd ~/learnRobotic/ros2_ws/src/learning_tf2_cpp/src
    # Download source
    wget https://raw.githubusercontent.com/ros/geometry_tutorials/jazzy/turtle_tf2_cpp/src/fixed_frame_tf2_broadcaster.cpp

```

#### Reconfigure package `learning_tf2_py`
- `CMakelists.txt`
    - Add executable and depependencies:
    ```txt
        add_executable(fixed_frame_tf2_broadcaster src/fixed_frame_tf2_broadcaster.cpp)
        ament_target_dependencies(
            fixed_frame_tf2_broadcaster
            geometry_msgs
            rclcpp
            tf2_ros
        )
    ```
    - Install executable into build
    ```txt
        install(TARGETS
            fixed_frame_tf2_broadcaster
            DESTINATION lib/${PROJECT_NAME})
    ```

#### Create new launch file `turtle_tf2_fixed_frame_demo_launch.*`
- Import required packages and creates `demo_nodes` variable that will store nodes that we created in previous tutotial's file
    - Python:   `~/learnRobotic/ros2_ws/src/learning_tf2_py/launch/turtle_tf2_fixed_frame_demo_launch.py`
    - XML:      `~/learnRobotic/ros2_ws/src/learning_tf2_py/launch/turtle_tf2_fixed_frame_demo_launch.xml`
    - YAML:     `~/learnRobotic/ros2_ws/src/learning_tf2_py/launch/turtle_tf2_fixed_frame_demo_launch.yaml`

#### Build and Run
- Build package
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws
    # Check dependencies
    rosdep install -i --from-path src --rosdistro jazzy -y
    # Build
    colcon build --packages-select learning_tf2_cpp
```

- Run using launch file. New terminal
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash
    # Run using launch file
    ros2 launch learning_tf2_cpp turtle_tf2_fixed_frame_demo_launch.py
    ## ros2 launch learning_tf2_cpp turtle_tf2_fixed_frame_demo_launch.xml
    ## ros2 launch learning_tf2_cpp turtle_tf2_fixed_frame_demo_launch.yaml
```

- Move `turtle1` with teleop and `turtle2` should follow `turtle1`. New Terminal
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws
    # Run teleop
    ros2 run turtlesim turtle_teleop_key
```

- Check how our `turtle2` follow carrot instead of `turtle1`. New Terminal
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash
    # Run teleop
    ros2 launch learning_tf2_py turtle_tf2_fixed_frame_demo_launch.xml target_frame:=carrot1
    ## ros2 launch learning_tf2_py turtle_tf2_fixed_frame_demo_launch.yaml target_frame:=carrot1
    ## ros2 launch learning_tf2_py turtle_tf2_fixed_frame_demo_launch.py target_frame:=carrot1    
```

- While running this examples, generates frame trees to check new frames s  tructure. New Terminal
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws
    # Run teleop
    ros2 run tf2_tools view_frames
```

##### Dynamic. Download dynamic broadcaster source
```bash
    # Navigate to folder of py sources
    cd ~/learnRobotic/ros2_ws/src/learning_tf2_cpp/src
    # Download source
    wget https://raw.githubusercontent.com/ros/geometry_tutorials/jazzy/turtle_tf2_cpp/src/dynamic_frame_tf2_broadcaster.cpp
```

##### Reconfigure package `learning_tf2_cpp`
- `CMakelists.txt`
    - Add executable and depependencies:
    ```txt
        add_executable(dynamic_frame_tf2_broadcaster src/dynamic_frame_tf2_broadcaster.cpp)
        ament_target_dependencies(
            dynamic_frame_tf2_broadcaster
            geometry_msgs
            rclcpp
            tf2_ros
        )
    ```
    - Install executable into build
    ```txt
        install(TARGETS
            dynamic_frame_tf2_broadcaster
            DESTINATION lib/${PROJECT_NAME})
    ```

##### Create new launch file `turtle_tf2_dynamic_frame_demo_launch.*`
- Import required packages and creates `demo_nodes` variable that will store nodes that we created in previous tutotial's file
    - Python:   `~/learnRobotic/ros2_ws/src/learning_tf2_cpp/launch/turtle_tf2_dynamic_frame_demo_launch.py`
    - XML:      `~/learnRobotic/ros2_ws/src/learning_tf2_cpp/launch/turtle_tf2_dynamic_frame_demo_launch.xml`
    - YAML:     `~/learnRobotic/ros2_ws/src/learning_tf2_cpp/launch/turtle_tf2_dynamic_frame_demo_launch.yaml`

#### Build and Run
- Build package
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws
    # Check dependencies
    rosdep install -i --from-path src --rosdistro jazzy -y
    # Build
    colcon build --packages-select learning_tf2_cpp
```

- Run using launch file. New terminal
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash
    # Run using launch file
    ros2 launch learning_tf2_cpp turtle_tf2_dynamic_frame_demo_launch.xml
    ## ros2 launch learning_tf2_cpp turtle_tf2_dynamic_frame_demo_launch.yaml
    ## ros2 launch learning_tf2_cpp turtle_tf2_dynamic_frame_demo_launch.py
```


### Using time - C++ (20')
- To get a transform at a specific time and wait for a transform to be available on the tf2 tree using `lookupTransform()` function

#### Update listener node
- In `learning_tf2_cpp` update `turtle_tf2_listener.cpp` to request a frame transform at time “now”
```cpp
    /*
    try {
        t = tf_buffer_->lookupTransform(
        toFrameRel, fromFrameRel,
        tf2::TimePointZero);
    } catch (const tf2::TransformException & ex) { */
    rclcpp::Time now = this->get_clock()->now();
    try {
        t = tf_buffer_->lookupTransform(
            toFrameRel, fromFrameRel,
            now);
    } catch (const tf2::TransformException & ex) {
```

- Build package
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws
    # Check dependencies
    rosdep install -i --from-path src --rosdistro jazzy -y
    # Build
    colcon build --packages-select learning_tf2_cpp
```

- Run using launch file. New terminal
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash
    # Run
    ros2 launch learning_tf2_cpp turtle_tf2_demo_launch.xml
    # ros2 launch learning_tf2_cpp turtle_tf2_demo_launch.xaml
    # ros2 launch learning_tf2_cpp turtle_tf2_demo_launch.py     
```

- The output tells you that the frame does not exist or that the data is in the future.
- Error is because you should wait a few milliseconds for that information to arrive.

#### Fix listener node
- `tf2` provides a tool that will wait until a transform becomes available, adding `timeout` param to `lookupTransform()`
- Using this `timeout` `lookupTransform()` will actually block until the transform between the two turtles becomes available. If `timeout` expires and transform is not available, an exception will be raised
```cpp
    /*
    try {
        t = tf_buffer_->lookupTransform(
        toFrameRel, fromFrameRel,
        tf2::TimePointZero);
    } catch (const tf2::TransformException & ex) { */
    rclcpp::Time now = this->get_clock()->now();
    try {
        t = tf_buffer_->lookupTransform(
            toFrameRel,
            fromFrameRel,
            now,
            50ms);
    } catch (const tf2::TransformException & ex) {
```

- Build package
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws
    # Check dependencies
    rosdep install -i --from-path src --rosdistro jazzy -y
    # Build
    colcon build --packages-select learning_tf2_cpp    
```

- Run using launch file. New terminal
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash
    # Run
    ros2 launch learning_tf2_cpp turtle_tf2_demo_launch.xml
    # ros2 launch learning_tf2_cpp turtle_tf2_demo_launch.xaml
    # ros2 launch learning_tf2_cpp turtle_tf2_demo_launch.py
```

### Traveling in time - C++ (30')
- `tf2` library is is able to transform data in time as well as in space
- `tf2` time travel feature may be useful tasks as:
    - Monitoring pose of robot for a long period of time
    - Build a follower robot that follow `steps` of leader with delay
- Current example: To look up transforms back in time and program `turtle2` to follow 5 seconds behind `carrot1`


#### Time travel - example
- On `learning_tf2_cpp`, modify node `turtle_tf2_listener.cpp`: Second turtle instead of go to where carrot is now, it will go to where first carrot was 5 seconds ago
```cpp
    /*
    try {
        t = tf_buffer_->lookupTransform(
        toFrameRel, fromFrameRel,
        tf2::TimePointZero);
    } catch (const tf2::TransformException & ex) { */
    rclcpp::Time when = this->get_clock()->now() - rclcpp::Duration(5, 0);
    try {
        t = tf_buffer_->lookupTransform(
            toFrameRel,
            fromFrameRel,
            when,
            50ms);
    } catch (const tf2::TransformException & ex) {
```

- Build package
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws
    # Check dependencies
    rosdep install -i --from-path src --rosdistro jazzy -y
    # Build
    colcon build --packages-select learning_tf2_cpp
```

- Run using launch file. New terminal
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash
    # Run
    ros2 launch learning_tf2_cpp turtle_tf2_demo_launch.xml
    # ros2 launch learning_tf2_cpp turtle_tf2_demo_launch.xaml
    # ros2 launch learning_tf2_cpp turtle_tf2_demo_launch.py
```

- This approach has a bug:
    - `turtle2` try to go where `turtle1` was 5 seconds ago in function to `turtle2` pose 5 seconds ago.
    - But `turtle2` is moving itself, so it generates wrong new trayectories that will not reach `turtle1` pose

#### Fix listener node - Use 6 parameters for `lookupTransform()`
 - What we really want in our example is: “What was the pose of carrot1 5 seconds ago, relative to the current position of the turtle2?”
 - We can say explicitly when to acquire the specified transformations by calling `lookupTransform()` with additional parameters
 ```cpp
    /*
    try {
        t = tf_buffer_->lookupTransform(
        toFrameRel, fromFrameRel,
        tf2::TimePointZero);
    } catch (const tf2::TransformException & ex) { */
    rclcpp::Time now = this->get_clock()->now();
    rclcpp::Time when = now - rclcpp::Duration(5, 0);
    try {
        t = tf_buffer_->lookupTransform(
            toFrameRel,
            now,
            fromFrameRel,
            when,
            "world",
            50ms);
    } catch (const tf2::TransformException & ex) {
```

- 6 arguments:
1. Target frame
2. The time to transform to
3. Source frame
4. The time at which source frame will be evaluated
5. Frame that does not change over time, in this case the world frame
6. Time to wait for the target frame to become available

- Build package
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws
    # Check dependencies
    rosdep install -i --from-path src --rosdistro jazzy -y
    # Build
    colcon build --packages-select learning_tf2_cpp
```

- Run using launch file. New terminal
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash
    # Run
    ros2 launch learning_tf2_cpp turtle_tf2_demo_launch.xml
    # ros2 launch learning_tf2_cpp turtle_tf2_demo_launch.xaml
    # ros2 launch learning_tf2_cpp turtle_tf2_demo_launch.py
```

### Debugging (30')
- Some debugging tools are `tf2_echo`, `tf2_monitor`, `view_frames`

#### Setting and starting example
- Into `learning_tf2_cpp/src`. Copy `turtle_tf2_listener.cpp` and rename as `turtle_tf2_listener_debug.cpp`

- Apply some changes to source:
    - Change line 65 to:
    ```cpp
    // std::string toFrameRel = "turtle2";
    std::string toFrameRel = "turtle3";
    ```

    - Change lines 73-77 to:
    ```cpp
    // try {
    //  t = tf_buffer_->lookupTransform(
    //  toFrameRel, fromFrameRel,
    //  tf2::TimePointZero);
    // } catch (const tf2::TransformException & ex) {
    try {
        t = tf_buffer_->lookupTransform(
            toFrameRel, fromFrameRel,
            this->now());
    } catch (const tf2::TransformException & ex) {
    ```

- Create new launch files in `launch` folder:
    - `start_tf2_debug_demo_launch.py`
    ```python
    from launch import LaunchDescription
    from launch.actions import DeclareLaunchArgument
    from launch.substitutions import LaunchConfiguration
    from launch_ros.actions import Node


    def generate_launch_description():
        return LaunchDescription([
            DeclareLaunchArgument(
                'target_frame', default_value='turtle1',
                description='Target frame name.'
            ),
            Node(
                package='turtlesim',
                executable='turtlesim_node',
                name='sim',
                output='screen'
            ),
            Node(
                package='learning_tf2_cpp',
                executable='turtle_tf2_broadcaster',
                name='broadcaster1',
                parameters=[
                    {'turtlename': 'turtle1'}
                ]
            ),
            Node(
                package='learning_tf2_cpp',
                executable='turtle_tf2_broadcaster',
                name='broadcaster2',
                parameters=[
                    {'turtlename': 'turtle2'}
                ]
            ),
            Node(
                package='learning_tf2_cpp',
                executable='turtle_tf2_listener_debug',
                name='listener_debug',
                parameters=[
                    {'target_frame': LaunchConfiguration('target_frame')}
                ]
            ),
        ])
    ```

    - `start_tf2_debug_demo_launch.xml`
    ```xml
    <?xml version="1.0" encoding="UTF-8"?>
    <launch>
        <arg name="target_frame" default="turtle1" description="Target frame name." />
        <node pkg="turtlesim" exec="turtlesim_node" name="sim" output="screen" />
        <node pkg="learning_tf2_cpp" exec="turtle_tf2_broadcaster" name="broadcaster1">
            <param name="turtlename" value="turtle1" />
        </node>
        <node pkg="learning_tf2_cpp" exec="turtle_tf2_broadcaster" name="broadcaster2">
            <param name="turtlename" value="turtle2" />
        </node>
        <node pkg="learning_tf2_cpp" exec="turtle_tf2_listener_debug" name="listener_debug">
            <param name="target_frame" value="$(var target_frame)" />
        </node>
    </launch>
    ```

    - `start_tf2_debug_demo_launch.yaml`
    ```yaml
    %YAML 1.2
    ---
    launch:
    - arg:
        name: "target_frame"
        default: "turtle1"
        description: "Target frame name."
    - node:
        pkg: "turtlesim"
        exec: "turtlesim_node"
        name: "sim"
        output: "screen"
    - node:
        pkg: "learning_tf2_cpp"
        exec: "turtle_tf2_broadcaster"
        name: "broadcaster1"
        param:
        - name: "turtlename"
            value: "turtle1"
    - node:
        pkg: "learning_tf2_cpp"
        exec: "turtle_tf2_broadcaster"
        name: "broadcaster2"
        param:
        - name: "turtlename"
            value: "turtle2"
    - node:
        pkg: "learning_tf2_cpp"
        exec: "turtle_tf2_listener_debug"
        name: "listener_debug"
        param:
        - name: "target_frame"
            value: "$(var target_frame)"
    ```

- Add to `CMakeLists.txt` the executable to build `turtle_tf2_listener_debug`

- Build package & Run
```bash
    ros2 launch learning_tf2_cpp start_tf2_debug_demo_launch.xml
    #ros2 launch learning_tf2_cpp start_tf2_debug_demo_launch.py
    #ros2 launch learning_tf2_cpp start_tf2_debug_demo_launch.yaml
```

- Now we can run the example but `turtle2` is not following `turtle1` as it should

#### Finding bugs
- `tfw_echo`: Find out if `tf2` knows about our transform between `turtle3` and `turtle1`
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash
    # Run
    ros2 run tf2_ros tf2_echo turtle3 turtle1
```
- Result: `turtle3` does not exist

- `view_frames`: Get graphical representation of existing frames
```bash 
ros2 run tf2_tools view_frames
```
- We can graphically see that `turtle3` does not exist
- We solve this issue: Line 65 of source restablishing original value of `turtle2`

- New issue appears with `timestamp` that we are going to debug now.
- `tf2_monitor`: Get stadisitics on timing
```bash
    # Init environment
    cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws && source install/setup.bash
    # tf2_monitor
    ros2 run tf2_ros tf2_monitor turtle2 turtle1
```
- We find out a 3 ms delay for the chain from `turtle2` to `turtle1`.
- Solve it by modifying lines 73-77 to ask for transform between turtles 100 ms ago:
```cpp
try {
  t = tf_buffer_->lookupTransform(
    toFrameRel, fromFrameRel,
    this->now() - rclcpp::Duration::from_seconds(0.1));
} catch (const tf2::TransformException & ex) {
```
- Run and check that now `turtle2` is following to `turtle1`
```bash
ros2 launch learning_tf2_cpp start_tf2_debug_demo_launch.xml
#ros2 launch learning_tf2_cpp start_tf2_debug_demo_launch.py
#ros2 launch learning_tf2_cpp start_tf2_debug_demo_launch.yaml
```

### Quaternion fundamentals (30')
- Quaternion is a 4-tuple representation of orientation, which is more concise than a rotation matrix

#### Components of a quaternion
- ROS 2 uses quaternions to apply rotations
- Quaternion has 4 componente: `(x, y, z, w)`
- **Unit quaternion**: (0, 0, 0, 1). It yields no rotation about x/y/z axes
- **Unit quaternion** can be created by
```cpp
    #include <tf2/LinearMath/Quaternion.h>
    ...

    tf2::Quaternion q;
    // Create a quaternion from roll/pitch/yaw in radians (0, 0, 0)
    q.setRPY(0, 0, 0);
    // Print the quaternion components (0, 0, 0, 1)
    RCLCPP_INFO(this->get_logger(), "%f %f %f %f",
                q.x(), q.y(), q.z(), q.w());
```
- Magnitud of a quaternion should be allways one
- To normalize a quaternion:
```cpp
    q.normalize();
```

#### Quaternion types in ROS 2
- ROS 2 use 2 quaternion datatype:
    - `tf2::Quaternion`
    - `geometry_msgs::msg::Quaternion`
- To convert between in *C++* use library: `tf2_geometry_msgs` 
```cpp
    #include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
    ...

    tf2::Quaternion tf2_quat, tf2_quat_from_msg;
    tf2_quat.setRPY(roll, pitch, yaw);
    // Convert tf2::Quaternion to geometry_msgs::msg::Quaternion
    geometry_msgs::msg::Quaternion msg_quat = tf2::toMsg(tf2_quat);

    // Convert geometry_msgs::msg::Quaternion to tf2::Quaternion
    tf2::convert(msg_quat, tf2_quat_from_msg);
    // or
    tf2::fromMsg(msg_quat, tf2_quat_from_msg);
```

- **Not equivalent in *Python* to `tf2::Quaternion`**. Builtin `list` is used:
```py
    from geometry_msgs.msg import Quaternion
    ...

    # Create a list of floats, which is compatible with tf2
    # Quaternion methods
    quat_tf = [0.0, 1.0, 0.0, 0.0]

    # Convert a list to geometry_msgs.msg.Quaternion
    msg_quat = Quaternion(x=quat_tf[0], y=quat_tf[1], z=quat_tf[2], w=quat_tf[3])
```

#### Quaternion operations
1. **Think on RPY and convert to quaternion**
- Calculate target rotation in terms of 3 individual rotations respect original, unmoving coordinates axes:
    - Roll (about X-axis)
    - Pitch (about Y-axis)
    - Yatch (about Z-axis)
- Then convert to quaternions
```py
# quaternion_from_euler method is available in turtle_tf2_py/turtle_tf2_py/turtle_tf2_broadcaster.py
q = quaternion_from_euler(1.5707, 0, -1.5707)
print(f'The quaternion representation is x: {q[0]} y: {q[1]} z: {q[2]} w: {q[3]}.')
```
- This techquine is called *fixed (or static) frame RPY*. It is related to **Euler angles**

2. **Applying a quaternion rotation**
- Apply rotation of one quaternion to a pose: Multiply previous quaternion of pose by the quaternion representing desired rotation
- *Note: Order of multiplication matters*
- C++:
```cpp
    #include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
    ...

    tf2::Quaternion q_orig, q_rot, q_new;

    q_orig.setRPY(0.0, 0.0, 0.0);
    // Rotate the previous pose by 180* about X
    q_rot.setRPY(3.14159, 0.0, 0.0);
    q_new = q_rot * q_orig;
    q_new.normalize();
```

- Python:
```python
    q_orig = quaternion_from_euler(0, 0, 0)
    # Rotate the previous pose by 180* about X
    q_rot = quaternion_from_euler(3.14159, 0, 0)
    q_new = quaternion_multiply(q_rot, q_orig)
```

3. **Inverting a quaternion**
- Easy way to invert a quaternion is to negate x,y,z components:
```py
    q[0] = -q[0]
    q[1] = -q[1]
    q[2] = -q[2]
```
- *Note: Not confuse with negating all elements of the quaternion*

4. **Relative rotations**
- Relative rotation `q_r` that convert `q_1` to `q_2`:
`q_2 = q_r * q_1`
- Can solve `q_r` similarly to a matrix of equations; by right-multiply `q_1_invert` on both sides:
`q_r = q_2 * q_1_inverse`
- Example. Get relative rotation from previous robot pose to current robot pose:
```py
    def quaternion_multiply(q0, q1):
        """
        Multiplies two quaternions.

        Input
        :param q0: A 4 element array containing the first quaternion (q01, q11, q21, q31)
        :param q1: A 4 element array containing the second quaternion (q02, q12, q22, q32)

        Output
        :return: A 4 element array containing the final quaternion (q03,q13,q23,q33) in (w, x, y, z) order

        """
        # Extract the values from q0
        x0 = q0[0]
        y0 = q0[1]
        z0 = q0[2]
        w0 = q0[3]

        # Extract the values from q1
        x1 = q1[0]
        y1 = q1[1]
        z1 = q1[2]
        w1 = q1[3]

        # Compute the product of the two quaternions, term by term
        q0q1_w = w0 * w1 - x0 * x1 - y0 * y1 - z0 * z1
        q0q1_x = w0 * x1 + x0 * w1 + y0 * z1 - z0 * y1
        q0q1_y = w0 * y1 - x0 * z1 + y0 * w1 + z0 * x1
        q0q1_z = w0 * z1 + x0 * y1 - y0 * x1 + z0 * w1

        # Create a 4 element array containing the final quaternion
        final_quaternion = np.array([q0q1_w, q0q1_x, q0q1_y, q0q1_z])

        # Return a 4 element array containing the final quaternion (q02,q12,q22,q32)
        return final_quaternion

    q1_inv[0] = -prev_pose.pose.orientation.x   # Negate for inverse
    q1_inv[1] = -prev_pose.pose.orientation.y   # Negate for inverse
    q1_inv[2] = -prev_pose.pose.orientation.z   # Negate for inverse
    q1_inv[3] = prev_pose.pose.orientation.w

    q2[0] = current_pose.pose.orientation.x
    q2[1] = current_pose.pose.orientation.y
    q2[2] = current_pose.pose.orientation.z
    q2[3] = current_pose.pose.orientation.w

    qr = quaternion_multiply(q2, q1_inv)
```