import rclpy
from rclpy.node import Node

from std_msgs.msg import String


class Subscriber1Task1(Node):

    def __init__(self):
        super().__init__('subscriber1_task1')
        self.subscription = self.create_subscription(
            String,
            'my_string',
            self.listener_callback,
            20)
        self.subscription  # prevent unused variable warning

    def listener_callback(self, msg):
        self.get_logger().info('I heard: "%s"' % msg.data)


def main(args=None):
    rclpy.init(args=args)

    subscriber1_task1 = Subscriber1Task1()

    rclpy.spin(subscriber1_task1)

    # Destroy the node explicitly
    # (optional - otherwise it will be done automatically
    # when the garbage collector destroys the node object)
    subscriber1_task1.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()