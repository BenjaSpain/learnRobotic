"""
ROS 2 string publisher node.

This module defines a ROS 2 node that publishes String messages to the
'my_string' topic at 10 Hz.
"""

import rclpy
from rclpy.node import Node

from std_msgs.msg import String


class Publisher1Task1(Node):

    def __init__(self):
        super().__init__('publisher1_task1')
        self.publisher_ = self.create_publisher(String, 'my_string', 20)
        timer_period = 0.1  # 10 Hz
        self.timer = self.create_timer(timer_period, self.timer_callback)
        self.i = 0

    def timer_callback(self):
        msg = String()
        msg.data = 'Task1(python): msg type String. Number: %d' % self.i
        self.publisher_.publish(msg)
        self.get_logger().info('Publishing: "%s"' % msg.data)
        self.i += 1


def main(args=None):
    rclpy.init(args=args)

    publisher1_task1 = Publisher1Task1()

    rclpy.spin(publisher1_task1)

    # Destroy the node explicitly
    # (optional - otherwise it will be done automatically
    # when the garbage collector destroys the node object)
    publisher1_task1.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()