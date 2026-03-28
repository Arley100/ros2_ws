#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include "sr32_led_cpp/led_controller.hpp"

using namespace std::chrono_literals;

int main(int argc, char* argv[]) {
  std::string can_interface = "can0";
  if (argc > 1) {
    can_interface = argv[1];
  }

  sr32_led_cpp::LedController controller(can_interface);

  if (!controller.initialize_can()) {
    std::cerr << "Failed to initialize CAN interface: " << can_interface << std::endl;
    return 1;
  }

  std::cout << "Using CAN interface: " << can_interface << std::endl;

  std::cout << "Sending TELEOP (blue solid)..." << std::endl;
  controller.set_rgb(0, 0, 255, 255, false, 50);
  std::this_thread::sleep_for(1s);

  std::cout << "Sending AUTONOMY (green solid)..." << std::endl;
  controller.set_rgb(0, 255, 0, 255, false, 50);
  std::this_thread::sleep_for(1s);

  std::cout << "Sending GOAL_REACHED (green blinking)..." << std::endl;
  controller.set_rgb(0, 255, 0, 255, true, 50);
  std::this_thread::sleep_for(1s);

  std::cout << "Sending EMERGENCY OFF..." << std::endl;
  controller.emergency_off();
  std::this_thread::sleep_for(500ms);

  std::cout << "Standalone SIL test complete." << std::endl;
  return 0;
}
