#include <memory>

#include "rclcpp/rclcpp.hpp"
//#include "std_msgs/msg/string.hpp"

// Include header for Hello.msg custom message type
#include "ros2_fund_t2_custom_msg/msg/hello.hpp"

/* 
- Subscriber node to a ros2_fund_t2_custom_msg/msg/Hello.msg message, 
- Message contain an arbitrary string value and an integer value
- Message published on the topic “/my_msg_custom” 
- Publish message width 10Hz 
*/

class SubscriberCustomMsg : public rclcpp::Node
{
public:
  SubscriberCustomMsg(): Node("subs_custom_msg")
  {
    auto topic_callback =
      [this](ros2_fund_t2_custom_msg::msg::Hello::UniquePtr msg) -> void {
        RCLCPP_INFO(this->get_logger(), "Task2: msg custom. Received: 'my_sf' = %s - 'my_int' = %ld", msg->my_sf.c_str(), msg->my_int);
      };
    subscription_ =
      this->create_subscription<ros2_fund_t2_custom_msg::msg::Hello>(topic_name_, buffer_size_, topic_callback);
  }

private:
  rclcpp::Subscription<ros2_fund_t2_custom_msg::msg::Hello>::SharedPtr subscription_;

  // static constexpr is constructed before Node(...)
  static constexpr const char * topic_name_ = "my_msg_custom_topic";
  static constexpr int buffer_size_ = 20;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<SubscriberCustomMsg>());
  rclcpp::shutdown();
  return 0;
}