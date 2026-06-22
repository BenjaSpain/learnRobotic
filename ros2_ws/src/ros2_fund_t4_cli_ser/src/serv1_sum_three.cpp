#include "rclcpp/rclcpp.hpp"
#include "ros2_fund_t4_cli_ser/srv/ros_task4.hpp"                                        

#include <memory>

void add(const std::shared_ptr<ros2_fund_t4_cli_ser::srv::RosTask4::Request> request,     
          std::shared_ptr<ros2_fund_t4_cli_ser::srv::RosTask4::Response>       response)  
{
  response->sum = request->a + request->b + request->c;                                     
  RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Incoming request\na: %ld" " b: %ld" " c: %ld", 
                request->a, request->b, request->c);                                         
  RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "sending back response: [%ld]", (long int)response->sum);
}

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);

  std::shared_ptr<rclcpp::Node> node = rclcpp::Node::make_shared("ros2_task4_node_service1");

  rclcpp::Service<ros2_fund_t4_cli_ser::srv::RosTask4>::SharedPtr service1 =              
    node->create_service<ros2_fund_t4_cli_ser::srv::RosTask4>("ros2_task4_pair1", &add);

  RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "READY to sum three ints....");

  rclcpp::spin(node);
  rclcpp::shutdown();
}