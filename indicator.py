import rclpy
from rclpy.node import Node

class RoverStateIndicator(Node):
    def __init__(self):
        super().__init__('indicator')
        self.get_logger().info("Indicator node started.")

def main(args=None):
    rclpy.init(args=args)
    node = RoverStateIndicator()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()
