# ROS 2 Workspace
# SR-32 LED Controller (ROS2)

## Overview
This package implements LED state control for the rover based on system state messages received through ROS2.

The system listens to rover state updates and determines the corresponding LED color, preparing commands to be sent over CAN.

---

## Functionality

The node subscribes to:

/rover/state  (std_msgs/msg/String)

It maps system states to LED colors as follows:

| Rover State  | LED Color |
|-------------|----------|
| autonomy     | RED      |
| teleop       | BLUE     |
| gps success  | GREEN    |

---

## Architecture

ROS2 Node → State Interpretation → LED Controller → CAN Interface

- **ROS2 Node**: subscribes to rover state topic
- **State Interpretation**: determines LED state
- **LED Controller**: prepares CAN command
- **CAN Interface**: currently a placeholder (pending integration)

---

## Current Status

- ROS2 node implementation: ✅ complete  
- State-to-LED mapping: ✅ complete  
- Logging and validation: ✅ complete  
- CAN integration: ⏳ pending electrical specifications  

---

## CAN Integration (Pending)

The following information is required from the electrical subsystem before final integration:

- CAN ID for LED controller  
- Frame type (standard or extended)  
- Payload format (byte structure for LED colors)  
- Data length (DLC)  
- Transmission behavior (event-based or periodic)  

---

## Known Limitation

⚠️ CAN communication is currently implemented as a placeholder.

The system logs intended CAN messages but does not yet send real CAN frames.

---

## Example Usage

Run the node:

```bash
ros2 run sr32_led_cpp sr32_led_cpp_node
