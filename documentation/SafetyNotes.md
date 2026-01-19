
# Safety Notes

Follow these safety guidelines when assembling, wiring, and operating the Automated Mechanical Desk Lift system. Adhering to these practices helps prevent hardware damage and ensures user safety.

---

## General Safety Guidelines
- **Always disconnect power before wiring.**
- Motor power must **never** pass through the Arduino.
- The ON/OFF switch must be placed **between the battery and the motor driver**.
- Arduino GND and motor driver GND **must be common**.

---


## Compliance
See [Software Detailed Design](09_SoftwareDetailedDesign.md#compliance-statement) for full compliance details. If you add new hardware or features, update this file and ensure compliance with relevant safety standards.

---

*For questions or safety concerns, open an issue or consult the project maintainers.*

---

## Error Handling and Recovery

The software detects and reports critical hardware faults (for example, both upper and lower limit switches active simultaneously). The following rules apply:

- **Fatal error detection:** When the application detects an invalid sensor combination (both limit switches active) it immediately sets the application error flag and asserts the error indicator. The motor is stopped and further motion commands are inhibited.
- **Recovery procedure:** To recover from a fatal error:
	1. Verify and restore correct hardware wiring and sensor states (clear the invalid condition).
	2. Power-cycle the device or call the application initialization sequence (on the host this is `DeskApp_task_init()`), which returns the application to IDLE when the hardware conditions are safe.
	3. Confirm normal behavior with a functional test (see `documentation/TESTING_README.md` and run integration tests such as `IT-009`).

Documenting recovery steps and recording the review/approval of the recovery procedure strengthens ASPICE evidence for safety and error handling.

## Startup and Safe Initialization

- The HAL provides `HAL_wait_startup()` which performs a short startup wait while processing HAL timed events (blink manager). This ensures hardware and human-facing indicators settle before any motion commands are issued.
- On device startup the software initializes outputs to safe defaults (motor stopped, LEDs off) in `DeskApp_task_init()`.

## Testing and Evidence

- See `documentation/TESTING_README.md` for instructions to run unit and integration tests. Integration tests verify safety behaviors (e.g., `IT-004`, `IT-009`, `IT-008`).
- Record the actions taken during any safety incident in `documentation/reviews/` (create `Safety_Review_YYYYMMDD.md`) and include the commit/tag used as the baseline for the corrective action.
