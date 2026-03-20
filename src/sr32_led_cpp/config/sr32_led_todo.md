# SR-32 CAN Integration TODO

Pending data required before final CAN integration:

1. Exact LED CAN ID
2. Standard or extended frame format
3. Payload format:
   - enum byte
   - bitfield
   - RGB bytes
4. Event-based or periodic refresh
5. Timeout / fallback behavior
6. Any acknowledgement or status frame

Required mapping:
- Autonomy -> Red
- Teleop -> Blue
- GPS Success -> Green
