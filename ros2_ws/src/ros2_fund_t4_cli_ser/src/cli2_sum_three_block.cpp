#include "rclcpp/rclcpp.hpp"
#include "ros2_fund_t4_cli_ser/srv/ros_task4.hpp"

#include <chrono>
#include <cstdlib>
#include <memory>

using namespace std::chrono_literals;

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);

  if (argc != 4) {
    RCLCPP_INFO(
      rclcpp::get_logger("rclcpp"),
      "Usage: cli2_sum_three X Y Z");
    return 1;
  }

  auto node = rclcpp::Node::make_shared("ros2_task4_node_client2");

  auto client = node->create_client<ros2_fund_t4_cli_ser::srv::RosTask4>(
    "ros2_task4_pair2");

  auto request = std::make_shared<ros2_fund_t4_cli_ser::srv::RosTask4::Request>();
  request->a = std::atoll(argv[1]);
  request->b = std::atoll(argv[2]);
  request->c = std::atoll(argv[3]);

  while (!client->wait_for_service(1s)) {
    if (!rclcpp::ok()) {
      RCLCPP_ERROR(
        rclcpp::get_logger("rclcpp"),
        "Interrupted while waiting for service ros2_task4_pair2.");
      return 1;
    }

    RCLCPP_INFO(
      rclcpp::get_logger("rclcpp"),
      "Service ros2_task4_pair2 not available, waiting...");
  }

  auto future = client->async_send_request(request);

  auto result = rclcpp::spin_until_future_complete(
    node,
    future,
    5s);

  if (result == rclcpp::FutureReturnCode::SUCCESS) {
    RCLCPP_INFO(
      rclcpp::get_logger("rclcpp"),
      "Pair2 result: %ld + %ld + %ld = %ld",
      request->a,
      request->b,
      request->c,
      future.get()->sum);
  } else if (result == rclcpp::FutureReturnCode::TIMEOUT) {
    RCLCPP_ERROR(
      rclcpp::get_logger("rclcpp"),
      "Service 'ros2_task4_pair2' call timed out.");
  } else {
    RCLCPP_ERROR(
      rclcpp::get_logger("rclcpp"),
      "Service 'ros2_task4_pair2' call failed.");
  }

  rclcpp::shutdown();
  return 0;
}