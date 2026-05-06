# SR32 SIL LED Controller Documentation

## 1. Purpose of the Package

The `sr32_led_cpp` package is a ROS 2 C++ package responsible for controlling the rover Status Indicator Light, also called the SIL.

The goal of this package is to listen to the rover state and translate that state into LED behavior. For example, different rover states can produce different LED colors or blink patterns.

Example rover states currently handled:

- `teleop`
- `autonomy`
- `goal_reached`
- `emergency_stop`

The package sends commands over SocketCAN so the SIL firmware can receive the LED command through the CAN bus.

---

## 2. High-Level Behavior

The package works in this order:

1. A ROS 2 node starts.
2. The node subscribes to a rover state topic.
3. When a rover state message is received, the callback function checks the state string.
4. The state is mapped to a LED color and blink configuration.
5. The package builds a CAN frame.
6. The CAN frame is sent through the selected CAN interface, such as `vcan0` for local testing or a real CAN interface on hardware.

In simple terms:

```text
ROS 2 rover state message
        ↓
LED controller callback
        ↓
State-to-color decision logic
        ↓
CAN frame construction
        ↓
SocketCAN interface
        ↓
SIL firmware
