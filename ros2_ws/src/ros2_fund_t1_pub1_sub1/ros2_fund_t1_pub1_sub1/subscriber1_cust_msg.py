"""
ROS 2 subscriber node

- This module defines a ROS 2 node that subscribes to messages
- Messages received are custom messages: 'Hello.msg'
- The node subscribes to the topic 'my_hello'.
"""
import rclpy
from rclpy.node import Node
#from std_msgs.msg import String
from ros2_fund_t2_custom_msg.msg import Hello

class Subscriber1CustomMsg(Node):

    def __init__(self):
        super().__init__('sub1_custom_msg')
        self.subscription = self.create_subscription(
            Hello,
            'my_hello',
            self.listener_callback,
            20)
        #self.subscription  # prevent unused variable warning

    def listener_callback(self, msg):
        #self.get_logger().info('Receiver Task2. "my_sf = %s - my_int = %ld"' % (msg.my_sf, msg.my_int))
        self.get_logger().info(f'Receiver Task2. "my_sf = {msg.my_sf} - my_int = {msg.my_int}"')

def main(args=None):
    rclpy.init(args=args)

    subscriber1_custom_msg = Subscriber1CustomMsg()

    rclpy.spin(subscriber1_custom_msg)

    subscriber1_custom_msg.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()