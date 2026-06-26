cmake_minimum_required(VERSION 3.8)
project(ros2_fund_t7_action)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(turtlesim REQUIRED)
find_package(rosidl_default_generators REQUIRED)

rosidl_generate_interfaces(${PROJECT_NAME} "action/NavigateToPose.action" )

rosidl_get_typesupport_target(cpp_typesupport_target ${PROJECT_NAME} rosidl_typesupport_cpp)

# Add executable for server and client
add_executable(cli_navigate_to src/navigate_to_cli.cpp)
ament_target_dependencies(cli_navigate_to rclcpp rclcpp_action)
target_link_libraries(cli_navigate_to "${cpp_typesupport_target}")

# Add executable for publisher
add_executable(serv_navigate_to src/navigate_to_serv.cpp)
ament_target_dependencies(serv_navigate_to rclcpp rclcpp_action geometry_msgs turtlesim)
target_link_libraries(serv_navigate_to "${cpp_typesupport_target}")

# Executables to be installed
install(TARGETS serv_navigate_to cli_navigate_to DESTINATION lib/${PROJECT_NAME})


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
