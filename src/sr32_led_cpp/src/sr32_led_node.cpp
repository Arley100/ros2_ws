#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "sr32_led_cpp/led_controller.hpp"
#include "sr32_led_cpp/led_states.hpp"

class SR32LedCppNode : public rclcpp::Node
{
public:
    SR32LedCppNode()
    : Node("sr32_led_cpp_node")
    {
        controller_.initialize_can();

        subscription_ = this->create_subscription<std_msgs::msg::String>(
            "/rover/state",
            10,
            std::bind(&SR32LedCppNode::state_callback, this, std::placeholders::_1)
        );

        RCLCPP_INFO(this->get_logger(), "SR-32 C++ node started");
    }

private:
    void state_callback(const std_msgs::msg::String::SharedPtr msg)
    {
        std::string state = msg->data;
        for (auto & c : state) c = std::tolower(c);

        if (state == "autonomy")
        {
            controller_.send_led_command(LedState::AUTONOMY_RED);
            RCLCPP_INFO(this->get_logger(), "Autonomy detected -> RED LED");
        }
        else if (state == "teleop")
        {
            controller_.send_led_command(LedState::TELEOP_BLUE);
            RCLCPP_INFO(this->get_logger(), "Teleop detected -> BLUE LED");
        }
        else if (state == "gps success" || state == "gps_success" || state == "success")
        {
            controller_.send_led_command(LedState::GPS_SUCCESS_GREEN);
            RCLCPP_INFO(this->get_logger(), "GPS success detected -> GREEN LED");
        }
        else
        {
            RCLCPP_WARN(this->get_logger(), "Unknown rover state: %s", msg->data.c_str());
        }
    }

    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;
    LedController controller_;
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<SR32LedCppNode>());
    rclcpp::shutdown();
    return 0;
}
