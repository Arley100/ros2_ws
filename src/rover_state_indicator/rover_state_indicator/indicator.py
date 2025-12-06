import rclpy
from rclpy.node import Node
from std_msgs.msg import String

class RoverStateIndicator(Node):
    def __init__(self):
        super().__init__('rover_state_indicator')

        # subscribe to rover state topic
        self.subscription = self.create_subscription(
            String,
            'rover/state',
            self.listener_callback,
            10
        )

        self.get_logger().info("[LED] Indicator node started and listening on /rover/state ...")

    def listener_callback(self, msg):
        state = msg.data.strip().lower()

        led_color = self.translate_state_to_led(state)

        self.get_logger().info(f"[LED] {led_color}  →  state received: {state}")

    def translate_state_to_led(self, state):
        """
        Maps rover states to LED colors.
        Customize this mapping to match the real rover's LED controller.
        """

        if state in ("autonomy", "auto", "autonomous"):
            return "RED → Autonomy mode"
        elif state in ("teleop", "manual", "operator"):
            return "BLUE → Teleoperation mode"
        elif state in ("success", "gps_success", "reached", "done"):
            return "GREEN → Destination reached"
        elif state in ("parking", "park"):
            return "YELLOW → Parking mode"
        else:
            return "RED → Unknown state (failsafe default)"

def main(args=None):
    rclpy.init(args=args)
    node = RoverStateIndicator()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()

