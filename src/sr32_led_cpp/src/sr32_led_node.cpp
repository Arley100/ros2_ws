#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

#include "sr32_led_cpp/led_controller.hpp"
#include "sr32_led_cpp/led_states.hpp"

namespace sr32_led_cpp {

class Sr32LedNode : public rclcpp::Node {
public:
  Sr32LedNode() : Node("sr32_led_node") {
    this->declare_parameter<std::string>("can_interface", "can0");
    const auto can_interface =
        this->get_parameter("can_interface").as_string();

    controller_ = std::make_unique<LedController>(can_interface);

    const bool ok = controller_->initialize_can();
    if (!ok) {
      RCLCPP_WARN(this->get_logger(),
                  "CAN initialization reported failure on interface '%s'.",
                  can_interface.c_str());
    }

    subscription_ = this->create_subscription<std_msgs::msg::String>(
        "rover_state",
        10,
        std::bind(&Sr32LedNode::state_callback, this, std::placeholders::_1));

    RCLCPP_INFO(this->get_logger(),
                "sr32_led_node started. Listening on topic: rover_state, using CAN interface: %s",
                can_interface.c_str());
  }

private:
  void state_callback(const std_msgs::msg::String::SharedPtr msg) {
    const std::string state = msg->data;
    LedState led_state = LedState::OFF;

    if (state == "teleop") {
      led_state = LedState::TELEOP;
    } else if (state == "autonomy") {
      led_state = LedState::AUTONOMY;
    } else if (state == "goal_reached") {
      led_state = LedState::GOAL_REACHED;
    } else if (state == "emergency_stop") {
      led_state = LedState::EMERGENCY_STOP;
    } else if (state == "off") {
      led_state = LedState::OFF;
    } else {
      RCLCPP_WARN(this->get_logger(),
                  "Unknown rover_state '%s', defaulting to OFF",
                  state.c_str());
      led_state = LedState::OFF;
    }

    const bool sent = controller_->set_state(led_state);
    if (!sent) {
      RCLCPP_ERROR(this->get_logger(),
                   "Failed to stage/send LED command for state '%s'",
                   state.c_str());
      return;
    }

    RCLCPP_INFO(this->get_logger(),
                "Mapped rover_state '%s' to CAN id 0x%X",
                state.c_str(),
                controller_->last_can_id());
  }

  std::unique_ptr<LedController> controller_;
  rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;
};

}  // namespace sr32_led_cpp

int main(int argc, char * argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<sr32_led_cpp::Sr32LedNode>());
  rclcpp::shutdown();
  return 0;
}
