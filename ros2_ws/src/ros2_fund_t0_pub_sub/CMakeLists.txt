cmake_minimum_required(VERSION 3.8)
project(ros2_fund_t0_pub_sub)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)

find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(ros2_fund_t2_custom_msg REQUIRED)                  # ADDED for Task2: custom message type Hello.msg

add_executable(publisher src/publisher.cpp)
ament_target_dependencies(publisher rclcpp std_msgs)

add_executable(subscriber src/subscriber.cpp)
ament_target_dependencies(subscriber rclcpp std_msgs)

add_executable(pub_cust_msg src/publisher_cust_msg.cpp)         # ADDED for Task2: custom message type Hello.msg
ament_target_dependencies(pub_cust_msg rclcpp ros2_fund_t2_custom_msg) # ADDED for Task2: custom message type Hello.msg

add_executable(sub_cust_msg src/subscriber_cust_msg.cpp)        # ADDED for Task2: custom message type Hello.msg
ament_target_dependencies(sub_cust_msg rclcpp ros2_fund_t2_custom_msg) # ADDED for Task2: custom message type Hello.msg


install(TARGETS
  publisher
  subscriber
  pub_cust_msg                                                  # ADDED for Task2: custom message type Hello.msg
  sub_cust_msg                                                  # ADDED for Task2: custom message type Hello.msg
  DESTINATION lib/${PROJECT_NAME})

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
