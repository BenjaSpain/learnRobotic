#include "rclcpp/rclcpp.hpp"
#include "ros2_fund_t8_executors/srv/ros_task4.hpp"

#include <chrono>
#include <cstdlib>
#include <functional>
#include <future>
#include <memory>

using namespace std::chrono_literals;

/*
 * Client node that solves the executor issue.
 * A timer callback performs a synchronous wait for a service response.
 * This implementation uses a MultiThreadedExecutor and a Reentrant callback group.
 */
class ClientExecutorMulti : public rclcpp::Node
{
public:
  ClientExecutorMulti(int64_t a, int64_t b, int64_t c)
  : Node("cli_executor_multi"), a_(a), b_(b), c_(c)
  {
    // Reentrant callback group allows callbacks from this group to run concurrently.
    callback_group_ = this->create_callback_group(rclcpp::CallbackGroupType::Reentrant);

    // Assign the service client to the reentrant callback group.
    client_ =
      this->create_client<ros2_fund_t8_executors::srv::RosTask4>(
        "ros2_task8_node_server",
        rclcpp::ServicesQoS(),
        callback_group_);

    // Assign the timer to the same reentrant callback group.
    timer_ = this->create_wall_timer(
      1s,
      std::bind(&ClientExecutorMulti::timer_callback, this),
      callback_group_);
  }

private:
  int64_t a_;
  int64_t b_;
  int64_t c_;

  rclcpp::CallbackGroup::SharedPtr callback_group_;
  rclcpp::Client<ros2_fund_t8_executors::srv::RosTask4>::SharedPtr client_;
  rclcpp::TimerBase::SharedPtr timer_;

  bool request_sent_ = false;

  /*
   * Timer callback.
   * Sends one service request and waits synchronously for the result.
   */
  void timer_callback()
  {
    if (request_sent_) {
      return;
    }

    request_sent_ = true;

    // Wait until the service becomes available.
    while (!client_->wait_for_service(1s)) {
      if (!rclcpp::ok()) {
        RCLCPP_ERROR(this->get_logger(), "Interrupted while waiting for service.");
        return;
      }

      RCLCPP_INFO(this->get_logger(), "Waiting for service...");
    }

    // Create and populate the service request.
    auto request =
      std::make_shared<ros2_fund_t8_executors::srv::RosTask4::Request>();

    request->a = a_;
    request->b = b_;
    request->c = c_;

    RCLCPP_INFO(
      this->get_logger(),
      "Timer callback: sending synchronous request using MultiThreadedExecutor.");

    // Send the request asynchronously.
    auto future = client_->async_send_request(request);

    // Synchronous wait without calling spin_until_future_complete().
    // The current callback waits, while another executor thread can process the response.
    while (rclcpp::ok() &&
           future.wait_for(100ms) != std::future_status::ready)
    {
      RCLCPP_INFO(this->get_logger(), "Waiting synchronously for response...");
    }

    if (!rclcpp::ok()) {
      return;
    }

    // Process the service response.
    RCLCPP_INFO(this->get_logger(), "Sum = %ld", future.get()->sum);

    rclcpp::shutdown();
  }
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);

  if (argc != 4) {
    RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Usage: cli_executor_multi X Y Z");
    return 1;
  }

  auto node = std::make_shared<ClientExecutorMulti>(
    std::atoll(argv[1]),
    std::atoll(argv[2]),
    std::atoll(argv[3]));

  // MultiThreadedExecutor allows one thread to wait inside the timer callback
  // while another thread processes the service response.
  rclcpp::executors::MultiThreadedExecutor executor;
  executor.add_node(node);
  executor.spin();

  rclcpp::shutdown();

  return 0;
}