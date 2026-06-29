#include "rclcpp/rclcpp.hpp"
#include "ros2_fund_t8_executors/srv/ros_task4.hpp"                                        

#include <memory>

void add(const std::shared_ptr<ros2_fund_t8_executors::srv::RosTask4::Request> request,     
          std::shared_ptr<ros2_fund_t8_executors::srv::RosTask4::Response>       response)  
{
  response->sum = request->a + request->b + request->c;                                     
  RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Incoming request\na: %ld" " b: %ld" " c: %ld", 
                request->a, request->b, request->c);                                         
  RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "sending back response: [%ld]", (long int)response->sum);
}


int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);

  std::shared_ptr<rclcpp::Node> node = rclcpp::Node::make_shared("ros2_task8_node_server");

  rclcpp::Service<ros2_fund_t8_executors::srv::RosTask4>::SharedPtr service_task8 =              
    node->create_service<ros2_fund_t8_executors::srv::RosTask4>("ros2_task8_node_server", &add);

  RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "READY to sum three ints....");

  rclcpp::spin(node);
  rclcpp::shutdown();
}