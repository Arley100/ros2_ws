#ifndef SR32_LED_CPP__LED_CONTROLLER_HPP_
#define SR32_LED_CPP__LED_CONTROLLER_HPP_

#include "sr32_led_cpp/led_states.hpp"

class LedController
{
public:
    LedController();

    bool initialize_can();
    bool send_led_command(LedState state);

private:
    // TODO: replace placeholders with confirmed CAN values
    unsigned int led_can_id_;
    bool use_extended_frame_;
};

#endif
