# SR-32 Status Indicator Light Controller (ROS 2)

A ROS 2 Humble C++ package that maps rover state messages to CAN frames for the SR-32 Status Indicator Light (SIL) subsystem, developed as part of [Space Concordia Robotics](https://spaceconcordia.ca/).

> **Status:** Development complete. Pending hardware validation on Jetson with physical SIL unit.

---

## What This Does

The rover operates in several distinct states (teleoperation, autonomous navigation, emergency stop, etc.). Ground crew and operators need a clear, at-a-glance indication of which state the rover is currently in.

This package bridges the ROS 2 software layer to the hardware SIL unit via CAN bus:

1. A ROS 2 node subscribes to `/rover_state` topic messages
2. Each state is mapped to a specific LED behaviour (colour, brightness, blink pattern)
3. The node builds a correctly formatted 29-bit extended CAN frame
4. The frame is sent over SocketCAN to the SIL firmware

---

## State Mapping

| Rover State     | LED Behaviour         |
|-----------------|----------------------|
| `teleop`        | Blue, solid           |
| `autonomy`      | Green, solid          |
| `goal_reached`  | Green, blinking       |
| `emergency_stop`| Emergency/off frame   |
| `off`           | Normal off frame      |

---

## CAN / SIL Protocol

Uses **29-bit extended CAN frames** (SocketCAN `CAN_EFF_FLAG`).

### CAN ID Layout

```
bits 28:24  →  deviceType   (5 bits)
bits 23:16  →  manufacturer (8 bits)
bits 15:14  →  severity     (2 bits)
bits 13:6   →  instruction  (8 bits)
bits  5:0   →  deviceId     (6 bits)
```

### Current Constants

| Constant        | Value  | Status              |
|-----------------|--------|---------------------|
| `MANUFACTURER`  | `0x08` | Confirmed           |
| `DEVICE_ID`     | `0x0F` | Confirmed           |
| `SEV_NORMAL`    | `0x02` | Confirmed           |
| `SEV_EMERGENCY` | `0x00` | Confirmed           |
| `DEVICE_TYPE`   | `0x0E` | Placeholder         |
| `INST_SET_LED`  | `0x00` | Pending electrical  |

### SIL Payload Format (8 bytes)

```
byte 0  →  Red          (0–255)
byte 1  →  Green        (0–255)
byte 2  →  Blue         (0–255)
byte 3  →  Brightness   (0–255)
byte 4  →  BlinkEnable  (0 or 1)
byte 5  →  BlinkPeriod  (factor)
byte 6  →  Reserved / 0
byte 7  →  Reserved / 0
```

---

## Package Structure

```
src/sr32_led_cpp/
├── CMakeLists.txt
├── package.xml
├── include/
│   └── sr32_led_cpp/
│       ├── led_controller.hpp     # State-to-payload mapping logic
│       ├── led_states.hpp         # Rover state definitions
│       └── sil_protocol.hpp       # CAN ID builder and frame constants
└── src/
    ├── led_controller.cpp         # LED behaviour implementation
    ├── sr32_led_node.cpp          # ROS 2 node entry point
    └── test_sil_standalone.cpp    # Standalone CAN frame test (no ROS 2 required)
```

---

## System Architecture

```
/rover_state  (std_msgs/msg/String)
      │
      ▼
 sr32_led_node          ← ROS 2 subscriber node
      │
      ▼
 led_controller         ← Maps state → RGB/brightness/blink payload
      │
      ▼
 sil_protocol           ← Builds 29-bit CAN frame
      │
      ▼
 SocketCAN interface
      │
      ├── vcan0         ← Virtual interface for local testing
      └── can0          ← Physical interface (Jetson, 1 Mbps)
            │
            ▼
     SIL Firmware       ← SR-32 Status Indicator Light hardware
```

---

## Prerequisites

- Ubuntu 22.04
- ROS 2 Humble
- SocketCAN utilities: `sudo apt install can-utils`
- `colcon` build tool

---

## Setup and Run

### 1. Clone and build

```bash
git clone https://github.com/Arley100/ros2_ws.git
cd ros2_ws
colcon build --packages-select sr32_led_cpp
source install/setup.bash
```

### 2. Set up virtual CAN interface (for local testing)

```bash
sudo modprobe vcan
sudo ip link add dev vcan0 type vcan 2>/dev/null || true
sudo ip link set up vcan0
```

### 3. Run — three terminals

**Terminal 1 — Monitor CAN traffic:**
```bash
candump vcan0
```

**Terminal 2 — Launch the ROS 2 node:**
```bash
ros2 run sr32_led_cpp sr32_led_node --ros-args -p can_interface:=vcan0
```

**Terminal 3 — Publish rover state:**
```bash
# Try any of these:
ros2 topic pub /rover_state std_msgs/msg/String "{data: 'teleop'}" -1
ros2 topic pub /rover_state std_msgs/msg/String "{data: 'autonomy'}" -1
ros2 topic pub /rover_state std_msgs/msg/String "{data: 'emergency_stop'}" -1
```

You should see CAN frames appear in Terminal 1 as each state is published.

### 4. Standalone CAN frame test (no ROS 2 required)

```bash
./build/sr32_led_cpp/test_sil_standalone vcan0
```

This validates CAN frame construction and SocketCAN transmission independently of the ROS 2 stack.

### 5. Physical hardware (Jetson)

```bash
sudo ip link set can0 up type can bitrate 1000000
ros2 run sr32_led_cpp sr32_led_node --ros-args -p can_interface:=can0
```

---

## Design Decisions

**Why SocketCAN?**
SocketCAN is the standard Linux kernel CAN interface. It allows the same code to target a virtual interface (`vcan0`) for development and a physical interface (`can0`) for hardware deployment, with only a parameter change at runtime.

**Why a standalone test executable?**
The standalone test (`test_sil_standalone.cpp`) validates CAN frame construction and transmission without requiring a full ROS 2 environment. This was important for early integration testing before the rover's full ROS 2 stack was available.

**Why 29-bit extended frames?**
The Space Concordia rover CAN middleware (designed by the cross-team integration lead) uses 29-bit extended CAN IDs to encode device type, manufacturer, severity, instruction, and device ID in a structured bit layout. The SIL follows this convention for consistency across all rover subsystems.

---

## Known Limitations and Next Steps

- `DEVICE_TYPE` and `INST_SET_LED` are currently placeholder values pending confirmation from the electrical team's firmware
- Hardware validation on the physical Jetson + SIL unit is the next milestone
- Blink timing is currently implemented via a period factor; exact timing values will be confirmed during hardware testing

---

## Related

- [Space Concordia Robotics](https://spaceconcordia.ca/)
- Built in coordination with the cross-team CAN middleware developed by the software integration team
- SIL firmware developed by the electrical engineering subteam

---

## Author

**Arley** — Software Engineering Co-op Student, Concordia University  
Space Concordia Robotics — Software Subteam
