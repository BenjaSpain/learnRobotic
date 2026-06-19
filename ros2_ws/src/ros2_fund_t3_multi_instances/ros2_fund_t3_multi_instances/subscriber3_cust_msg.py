"""
ROS 2 subscriber node

- This module defines a reusable ROS 2 subscriber node.
- Messages received are custom messages: 'Hello.msg'.
- The topic name is configurable through a ROS 2 parameter.
"""

import rclpy
from rclpy.node import Node

from ros2_fund_t2_custom_msg.msg import Hello


class Subs3NamesCustomMsg(Node):

    def __init__(self):
        super().__init__('subs3_names_custom_msg')

        self.declare_parameter('topic_name', 'my_hello')
        self.topic_name = self.get_parameter('topic_name').value

        self.subscription = self.create_subscription(
            Hello,
            self.topic_name,
            self.listener_callback,
            20
        )

        self.get_logger().info(
            f'Subscriber started. '
            f'node="{self.get_fully_qualified_name()}", '
            f'topic="{self.resolve_topic_name(self.topic_name)}", '
            f'Waiting for messages...'
        )

    def listener_callback(self, msg):
        self.get_logger().info(
            f'Receiver Task3. "my_sf" = "{msg.my_sf}" - "my_int" = {msg.my_int}'
        )


def main(args=None):
    rclpy.init(args=args)

    node = Subs3NamesCustomMsg()

    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()