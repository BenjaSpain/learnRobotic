cmake_minimum_required(VERSION 3.8)
project(ros2_fund_t4_cli_ser)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)

# Add use of services to the package
find_package(rosidl_default_generators REQUIRED)
rosidl_generate_interfaces(${PROJECT_NAME}
  "srv/RosTask4.srv"
)

# Add C++ typesupport target. Need to use interface 'RosTask4.srv' created inside the package
rosidl_get_typesupport_target(cpp_typesupport_target
  ${PROJECT_NAME}
  rosidl_typesupport_cpp
)

# Add executables for the pair1: client/service nodes
add_executable(service1 src/serv1_sum_three.cpp)
ament_target_dependencies(service1 rclcpp)
# Need for service1 to handle the service requests and responses
target_link_libraries(service1 ${cpp_typesupport_target})

# Add executables for the pair1: client/service nodes
add_executable(client1 src/cli1_sum_three.cpp)
ament_target_dependencies(client1 rclcpp)
# Need for client1 to handle the service requests and responses
target_link_libraries(client1 ${cpp_typesupport_target})

# Add executables for the pair2: client/service nodes
add_executable(service2 src/serv2_sum_three.cpp)
ament_target_dependencies(service2 rclcpp)
# Need for service2 to handle the service requests and responses
target_link_libraries(service2 ${cpp_typesupport_target})

# Add executables for the pair2: client/service nodes
add_executable(client2 src/cli2_sum_three_block.cpp)
ament_target_dependencies(client2 rclcpp)
# Need for client2 to handle the service requests and responses
target_link_libraries(client2 ${cpp_typesupport_target})


# Executables to be installed
install(TARGETS service1 client1 service2 client2 DESTINATION lib/${PROJECT_NAME})


if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
