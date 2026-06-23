import rclpy
from rclpy.node import Node
from std_msgs.msg import Float64MultiArray, Float64
import numpy as np


class ArraySumNode(Node):
    def __init__(self):
        super().__init__('NodeC')
        self.subscription = self.create_subscription(
            Float64MultiArray,
            'BC',
            self.listener_callback,
            10)
        self.publisher_ = self.create_publisher(Float64, 'C', 10)

    def listener_callback(self, msg):
        array_np = np.array(msg.data)
        total_sum = float(np.sum(array_np))
        
        self.get_logger().info(
            f'Received list: {list(msg.data)} | Sum: {total_sum}'
        )

        output_msg = Float64()
        output_msg.data = total_sum
        self.publisher_.publish(output_msg)


def main(args=None):
    rclpy.init(args=args)
    node = ArraySumNode()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        if rclpy.ok():
            rclpy.shutdown()


if __name__ == '__main__':
    main()
