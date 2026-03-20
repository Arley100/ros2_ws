import rclpy
from rclpy.node import Node
from std_msgs.msg import String
from .can_stub import send_led_command_stub


class SR32LedPyNode(Node):
    def __init__(self):
        super().__init__('sr32_led_py_node')

        # TODO: confirm official topic name with team
        self.subscription = self.create_subscription(
            String,
            '/rover/state',
            self.state_callback,
            10
        )

        self.get_logger().info('SR-32 Python node started')

    def state_callback(self, msg: String):
        state = msg.data.strip().lower()

        if state == 'autonomy':
            send_led_command_stub('RED')
            self.get_logger().info('Autonomy detected -> RED LED')
        elif state == 'teleop':
            send_led_command_stub('BLUE')
            self.get_logger().info('Teleop detected -> BLUE LED')
        elif state in ('gps success', 'gps_success', 'success'):
            send_led_command_stub('GREEN')
            self.get_logger().info('GPS success detected -> GREEN LED')
        else:
            self.get_logger().warn(f'Unknown rover state: {msg.data}')


def main(args=None):
    rclpy.init(args=args)
    node = SR32LedPyNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()
