cmake_minimum_required(VERSION 3.8)
project(ros2_fund_t8_executors)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rosidl_default_generators REQUIRED)

# Generate service interface
rosidl_generate_interfaces(${PROJECT_NAME} "srv/RosTask4.srv")

# Get C++ typesupport target for the generated service interface
rosidl_get_typesupport_target(cpp_typesupport_target 
  ${PROJECT_NAME}
  "rosidl_typesupport_cpp"
)

# Server executable
add_executable(serv_sum_three src/serv_sum_three.cpp)

ament_target_dependencies(serv_sum_three rclcpp)

target_link_libraries(serv_sum_three "${cpp_typesupport_target}")

# Client SingleExecutor executable
add_executable(cli_executor_single src/cli_executor_single.cpp)

ament_target_dependencies(cli_executor_single rclcpp)

target_link_libraries(cli_executor_single "${cpp_typesupport_target}")

# Client MultiExecutor executable
add_executable(cli_executor_multi src/cli_executor_multi.cpp)

ament_target_dependencies(cli_executor_multi rclcpp)

target_link_libraries(cli_executor_multi "${cpp_typesupport_target}")

# Install executables
install(TARGETS
  serv_sum_three
  cli_executor_single
  cli_executor_multi
  DESTINATION lib/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)

  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)

  ament_lint_auto_find_test_dependencies()
endif()

ament_export_dependencies(rosidl_default_runtime)

ament_package()