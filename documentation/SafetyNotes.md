
# Safety Notes

Follow these safety guidelines when assembling, wiring, and operating the Automated Mechanical Desk Lift system. Adhering to these practices helps prevent hardware damage and ensures user safety.

---

## General Safety Guidelines
- **Always disconnect power before wiring.** Check polarity before powering on.
- Motor power must **never** pass through the Arduino; use dedicated battery/PSU → motor driver path.
- The ON/OFF power switch must be placed **between the battery and the motor driver** (with fuse inline).
- Arduino GND and motor driver GND **must be common** (critical for reliable operation).
- Motor driver R_EN and L_EN pins must be **tied together** and driven identically for safety (prevents half-bridge conflicts).
- Rocker switch inputs (PIN 7, 8) must be debounced appropriately; see HAL for current implementation.

---


## Compliance
See [SoftwareDetailedDesign.md](SoftwareDetailedDesign.md) for full compliance and safety requirements details. If you add new hardware or features, update this file and ensure compliance with relevant safety standards.

---

*For questions or safety concerns, open an issue or consult the project maintainers.*

---

## v1.0 Design Limitations

**Current v1.0 implementation:**
- Direct rocker switch (ON/OFF/ON) to motor control mapping
- No end-of-travel limit switches (planned for v2.0)
- No automatic timeout enforcement (planned for v2.0, SWE-REQ-018)
- No emergency stop circuit (planned for v2.0, SWE-REQ-010/011)
- Full-speed motor operation (no ramping or variable PWM in v1.0)

**User responsibility:**
- Manually release the switch to stop motion
- Monitor desk height manually (no encoder feedback)
- Do not leave desk unattended while power is on

---

## Error Handling and Recovery (v1.0)

The v1.0 software detects and reports invalid input conditions:

- **NULL pointer detection:** If inputs or outputs pointers are NULL, `DeskApp_task()` returns `APP_TASK_ERROR` and does not drive the motor.
- **Motor disable on error:** Any error condition immediately disables motor output to prevent uncontrolled motion.
- **Recovery procedure:** To recover from a NULL pointer error:
  1. Verify that inputs and outputs structures are properly allocated and passed to `DeskApp_task()`.
  2. Call `DeskApp_task_init()` to reinitialize the application to a safe IDLE state.
  3. Confirm normal behavior by sending valid switch commands and observing motor response.

**Future v2.0 features** (planned):
- Limit switch detection (top and bottom end-of-travel)
- Invalid state detection (conflicting limit switches)
- Timeout enforcement and automatic stop after 30 seconds
- Emergency stop input validation (SWE-REQ-010, 011)

## Startup and Safe Initialization

- The HAL provides `HAL_wait_startup()` which performs an initial settling period while processing HAL timed events (LED blink manager). This ensures hardware indicators are ready before motion commands are issued.
- On system startup, `DeskApp_task_init()` initializes all outputs to safe defaults: motor disabled, direction cleared, PWM set to 0.
- The application starts in IDLE state; motor remains off until a valid switch command is received.
- See [PinConfig.h](../source/arduino/PinConfig.h) for pin assignments used in hardware initialization.

## Testing and Validation

- See [TESTING_README.md](TESTING_README.md) for instructions to run unit and integration tests.
- Safety behaviors are verified by integration tests:
  - `IT-004`, `IT-005`: Motor response to commands
  - `IT-008`: Error detection (NULL inputs/outputs)
  - `IT-011`: Startup settling time
  - `SIT-005`: Safety interlock (conflicting inputs during direction reversal)
- Unit tests validate state handler error paths and timeout logic (planned for v2.0 full implementation).
- For any safety incidents, document in `documentation/reviews/Safety_Review_YYYYMMDD.md` and include the commit/tag used.
