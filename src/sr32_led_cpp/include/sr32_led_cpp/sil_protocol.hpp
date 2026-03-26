#pragma once

#include <cstdint>

namespace sr32_led_cpp {
namespace sil_protocol {

// 29-bit extended CAN layout:
// bits 28:24 -> DeviceType   (5 bits)
// bits 23:16 -> Manufacturer (8 bits)
// bits 15:14 -> Severity     (2 bits)
// bits 13:6  -> Instruction  (8 bits)
// bits  5:0  -> DeviceId     (6 bits)

inline constexpr uint8_t DEVICE_TYPE  = 0x0E; // placeholder until final confirmation
inline constexpr uint8_t MANUFACTURER = 0x08; // TEAM_USE
inline constexpr uint8_t DEVICE_ID    = 0x0F; // confirmed by Michael for current implementation

inline constexpr uint8_t SEV_EMERGENCY = 0x00;
inline constexpr uint8_t SEV_NORMAL    = 0x02;

inline constexpr uint8_t INST_SET_LED = 0x00; // placeholder until final confirmation

struct SilPayload {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t brightness;
  uint8_t blink_enable;
  uint8_t blink_period_factor;
  uint8_t reserved0;
  uint8_t reserved1;
};

inline uint32_t build_can_id(uint8_t device_type,
                             uint8_t manufacturer,
                             uint8_t severity,
                             uint8_t instruction,
                             uint8_t device_id) {
  const uint32_t dt   = static_cast<uint32_t>(device_type) & 0x1Fu;
  const uint32_t mfg  = static_cast<uint32_t>(manufacturer) & 0xFFu;
  const uint32_t sev  = static_cast<uint32_t>(severity) & 0x03u;
  const uint32_t inst = static_cast<uint32_t>(instruction) & 0xFFu;
  const uint32_t did  = static_cast<uint32_t>(device_id) & 0x3Fu;

  return (dt << 24) | (mfg << 16) | (((sev << 8) | inst) << 6) | did;
}

inline uint32_t build_sil_can_id(uint8_t severity, uint8_t instruction) {
  return build_can_id(DEVICE_TYPE, MANUFACTURER, severity, instruction, DEVICE_ID);
}

inline SilPayload make_payload(uint8_t r,
                               uint8_t g,
                               uint8_t b,
                               uint8_t brightness,
                               bool blink_enable,
                               uint8_t blink_period_factor) {
  return SilPayload{
      r,
      g,
      b,
      brightness,
      static_cast<uint8_t>(blink_enable ? 1 : 0),
      blink_period_factor,
      0,
      0};
}

}  // namespace sil_protocol
}  // namespace sr32_led_cpp
