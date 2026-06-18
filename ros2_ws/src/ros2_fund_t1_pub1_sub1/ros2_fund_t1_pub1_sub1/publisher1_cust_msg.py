"""
ROS 2 publisher node

- This module defines a ROS 2 node that publishes messages at 10 Hz.
- Messages publised are custom message: 'Hello.msg'
"""
import rclpy
from rclpy.node import Node
#from std_msgs.msg import String
from ros2_fund_t2_custom_msg.msg import Hello


class Publisher1CustomMsg(Node):

    def __init__(self):
        super().__init__('pub1_custom_msg')
        self.publisher_ = self.create_publisher(Hello, 'my_hello', 20)
        timer_period = 0.1  # 10 Hz
        self.timer = self.create_timer(timer_period, self.timer_callback)
        self.i = 0

    def timer_callback(self):
        msg = Hello()
        msg.my_sf = 'Task1(python): msg type String'
        msg.my_int = self.i

        self.get_logger().info(f'Publishing Task2. "my_sf" = "{msg.my_sf}" - "my_int" = {msg.my_int}')
        self.publisher_.publish(msg)        

        self.i += 1


def main(args=None):
    rclpy.init(args=args)

    publisher1_custom_msg = Publisher1CustomMsg()

    rclpy.spin(publisher1_custom_msg)

    publisher1_custom_msg.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()