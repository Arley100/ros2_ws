#pragma once

#include <cstdint>
#include <string>

#include "sr32_led_cpp/led_states.hpp"
#include "sr32_led_cpp/sil_protocol.hpp"

namespace sr32_led_cpp {

class LedController {
public:
  explicit LedController(const std::string& can_interface = "vcan0");
  ~LedController();

  bool initialize_can();

  bool set_state(LedState state);
  bool set_rgb(uint8_t r,
               uint8_t g,
               uint8_t b,
               uint8_t brightness = 255,
               bool blink_enable = false,
               uint8_t blink_period_factor = 50);

  bool emergency_off();

  uint32_t last_can_id() const;
  sil_protocol::SilPayload last_payload() const;

private:
  bool send_payload(uint8_t severity,
                    uint8_t instruction,
                    const sil_protocol::SilPayload& payload);

  std::string can_interface_;
  bool can_ready_;
  int can_socket_;

  uint32_t last_can_id_;
  sil_protocol::SilPayload last_payload_;
};

}  // namespace sr32_led_cpp
