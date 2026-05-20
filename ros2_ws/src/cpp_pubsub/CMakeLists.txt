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

# Add path of source for build executable and dependencies
add_executable(talker src/publisher_lambda_function.cpp)
add_executable(listener src/subscriber_lambda_function.cpp)
# Add dependencies for the executable
ament_target_dependencies(talker rclcpp std_msgs)
ament_target_dependencies(listener rclcpp std_msgs)


 # Let ros2 run find executable
install(TARGETS talker listener DESTINATION lib/${PROJECT_NAME})

ament_package()

