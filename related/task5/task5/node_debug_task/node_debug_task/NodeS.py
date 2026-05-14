import rclpy
from rclpy.node import Node
from std_msgs.msg import Float64MultiArray
import numpy as np


class ArrayPublisher(Node):
    def __init__(self):
        super().__init__('NodeS')

        self.declare_parameter('s', 10)

        self.publisher_ = self.create_publisher(Float64MultiArray, '/SA', 10)
        self.timer      = self.create_timer(1.0, self.timer_callback)
        self.s = self.get_parameter('s').get_parameter_value().integer_value
    
    def timer_callback(self):
        array_np = np.random.rand(self.s, 1)
        
        msg = Float64MultiArray()
        msg.data = array_np.flatten().tolist()

        self.publisher_.publish(msg)


def main(args=None):
    rclpy.init(args=args)
    node = ArrayPublisher()
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
