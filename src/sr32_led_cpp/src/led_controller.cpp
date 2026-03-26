#include "sr32_led_cpp/led_controller.hpp"

#include <cerrno>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <linux/can.h>
#include <linux/can/raw.h>

namespace sr32_led_cpp {

LedController::LedController(const std::string& can_interface)
    : can_interface_(can_interface),
      can_ready_(false),
      can_socket_(-1),
      last_can_id_(0),
      last_payload_{0, 0, 0, 0, 0, 0, 0, 0} {}

LedController::~LedController() {
  if (can_socket_ >= 0) {
    close(can_socket_);
    can_socket_ = -1;
  }
}

bool LedController::initialize_can() {
  can_socket_ = socket(PF_CAN, SOCK_RAW, CAN_RAW);
  if (can_socket_ < 0) {
    std::cerr << "[LedController] Failed to create CAN socket: "
              << std::strerror(errno) << std::endl;
    can_ready_ = false;
    return false;
  }

  struct ifreq ifr {};
  std::strncpy(ifr.ifr_name, can_interface_.c_str(), IFNAMSIZ - 1);

  if (ioctl(can_socket_, SIOCGIFINDEX, &ifr) < 0) {
    std::cerr << "[LedController] Failed to resolve interface '" << can_interface_
              << "': " << std::strerror(errno) << std::endl;
    close(can_socket_);
    can_socket_ = -1;
    can_ready_ = false;
    return false;
  }

  struct sockaddr_can addr {};
  addr.can_family = AF_CAN;
  addr.can_ifindex = ifr.ifr_ifindex;

  if (bind(can_socket_, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) < 0) {
    std::cerr << "[LedController] Failed to bind CAN socket to '" << can_interface_
              << "': " << std::strerror(errno) << std::endl;
    close(can_socket_);
    can_socket_ = -1;
    can_ready_ = false;
    return false;
  }

  std::cout << "[LedController] initialize_can() on interface: " << can_interface_ << std::endl;
  can_ready_ = true;
  return true;
}

bool LedController::set_state(LedState state) {
  switch (state) {
    case LedState::OFF:
      return set_rgb(0, 0, 0, 0, false, 0);

    case LedState::TELEOP:
      return set_rgb(0, 0, 255, 255, false, 50);

    case LedState::AUTONOMY:
      return set_rgb(0, 255, 0, 255, false, 50);

    case LedState::GOAL_REACHED:
      return set_rgb(0, 255, 0, 255, true, 50);

    case LedState::EMERGENCY_STOP:
      return emergency_off();
  }

  return false;
}

bool LedController::set_rgb(uint8_t r,
                            uint8_t g,
                            uint8_t b,
                            uint8_t brightness,
                            bool blink_enable,
                            uint8_t blink_period_factor) {
  const auto payload = sil_protocol::make_payload(
      r, g, b, brightness, blink_enable, blink_period_factor);

  return send_payload(
      sil_protocol::SEV_NORMAL,
      sil_protocol::INST_SET_LED,
      payload);
}

bool LedController::emergency_off() {
  const auto payload = sil_protocol::make_payload(
      0, 0, 0, 0, false, 0);

  return send_payload(
      sil_protocol::SEV_EMERGENCY,
      sil_protocol::INST_SET_LED,
      payload);
}

bool LedController::send_payload(uint8_t severity,
                                 uint8_t instruction,
                                 const sil_protocol::SilPayload& payload) {
  if (!can_ready_ || can_socket_ < 0) {
    std::cerr << "[LedController] CAN not initialized." << std::endl;
    return false;
  }

  last_can_id_ = sil_protocol::build_sil_can_id(severity, instruction);
  last_payload_ = payload;

  struct can_frame frame {};
  frame.can_id = last_can_id_ | CAN_EFF_FLAG;
  frame.can_dlc = 8;
  std::memcpy(frame.data, &payload, sizeof(payload));

  std::ostringstream oss;
  oss << "[LedController] sending extended CAN frame:"
      << " can_id=0x" << std::hex << std::uppercase << last_can_id_
      << " payload=["
      << std::dec
      << static_cast<int>(payload.r) << ", "
      << static_cast<int>(payload.g) << ", "
      << static_cast<int>(payload.b) << ", "
      << static_cast<int>(payload.brightness) << ", "
      << static_cast<int>(payload.blink_enable) << ", "
      << static_cast<int>(payload.blink_period_factor) << ", "
      << static_cast<int>(payload.reserved0) << ", "
      << static_cast<int>(payload.reserved1) << "]";

  std::cout << oss.str() << std::endl;

  const auto bytes_written = write(can_socket_, &frame, sizeof(frame));
  if (bytes_written != static_cast<ssize_t>(sizeof(frame))) {
    std::cerr << "[LedController] Failed to write CAN frame: "
              << std::strerror(errno) << std::endl;
    return false;
  }

  return true;
}

uint32_t LedController::last_can_id() const {
  return last_can_id_;
}

sil_protocol::SilPayload LedController::last_payload() const {
  return last_payload_;
}

}  // namespace sr32_led_cpp
