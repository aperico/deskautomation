
# Software Requirements

These requirements are derived from the System Use Cases and follow INCOSE guidelines for clarity, testability, and traceability.

---

## Assumptions and Constraints
- The system hardware (ECU, motor driver, indicator LEDs, limit switches, buttons) is available and functions as specified.
- The user is trained to operate the desk safely.
- The system operates in a typical office environment with standard power supply.
- Regulatory and safety standards for motorized furniture are met.

---

## Requirements Classification
- Functional Requirements: SR-01 to SR-08
- Safety Requirements: SR-04, SR-08
- Interface Requirements: SR-05

---

## Requirements

### SR-01: System Initialization (Functional)
**ID:** SR-01
**Source:** System Use Case UC-01
**Assumptions:** System hardware is powered and functional.
**The software shall initialize all input and output variables to their default states upon power-on.**
**The software shall set the system state to IDLE after initialization.**
**The software shall ensure that all movement outputs are inactive during initialization.**
**The software shall activate the ready status indicator when initialization is complete.**

### SR-02: Upward Movement Command (Functional)
**ID:** SR-02
**Source:** System Use Case UC-02
**Assumptions:** Up button and upper limit switch are functional.
**The software shall detect when the Up button is pressed by the user.**
**The software shall command upward movement only if the upper limit is not active and the Down button is not pressed.**
**The software shall activate the upward movement output when commanded.**
**The software shall activate the Up indicator LED during upward movement.**
**The software shall stop upward movement when the Up button is released, a timeout occurs, or the upper limit is reached.**
**The software shall return the indicator LED to the IDLE state after movement stops.**

### SR-03: Downward Movement Command (Functional)
**ID:** SR-03
**Source:** System Use Case UC-03
**Assumptions:** Down button and lower limit switch are functional.
**The software shall detect when the Down button is pressed by the user.**
**The software shall command downward movement only if the lower limit is not active and the Up button is not pressed.**
**The software shall activate the downward movement output when commanded.**
**The software shall activate the Down indicator LED during downward movement.**
**The software shall stop downward movement when the Down button is released, a timeout occurs, or the lower limit is reached.**
**The software shall return the indicator LED to the IDLE state after movement stops.**

### SR-04: Emergency Stop (Safety)
**ID:** SR-04
**Source:** System Use Case UC-04
**Assumptions:** Fault detection logic is implemented and reliable.
**The software shall detect when both Up and Down buttons are pressed simultaneously or when a fault/timeout occurs.**
**The software shall immediately deactivate all movement outputs upon detection of an emergency stop condition.**
**The software shall set the system state to ERROR when an emergency stop is triggered.**
**The software shall activate the error indicator LED during an error state.**
**The software shall allow the user to clear the error state by cycling power.**

### SR-05: Visual Feedback (Interface)
**ID:** SR-05
**Source:** System Use Case UC-05
**Assumptions:** Indicator LEDs are functional and visible to the user.
**The software shall update output LEDs to reflect the current system state:**
    - **IDLE:** Ready/neutral LED
    - **MOVING_UP:** Up LED
    - **MOVING_DOWN:** Down LED
    - **ERROR:** Error LED

### SR-06: Power-Off Handling (Functional)
**ID:** SR-06
**Source:** System Use Case UC-06
**Assumptions:** Power supply is stable and system can detect power loss.
**The software shall immediately deactivate all movement outputs when system power is lost during movement.**
**The software shall reinitialize the system to the IDLE state upon power restoration.**

### SR-07: Simultaneous Button Presses (Functional)
**ID:** SR-07
**Source:** System Use Case UC-07
**Assumptions:** Button press detection logic is reliable.
**The software shall detect when both Up and Down buttons are pressed simultaneously.**
**The software shall not command any movement when both buttons are pressed.**
**The software shall stop desk movement if it is in progress when both buttons are pressed.**
**The software shall resume normal operation when both buttons are released.**

### SR-08: Error Detection and Recovery (Safety)
**ID:** SR-08
**Source:** System Use Case UC-08
**Assumptions:** Error detection logic is implemented and reliable.
**The software shall detect error conditions, including both limits being active or overcurrent events.**
**The software shall set the system state to ERROR upon detection of an error condition.**
**The software shall activate the error indicator LED during an error state.**
**The software shall allow the user to attempt recovery by cycling power.**

---

## Traceability
Each requirement is traceable to one or more System Use Cases as documented in SystemUseCases.md.

---

## Quality Attributes
- All requirements are stated in clear, unambiguous language.
- Each requirement is testable and verifiable.
- Requirements are atomic and do not combine multiple behaviors.
- Requirements are implementation-independent and focus on externally observable behavior.

---

## Notes
- Requirements are subject to change based on future enhancements or hardware modifications.
- All requirements comply with INCOSE best practices for systems engineering.
