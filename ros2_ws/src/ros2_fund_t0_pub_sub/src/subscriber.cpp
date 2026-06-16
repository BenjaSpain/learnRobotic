#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

/* 
- PublisherTask0 node publishes a std_msgs/msg/String.msg message, 
- Message contain an arbitrary string value 
- Message published on the topic “/my_string” 
- Publish message width 10Hz 
*/

class SubscriberTask0 : public rclcpp::Node
{
public:
  SubscriberTask0(): Node("subscriber_task0")
  {
    auto topic_callback =
      [this](std_msgs::msg::String::UniquePtr msg) -> void {
        RCLCPP_INFO(this->get_logger(), "Task0: msg type String. Received: '%s'", msg->data.c_str());
      };
    subscription_ =
      this->create_subscription<std_msgs::msg::String>("my_string", 10, topic_callback);
  }

private:
  rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<SubscriberTask0>());
  rclcpp::shutdown();
  return 0;
}