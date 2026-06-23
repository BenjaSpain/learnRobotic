import rclpy
from rclpy.node import Node
from std_msgs.msg import Float64MultiArray


class ArrayTrimmer(Node):
    def __init__(self):
        super().__init__('NodeA')
        self.declare_parameter('x', 5)
        self.subscription = self.create_subscription(
            Float64MultiArray,
            '/SA',
            self.listener_callback,
            10)
        self.publisher_ = self.create_publisher(Float64MultiArray, 'AB', 10)

    def listener_callback(self, msg):
        x = self.get_parameter('x').get_parameter_value().integer_value
        
        new_msg = Float64MultiArray()
        new_msg.data = msg.data[:-x]
        
        self.publisher_.publish(new_msg)


def main(args=None):
    rclpy.init(args=args)
    node = ArrayTrimmer()
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
