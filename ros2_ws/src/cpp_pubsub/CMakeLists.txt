cmake_minimum_required(VERSION 3.8)
project(cpp_pubsub)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(tutorial_interfaces REQUIRED)                      # CHANGE

# Add path of source for build executable and dependencies
add_executable(talker src/publisher_lambda_function.cpp)
add_executable(listener src/subscriber_lambda_function.cpp)
# Add dependencies for the executable
ament_target_dependencies(talker rclcpp std_msgs)
ament_target_dependencies(listener rclcpp std_msgs)

# Add path and dependencies for interfaces
add_executable(talker_interfaces src/publisher_lambda_function_interf.cpp)       # CHANGE
add_executable(listener_interfaces src/subscriber_lambda_function_interf.cpp)    # CHANGE
ament_target_dependencies(talker_interfaces rclcpp tutorial_interfaces)          # CHANGE
ament_target_dependencies(listener_interfaces rclcpp tutorial_interfaces)       # CHANGE

 # Let ros2 run find executable
install(TARGETS talker listener talker_interfaces listener_interfaces DESTINATION lib/${PROJECT_NAME}) # CHANGE

ament_package()

