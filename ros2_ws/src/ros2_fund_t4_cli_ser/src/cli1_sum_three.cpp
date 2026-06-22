#include "rclcpp/rclcpp.hpp"
#include "ros2_fund_t4_cli_ser/srv/ros_task4.hpp"                             

#include <cstdlib>
#include <memory>
#include <chrono>

using namespace std::chrono_literals;


int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);

    if (argc != 4) {
        RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Usage: add_three_ints_client X Y Z");
        return 1;
    }

    std::shared_ptr<rclcpp::Node> node = rclcpp::Node::make_shared("ros2_task4_node_client1");  
    rclcpp::Client<ros2_fund_t4_cli_ser::srv::RosTask4>::SharedPtr client =           
    node->create_client<ros2_fund_t4_cli_ser::srv::RosTask4>("ros2_task4_pair1");          

    auto request = std::make_shared<ros2_fund_t4_cli_ser::srv::RosTask4::Request>();    
    request->a = atoll(argv[1]);
    request->b = atoll(argv[2]);
    request->c = atoll(argv[3]);                                                            

    while (!client->wait_for_service(1s)) {
        if (!rclcpp::ok()) {
            RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "Interrupted while waiting for the service of ros2_task4_pair1. Exiting.");
            return 0;
        }
        RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Service ros2_task4_pair1 not available, waiting...");
    }

    auto response_callback = [node](rclcpp::Client<ros2_fund_t4_cli_ser::srv::RosTask4>::SharedFuture future) {
        auto response = future.get();

        RCLCPP_INFO(
        node->get_logger(),
        "Async response received. Sum: %ld",
        response->sum);

        rclcpp::shutdown();
    };

    client->async_send_request(request, response_callback);

    RCLCPP_INFO(node->get_logger(), "Request sent asynchronously. Main thread is not waiting for the result directly.");

    rclcpp::spin(node);

  return 0;
}