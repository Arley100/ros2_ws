# SR32 SIL LED Controller References and Diagrams

This file gives a visual overview of how the `sr32_led_cpp` package works.

The goal is to help future readers understand the package from a different perspective than the code alone.

---

## 1. Package Overview

```mermaid
flowchart TD
    A[ROS 2 Topic: /rover_state] --> B[sr32_led_node]
    B --> C[State Decision Logic]
    C --> D[LED Command Builder]
    D --> E[CAN Frame Builder]
    E --> F[SocketCAN Interface]
    F --> G[SIL Firmware]
    G --> H[Physical Status Indicator Light]
```

This diagram shows the main flow of the package. The rover publishes a state, the LED node receives it, the code decides the correct LED behavior, then a CAN frame is sent to the SIL firmware.

---

## 2. Local Testing Architecture

```mermaid
flowchart LR
    A["ros2 topic pub"] --> B["/rover_state"]
    B --> C["sr32_led_node"]
    C --> D["vcan0"]
    D --> E["candump vcan0"]
```

During local testing, the package does not need physical CAN hardware. Instead, it sends frames to `vcan0`, and `candump vcan0` is used to check whether the frames are being produced correctly.

---

## 3. Runtime Sequence Diagram

```mermaid
sequenceDiagram
    participant Tester
    participant Topic as ROS 2 Topic /rover_state
    participant Node as sr32_led_node
    participant CAN as SocketCAN Interface
    participant Dump as candump vcan0

    Tester->>Topic: Publish rover state string
    Topic->>Node: Deliver std_msgs/msg/String
    Node->>Node: Match state to LED behavior
    Node->>Node: Build CAN ID and payload
    Node->>CAN: Send extended CAN frame
    CAN->>Dump: Frame appears on vcan0
```

This sequence diagram shows what happens when a test state is published from the terminal.

---

## 4. State-to-LED Logic

```mermaid
flowchart TD
    A[Receive rover state] --> B{Which state?}

    B -->|teleop| C[Set teleop LED behavior]
    B -->|autonomy| D[Set autonomy LED behavior]
    B -->|goal_reached| E[Set goal reached LED behavior]
    B -->|emergency_stop| F[Set emergency stop LED behavior]
    B -->|unknown| G[Ignore or use default behavior]

    C --> H[Build SIL payload]
    D --> H
    E --> H
    F --> H
    G --> H

    H --> I[Send CAN frame]
```

This diagram explains the decision-making logic. Each rover state is mapped to a LED behavior before the CAN frame is sent.

---

## 5. CAN Payload Layout

```mermaid
flowchart LR
    B0[Byte 0: Red] --> B1[Byte 1: Green]
    B1 --> B2[Byte 2: Blue]
    B2 --> B3[Byte 3: Brightness]
    B3 --> B4[Byte 4: BlinkEnable]
    B4 --> B5[Byte 5: BlinkPeriod]
    B5 --> B6[Byte 6: Unused]
    B6 --> B7[Byte 7: Unused]
```

Current expected payload format:

```text
[R, G, B, Brightness, BlinkEnable, BlinkPeriod, 0, 0]
```

The SIL firmware currently interprets `BlinkPeriod` as:

```text
BlinkPeriod byte * 10 ms
```

Example:

```text
BlinkPeriod = 50
50 * 10 ms = 500 ms
```

---

## 6. Responsibility Diagram

This is not necessarily an exact C++ class diagram. It is a responsibility diagram that explains how the main parts of the package relate to each other.

```mermaid
classDiagram
    class Sr32LedNode {
        +subscribe_to_rover_state()
        +handle_rover_state()
        +send_led_command()
    }

    class LedController {
        +set_state()
        +build_command()
        +send_command()
    }

    class LedStates {
        +teleop
        +autonomy
        +goal_reached
        +emergency_stop
    }

    class SilProtocol {
        +build_can_id()
        +build_payload()
        +create_frame()
    }

    class SocketCAN {
        +open_interface()
        +send_frame()
    }

    Sr32LedNode --> LedController
    LedController --> LedStates
    LedController --> SilProtocol
    SilProtocol --> SocketCAN
```

This diagram gives a higher-level view of the package responsibilities:

- `sr32_led_node` handles the ROS 2 node behavior.
- `led_controller` handles the LED decision logic.
- `led_states` stores or represents the possible rover states.
- `sil_protocol` handles SIL-specific CAN details.
- SocketCAN sends the frame to `vcan0` or a real CAN interface.

---

## 7. Testing Checklist

```mermaid
flowchart TD
    A[Start] --> B[Build package]
    B --> C[Source workspace]
    C --> D[Create vcan0]
    D --> E[Run sr32_led_node]
    E --> F[Run candump vcan0]
    F --> G[Publish test rover state]
    G --> H{Frame appears?}
    H -->|Yes| I[Local test passed]
    H -->|No| J[Debug node, topic, CAN interface, or state string]
```

This checklist gives a quick visual summary of the local test process.

---

## 8. Future Hardware Testing Checklist

```mermaid
flowchart TD
    A[Connect SIL hardware] --> B[Bring up real CAN interface]
    B --> C[Run LED node with can0]
    C --> D[Publish rover state]
    D --> E{SIL changes correctly?}
    E -->|Yes| F[Confirm behavior and document result]
    E -->|No| G[Check CAN ID, payload, wiring, bitrate, firmware expectations]
```

This section should be updated after real SIL testing.

---

## 9. Important Commands

### Set up virtual CAN

```bash
sudo modprobe vcan
sudo ip link add dev vcan0 type vcan 2>/dev/null || true
sudo ip link set up vcan0
```

### Monitor CAN frames

```bash
candump vcan0
```

### Run the LED node

```bash
ros2 run sr32_led_cpp sr32_led_node --ros-args -p can_interface:=vcan0
```

### Publish a test rover state

```bash
ros2 topic pub /rover_state std_msgs/msg/String "{data: 'teleop'}" -1
```



- Confirmed brightness behavior
- Confirmed blink behavior
- Any bugs discovered during hardware testing
