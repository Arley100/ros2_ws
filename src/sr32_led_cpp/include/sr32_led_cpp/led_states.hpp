#pragma once

namespace sr32_led_cpp {

enum class LedState {
  OFF = 0,
  TELEOP,
  AUTONOMY,
  GOAL_REACHED,
  EMERGENCY_STOP
};

}  // namespace sr32_led_cpp
