#include <chrono>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

using namespace std::chrono_literals;

/* 
- PublisherTask0 node publishes a std_msgs/msg/String.msg message, 
- Message contain an arbitrary string value 
- Message published on the topic “/my_string” 
- Publish message width 10Hz 
*/

class PublisherTask0 : public rclcpp::Node
{
public:
  PublisherTask0()
  : Node("publisher_task0"), count_(0)
  {
    // Publisher node, topic "my_string" of type std_msgs::msg::String, queue size of 20
    publisher_ = this->create_publisher<std_msgs::msg::String>("my_string", 20);
    auto timer_callback =
      [this]() -> void {
        auto msg_my_string = std_msgs::msg::String();
        msg_my_string.data = "Task0: msg type String. Number: " + std::to_string(this->count_++);
        RCLCPP_INFO(this->get_logger(), "Publishing: '%s'", msg_my_string.data.c_str());
        this->publisher_->publish(msg_my_string);
      };
    timer_ = this->create_wall_timer(100ms, timer_callback);
  }

private:
  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
  size_t count_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<PublisherTask0>());
  rclcpp::shutdown();
  return 0;
}