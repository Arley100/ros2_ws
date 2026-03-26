# ROS 2 Workspace

## SR-32 LED Controller (ROS2)

### Overview

This workspace contains the `sr32_led_cpp` package, which implements the SR-32 / SIL LED controller for Space Concordia Robotics.

The controller:
- subscribes to rover state updates through ROS2
- maps rover states to LED behaviors
- builds SIL CAN frames using the agreed 29-bit CAN layout
- sends frames over SocketCAN
- includes a standalone non-ROS test executable for direct SIL frame validation

---

## Package

The main package is:

- `src/sr32_led_cpp`

---

## Current Functionality

The ROS2 node subscribes to:

- `/rover_state` (`std_msgs/msg/String`)

Current state mapping:

| Rover State       | SIL Behavior        |
|------------------|---------------------|
| `teleop`         | blue solid          |
| `autonomy`       | green solid         |
| `goal_reached`   | green blinking      |
| `emergency_stop` | emergency/off frame |
| `off`            | normal off frame    |

---

## CAN / SIL Protocol Used

Current implementation uses:

- **29-bit extended CAN**
- Michael’s CAN layout:
  - bits 28:24 -> `deviceType` (5 bits)
  - bits 23:16 -> `manufacturer` (8 bits)
  - bits 15:14 -> `severity` (2 bits)
  - bits 13:6  -> `instruction` (8 bits)
  - bits 5:0   -> `deviceId` (6 bits)

Current SIL payload format follows Chloe’s firmware expectations:

- byte 0 -> Red
- byte 1 -> Green
- byte 2 -> Blue
- byte 3 -> Brightness
- byte 4 -> BlinkEnable
- byte 5 -> BlinkPeriod factor
- byte 6 -> reserved / 0
- byte 7 -> reserved / 0

### Current Working Constants

These are the current implementation values used in the package:

- `MANUFACTURER = 0x08`
- `DEVICE_ID = 0x0F`
- `SEV_NORMAL = 0x02`
- `SEV_EMERGENCY = 0x00`

The following are still placeholders and may be updated later if hardware testing requires it:

- `DEVICE_TYPE = 0x0E`
- `INST_SET_LED = 0x00`

---

## Architecture

The package is structured as follows:

```text
sr32_led_cpp/
├── CMakeLists.txt
├── package.xml
├── include/
│   └── sr32_led_cpp/
│       ├── led_controller.hpp
│       ├── led_states.hpp
│       └── sil_protocol.hpp
└── src/
    ├── led_controller.cpp
    ├── sr32_led_node.cpp
    └── test_sil_standalone.cpp
