#include "rclcpp/rclcpp.hpp"
#include "ros2_fund_t8_executors/srv/ros_task4.hpp"

#include <functional>
#include <chrono>
#include <cstdlib>
#include <memory>

using namespace std::chrono_literals;

/*
 * Client node used to demonstrate the executor problem.
 * A timer callback performs a synchronous service call.
 * This implementation uses the default SingleThreadedExecutor
 */
class ClientExecutorSingle : public rclcpp::Node
{
public:
  ClientExecutorSingle(int64_t a, int64_t b, int64_t c)
  : Node("cli_executor_single"), a_(a), b_(b), c_(c)
  {
    // Create the service client to server "ros2_task8_node_server"
    client_ =
      this->create_client<ros2_fund_t8_executors::srv::RosTask4>(
      "ros2_task8_node_server");

    // Execute the timer callback every second
    timer_ = this->create_wall_timer(
      1s,
      std::bind(&ClientExecutorSingle::timer_callback, this));
  }

private:
  // Request values
  int64_t a_;
  int64_t b_;
  int64_t c_;

  // Service client
  rclcpp::Client<ros2_fund_t8_executors::srv::RosTask4>::SharedPtr client_;

  // Periodic timer
  rclcpp::TimerBase::SharedPtr timer_;

  // Prevent multiple requests from being sent
  bool request_sent_ = false;

  /*
   * Timer callback
   * Sends a synchronous service request only once
   */
  void timer_callback()
  {
    // Prevent sending multiple requests
    if (request_sent_) {
      return;
    }

    request_sent_ = true;

    // Wait until the service becomes available
    while (!client_->wait_for_service(1s)) {
      if (!rclcpp::ok()) {
        RCLCPP_ERROR(this->get_logger(), "Interrupted while waiting for service.");
        return;
      }

      RCLCPP_INFO(this->get_logger(), "Waiting for service...");
    }

    // Create and populate the service request
    auto request =
      std::make_shared<ros2_fund_t8_executors::srv::RosTask4::Request>();

    request->a = a_;
    request->b = b_;
    request->c = c_;

    RCLCPP_INFO(this->get_logger(), "Timer callback: sending synchronous request.");

    // Send the request asynchronously
    auto future = client_->async_send_request(request);

    // Wait synchronously for the service response.
    // This call is expected to block when executed inside
    // a timer callback using a SingleThreadedExecutor
    auto result =
      rclcpp::spin_until_future_complete(
      this->get_node_base_interface(),
      future);

    // Process the service response
    if (result == rclcpp::FutureReturnCode::SUCCESS) {
      RCLCPP_INFO(this->get_logger(), "Sum = %ld", future.get()->sum);
    } else {
      RCLCPP_ERROR(this->get_logger(), "Service call failed.");
    }

    // Shutdown after processing the first request
    rclcpp::shutdown();
  }
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);

  // Check command-line arguments
  if (argc != 4) {
    RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Usage: cli_executor_single X Y Z");
    return 1;
  }

  // Create the client node
  auto node = std::make_shared<ClientExecutorSingle>(
    std::atoll(argv[1]),
    std::atoll(argv[2]),
    std::atoll(argv[3]));

  // Run using the default SingleThreadedExecutor
  rclcpp::spin(node);

  rclcpp::shutdown();

  return 0;
}