//#include <chrono>
#include <memory>
//#include <string>

// Include header for Hello.msg custom message type
#include "ros2_fund_t2_custom_msg/msg/hello.hpp"

#include "rclcpp/rclcpp.hpp"
//#include "std_msgs/msg/string.hpp"

using namespace std::chrono_literals;

/* 
- PublisherTask0 node publishes a std_msgs/msg/String.msg message, 
- Message contain an arbitrary string value 
- Message published on the topic “/my_string” 
- Publish message width 10Hz 
*/

class PublisherCustomMsg : public rclcpp::Node
{
public:
  PublisherCustomMsg()
  : Node("pub_custom_msg"), count_(0)
  {
    // Publisher node, topic "my_msg_custom_topic" of type ros2_fund_t2_custom_msg::msg::Hello, queue size of 20
    publisher_custom_msg = this->create_publisher<ros2_fund_t2_custom_msg::msg::Hello>(topic_name_, buffer_size_);

    auto timer_callback =
      [this]() -> void {
        auto msg_my_string_custom   = ros2_fund_t2_custom_msg::msg::Hello();
        msg_my_string_custom.my_sf  = "Task2: msg custom, type String.";
        msg_my_string_custom.my_int = this->count_++;

        RCLCPP_INFO(this->get_logger(), "Publishing Task2. 'my_sf' = %s - 'my_int' = %ld", msg_my_string_custom.my_sf.c_str(), msg_my_string_custom.my_int);
        this->publisher_custom_msg->publish(msg_my_string_custom);
      };
    timer_ = this->create_wall_timer(timer_period_, timer_callback);
  }

private:
  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Publisher<ros2_fund_t2_custom_msg::msg::Hello>::SharedPtr publisher_custom_msg;
  
  size_t count_;
  std::chrono::milliseconds timer_period_ = 100ms; // 10Hz

  // static constexpr is constructed before Node(...)
  static constexpr const char * topic_name_ = "my_msg_custom_topic";
  static constexpr int buffer_size_ = 20;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<PublisherCustomMsg>());
  rclcpp::shutdown();
  return 0;
}