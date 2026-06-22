#include "rclcpp/rclcpp.hpp"
#include "ros2_fund_t4_cli_ser/srv/ros_task4.hpp"

#include <memory>

void add(
  const std::shared_ptr<ros2_fund_t4_cli_ser::srv::RosTask4::Request> request,
  std::shared_ptr<ros2_fund_t4_cli_ser::srv::RosTask4::Response> response)
{
  response->sum = request->a + request->b + request->c;

  RCLCPP_INFO(
    rclcpp::get_logger("rclcpp"),
    "Pair2 request: a=%ld b=%ld c=%ld",
    request->a,
    request->b,
    request->c);

  RCLCPP_INFO(
    rclcpp::get_logger("rclcpp"),
    "Pair2 response: sum=%ld",
    response->sum);
}

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);

  auto node = rclcpp::Node::make_shared("ros2_task4_node_service2");

  auto service = node->create_service<ros2_fund_t4_cli_ser::srv::RosTask4>(
    "ros2_task4_pair2",
    &add);

  RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "READY pair2 service: sum three ints");

  rclcpp::spin(node);
  rclcpp::shutdown();

  return 0;
}