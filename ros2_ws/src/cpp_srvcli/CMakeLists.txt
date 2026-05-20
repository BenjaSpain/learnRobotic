cmake_minimum_required(VERSION 3.8)
project(cpp_srvcli)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(example_interfaces REQUIRED)
find_package(tutorial_interfaces REQUIRED)                        # CHANGE

# Indicate executable, dependencies and installation for the server/client
add_executable(server src/add_two_ints_server.cpp)
add_executable(client src/add_two_ints_client.cpp)
ament_target_dependencies(server rclcpp example_interfaces)
ament_target_dependencies(client rclcpp example_interfaces)
# Added for using interfaces from tutorial_interfaces
add_executable(server_interface src/add_two_ints_server_interface.cpp)  # CHANGE
add_executable(client_interface src/add_two_ints_client_interface.cpp)  # CHANGE
ament_target_dependencies(server_interface rclcpp tutorial_interfaces)  # CHANGE
ament_target_dependencies(client_interface rclcpp tutorial_interfaces)  # CHANGE

install(TARGETS server client server_interface client_interface DESTINATION lib/${PROJECT_NAME})  # CHANGE

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
