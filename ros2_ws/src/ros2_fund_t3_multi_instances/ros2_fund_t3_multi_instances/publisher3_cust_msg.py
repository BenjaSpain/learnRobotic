"""
ROS 2 publisher node

- This module defines a reusable ROS 2 publisher node.
- Messages published are custom messages: 'Hello.msg'.
- The topic name and message text are configurable through ROS 2 parameters.
"""

import rclpy
from rclpy.node import Node

from ros2_fund_t2_custom_msg.msg import Hello


class Pub3NamesCustomMsg(Node):

    def __init__(self):
        super().__init__('pub3_name_custom_msg')

        # Declare parameters. Assign default values if not provided.
        self.declare_parameter('topic_name', 'my_hello')    
        self.declare_parameter('message_text', 'Task3 (from source): custom Hello message')

        self.topic_name = self.get_parameter('topic_name').value
        self.message_text = self.get_parameter('message_text').value

        self.publisher_ = self.create_publisher(Hello, self.topic_name, 20)

        timer_period = 0.1  # 10 Hz
        self.i = 0
        self.timer = self.create_timer(timer_period, self.timer_callback)
        
        # Initial log message to indicate node initialization and parameters
        self.get_logger().info(
            f'Publisher started. '
            f'node="{self.get_fully_qualified_name()}", '
            f'topic="{self.resolve_topic_name(self.topic_name)}", '
            f'message_text="{self.message_text}"'
        )

    def timer_callback(self):
        msg = Hello()
        msg.my_sf = self.message_text
        msg.my_int = self.i

        self.get_logger().info(
            f'Publishing Task3. "my_sf" = "{msg.my_sf}" - "my_int" = {msg.my_int}'
        )

        self.publisher_.publish(msg)
        self.i += 1


def main(args=None):
    rclpy.init(args=args)

    node = Pub3NamesCustomMsg()

    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()