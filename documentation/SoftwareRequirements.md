


# Software Requirements (DeskHigh torque branch: BTS7960/IBT-2, 31ZY-5840)

**Document Version:** 1.2  
**Last Updated:** January 7, 2026  
**Status:** Approved  
**Author:** Development Team  
**Reviewers:** System Engineering, Safety Team  
**Compliance:** IEEE 29148-2018, INCOSE, ISO 26262

---

## Overview

This document specifies the software requirements for the Automated Mechanical Desk Lift system. This version is for the DeskHigh torque branch, using the BTS7960/IBT-2 motor driver and 31ZY-5840 DC Worm Gear Motor. All requirements are:
- **Derived from** [System Use Cases](SystemUseCases.md)
- **Traceable** to use cases and test cases
- **Testable** with defined acceptance criteria
- **Unambiguous** and implementation-independent
- **Compliant** with INCOSE and IEEE standards for requirements engineering

---


## Navigation
- [System Use Cases](SystemUseCases.md)
- [Traceability Matrix](TraceabilityMatrix.md)
- [Software Test Cases Specification](SoftwareTestCasesSpecification.md)

---

## Requirements Summary

| Req ID | Name | Category | Priority | Derived From | Verification Method | Status |
|--------|------|----------|----------|--------------|---------------------|--------|
| SWE-REQ-001 | System Initialization | Functional | Mandatory | UC-01 | Test | Approved |
| SWE-REQ-002 | Ready State Indication | Functional | Mandatory | UC-01 | Test | Approved |
| SWE-REQ-003 | Upward Movement Detection | Functional | Mandatory | UC-02 | Test | Approved |
| SWE-REQ-004 | Downward Movement Detection | Functional | Mandatory | UC-03 | Test | Approved |
| SWE-REQ-005 | Upward Movement Execution | Functional | Mandatory | UC-02 | Test | Approved |
| SWE-REQ-006 | Downward Movement Execution | Functional | Mandatory | UC-03 | Test | Approved |
| SWE-REQ-007 | Upward Movement Termination | Functional | Mandatory | UC-02 | Test | Approved |
| SWE-REQ-008 | Downward Movement Termination | Functional | Mandatory | UC-03 | Test | Approved |
| SWE-REQ-009 | Power Loss Handling | Functional | Mandatory | UC-06 | Test | Approved |
| SWE-REQ-010 | Emergency Stop Detection | Safety | Mandatory | UC-04, UC-07 | Test | Approved |
| SWE-REQ-011 | Emergency Stop Execution | Safety | Mandatory | UC-04 | Test | Approved |
| SWE-REQ-012 | LED State Indication | Interface | Mandatory | UC-05 | Test | Approved |
| SWE-REQ-013 | LED Update Timing | Performance | Mandatory | UC-05 | Test | Approved |
| SWE-REQ-014 | Conflicting Input Handling | Safety | Mandatory | UC-07 | Test | Approved |
| SWE-REQ-015 | Error Detection | Safety | Mandatory | UC-08 | Test | Approved |
| SWE-REQ-016 | Error Recovery | Functional | Mandatory | UC-08 | Test | Approved |
| SWE-REQ-017 | Button Debouncing | Functional | Mandatory | UC-02, UC-03 | Test | Approved |
| SWE-REQ-018 | Movement Timeout | Safety | Mandatory | UC-02, UC-03 | Test | Approved |
| SWE-REQ-019 | Emergency Stop Response Time | Performance | Mandatory | UC-04 | Test | Approved |
| SWE-REQ-020 | State Transition Integrity | Functional | Mandatory | Derived | Test | Approved |
| SWE-REQ-022 | PWM Soft-Start for Motor | Functional | Mandatory | UC-10 | Test | Proposed |
### SWE-REQ-022: PWM Soft-Start for Motor

**Category:** Functional  
**Priority:** Mandatory  
**Derived From:** UC-10 (PWM Soft-Start for Motor)

**Description:**

The software shall apply a PWM soft-start ramp when initiating motor movement (up or down) to gradually increase power and reduce mechanical shock to the worm gear motor.

**Specific Requirements:**
- When a movement command is issued, the software shall set the PWM output to a low initial value
- The software shall ramp the PWM value to the configured target speed over a configurable time (e.g., 200–500ms)
- The ramp shall be applied for both upward and downward movement
- If movement is stopped during ramp, PWM is immediately set to zero
- If ramp fails to reach target PWM (hardware fault), system enters ERROR state

**Rationale:**
Soft-start reduces mechanical stress, increases motor and gearbox longevity, and improves user safety.

**Acceptance Criteria:**
- Motor accelerates smoothly to target speed on every movement
- No abrupt starts or mechanical shock
- Ramp duration and initial/final PWM values are configurable
- System enters ERROR state if ramp fails

**Dependencies:**
- SWE-REQ-005, SWE-REQ-006 (Movement Execution)

**Verification Method:** Test (integration tests with timing and ramp monitoring)

**Test Cases:** IT-009 (if implemented)

---

## Assumptions and Constraints
- The system hardware (ECU, BTS7960/IBT-2 motor driver, 31ZY-5840 DC Worm Gear Motor, indicator LEDs, limit switches, buttons) is available and functions as specified.
- The user is trained to operate the desk safely.
- The system operates in a typical office environment with standard power supply.
- Regulatory and safety standards for motorized furniture are met.

---

## Requirements Classification

### By Category
- **Functional Requirements:** SWE-REQ-001 to SWE-REQ-009, SWE-REQ-016, SWE-REQ-017, SWE-REQ-020
- **Safety Requirements:** SWE-REQ-010, SWE-REQ-011, SWE-REQ-014, SWE-REQ-015, SWE-REQ-018, SWE-REQ-019
- **Interface Requirements:** SWE-REQ-012
- **Performance Requirements:** SWE-REQ-013, SWE-REQ-019

### By Priority
- **Mandatory:** All requirements (20/20)

### By Verification Method
- **Test:** All requirements verified through automated testing

---

## Detailed Requirements

Each requirement follows the standard format:
- **ID, Name, Category, Priority**
- **Derived From** (use case traceability)
- **Description** (what the system shall do)
- **Rationale** (why this requirement exists)
- **Acceptance Criteria** (measurable verification)
- **Dependencies** (related requirements)
- **Verification Method** (how to verify)
- **Test Cases** (which tests verify this)

---


### SWE-REQ-001: System Initialization (BTS7960/IBT-2, 31ZY-5840)

**Category:** Functional  
**Priority:** Mandatory  
**Derived From:** UC-01 (Power-On the Desk Control System)

**Description:**

The software shall initialize all input and output variables to their default states upon power-on.

**Specific Requirements:**
- The software shall initialize all input pins (buttons, limit switches, current sense A0/A1 if used) to INPUT mode
- The software shall initialize all output pins (motor control: RPWM, LPWM, R_EN, L_EN; LEDs) to OUTPUT mode
- The software shall set all motor control outputs (RPWM, LPWM, R_EN, L_EN) to inactive (motor stopped, both PWM LOW, enables LOW or HIGH as required)
- The software shall set the system state to IDLE
- The software shall perform initialization within 500ms of power application

**Rationale:**

Proper initialization ensures the system starts in a known, safe state with all hardware interfaces correctly configured, preventing undefined behavior or unintended motor activation.

**Acceptance Criteria:**
- All variables initialized to documented default values
- System reaches IDLE state within 500ms
- No motor movement occurs during initialization
- All pin modes correctly configured
- Initialization sequence completes successfully 100% of test runs

- Hardware: Arduino power supply, BTS7960/IBT-2 wiring, pin connections
- None (this is the first requirement executed)

**Verification Method:** Test (automated unit tests)

**Test Cases:** TC-001, TC-002, IT-001

---

### SWE-REQ-002: Ready State Indication

**Category:** Functional  
**Priority:** Mandatory  
**Derived From:** UC-01 (Power-On the Desk Control System)

**Description:**

The software shall activate the ready indicator LED when initialization is complete and the system enters IDLE state.

**Rationale:**

Provides visual confirmation to the user that the system is powered, initialized, and ready to accept commands.

**Acceptance Criteria:**
- Ready LED (green) illuminates when system enters IDLE state
- LED activation occurs within 50ms of entering IDLE state
- LED remains ON continuously while in IDLE state
- LED turns OFF when transitioning out of IDLE state

**Dependencies:**
- SWE-REQ-001 (System Initialization)
- SWE-REQ-012 (LED State Indication)

**Verification Method:** Test (automated integration tests)

**Test Cases:** TC-001, TC-002, IT-001

---

### SWE-REQ-003: Upward Movement Detection (Rocker Switch)

**Category:** Functional  
**Priority:** Mandatory  
**Derived From:** UC-02 (Raise Desk)

**Description:**

The software shall detect when the ON/OFF/ON rocker switch is set to the UP position by the user.

**Rationale:**

Accurate switch state detection is essential for user-initiated movement commands. The ON/OFF/ON rocker switch provides a clear, debounced hardware interface.

**Acceptance Criteria:**
- UP switch position detected within 50ms of physical actuation
- Switch state accurately reflects physical switch position
- Detection works reliably across 10,000+ switch cycles

**Dependencies:**
- SWE-REQ-001 (System Initialization)

**Verification Method:** Test (unit and integration tests)

**Test Cases:** TC-003, IT-002

---

### SWE-REQ-004: Downward Movement Detection (Rocker Switch)

**Category:** Functional  
**Priority:** Mandatory  
**Derived From:** UC-03 (Lower Desk)

**Description:**

The software shall detect when the ON/OFF/ON rocker switch is set to the DOWN position by the user.

**Rationale:**

Accurate switch state detection is essential for user-initiated movement commands. The ON/OFF/ON rocker switch provides a clear, debounced hardware interface.

**Acceptance Criteria:**
- DOWN switch position detected within 50ms of physical actuation
- Switch state accurately reflects physical switch position
- Detection works reliably across 10,000+ switch cycles

**Dependencies:**
- SWE-REQ-001 (System Initialization)

**Verification Method:** Test (unit and integration tests)

**Test Cases:** TC-005, IT-003

---

### SWE-REQ-005: Upward Movement Execution (BTS7960/IBT-2)

**Category:** Functional  
**Priority:** Mandatory  
**Derived From:** UC-02 (Raise Desk)

**Description:**

The software shall command upward movement only if all safety conditions are met:
- System is in IDLE state
- Up button is pressed
- Upper limit is not active (or not implemented)
- Down button is not pressed simultaneously

**Specific Requirements:**
- The software shall transition from IDLE to MOVING_UP state
- The software shall set BTS7960 RPWM to PWM value, LPWM to LOW, enables HIGH (or as required)
- The software shall activate the Up indicator LED (blue)
- The software shall monitor for stop conditions continuously

**Rationale:**

Safety interlocks prevent movement in unsafe conditions (conflicting inputs, limit reached). Proper state management ensures predictable behavior.

**Acceptance Criteria:**
- Movement initiates within 50ms of button press (when conditions met)
- No movement occurs if any safety condition is violated
- State transitions correctly (IDLE → MOVING_UP)
- Motor direction set correctly (verified via HAL)
- Up LED activates simultaneously with motor activation

**Dependencies:**
- SWE-REQ-003 (Upward Movement Detection)
- SWE-REQ-014 (Conflicting Input Handling)
- SWE-REQ-020 (State Transition Integrity)

**Verification Method:** Test (integration tests with HAL mock)

**Test Cases:** TC-003, TC-004, TC-009, IT-002

---

### SWE-REQ-006: Downward Movement Execution (BTS7960/IBT-2)

**Category:** Functional  
**Priority:** Mandatory  
**Derived From:** UC-03 (Lower Desk)

**Description:**

The software shall command downward movement only if all safety conditions are met:
- System is in IDLE state
- Down button is pressed
- Lower limit is not active (or not implemented)
- Up button is not pressed simultaneously

**Specific Requirements:**
- The software shall transition from IDLE to MOVING_DOWN state
- The software shall set BTS7960 LPWM to PWM value, RPWM to LOW, enables HIGH (or as required)
- The software shall activate the Down indicator LED (yellow)
- The software shall monitor for stop conditions continuously

**Rationale:**

Safety interlocks prevent movement in unsafe conditions (conflicting inputs, limit reached). Proper state management ensures predictable behavior.

**Acceptance Criteria:**
- Movement initiates within 50ms of button press (when conditions met)
- No movement occurs if any safety condition is violated
- State transitions correctly (IDLE → MOVING_DOWN)
- Motor direction set correctly (verified via HAL)
- Down LED activates simultaneously with motor activation

**Dependencies:**
- SWE-REQ-004 (Downward Movement Detection)
- SWE-REQ-014 (Conflicting Input Handling)
- SWE-REQ-020 (State Transition Integrity)

**Verification Method:** Test (integration tests with HAL mock)

**Test Cases:** TC-005, TC-006, TC-010, IT-003

---

### SWE-REQ-007: Upward Movement Termination (BTS7960/IBT-2)

**Category:** Functional  
**Priority:** Mandatory  
**Derived From:** UC-02 (Raise Desk)

**Description:**

The software shall stop upward movement when any of the following conditions occur:
- Up button is released
- Movement timeout expires (30 seconds)
- Upper limit switch is activated (when implemented)
- Emergency stop condition is detected

**Specific Requirements:**
- The software shall set both RPWM and LPWM to LOW (motor stop), disables as required
- The software shall transition from MOVING_UP to IDLE state (or ERROR if applicable)
- The software shall deactivate the Up LED
- The software shall activate the ready LED (if returning to IDLE)

**Rationale:**

Timely and reliable movement termination is critical for safety and user control. Multiple stop conditions provide defense in depth.

**Acceptance Criteria:**
- Motor stops within 50ms of button release
- Motor stops within 100ms of timeout expiration
- Motor stops within 50ms of limit switch activation
- State transitions correctly (MOVING_UP → IDLE or ERROR)
- LED updates reflect new state within 50ms

**Dependencies:**
- SWE-REQ-005 (Upward Movement Execution)
- SWE-REQ-018 (Movement Timeout)
- SWE-REQ-020 (State Transition Integrity)

**Verification Method:** Test (integration tests)

**Test Cases:** TC-003, TC-004, TC-009, IT-002

---

### SWE-REQ-008: Downward Movement Termination (BTS7960/IBT-2)

**Category:** Functional  
**Priority:** Mandatory  
**Derived From:** UC-03 (Lower Desk)

**Description:**

The software shall stop downward movement when any of the following conditions occur:
- Down button is released
- Movement timeout expires (30 seconds)
- Lower limit switch is activated (when implemented)
- Emergency stop condition is detected

**Specific Requirements:**
- The software shall set both RPWM and LPWM to LOW (motor stop), disables as required
- The software shall transition from MOVING_DOWN to IDLE state (or ERROR if applicable)
- The software shall deactivate the Down LED
- The software shall activate the ready LED (if returning to IDLE)

**Rationale:**

Timely and reliable movement termination is critical for safety and user control. Multiple stop conditions provide defense in depth.

**Acceptance Criteria:**
- Motor stops within 50ms of button release
- Motor stops within 100ms of timeout expiration
- Motor stops within 50ms of limit switch activation
- State transitions correctly (MOVING_DOWN → IDLE or ERROR)
- LED updates reflect new state within 50ms

**Dependencies:**
- SWE-REQ-006 (Downward Movement Execution)
- SWE-REQ-018 (Movement Timeout)
- SWE-REQ-020 (State Transition Integrity)

**Verification Method:** Test (integration tests)

**Test Cases:** TC-005, TC-006, TC-010, IT-003

---

### SWE-REQ-009: Power Loss Handling

**Category:** Functional  
**Priority:** Mandatory  
**Derived From:** UC-06 (Power-Off During Movement)

**Description:**

The software shall handle power loss gracefully:
- No software action required during power loss (hardware inherently de-energizes motor)
- Upon power restoration, the software shall reinitialize to IDLE state (stateless reset)
- The software shall not retain any memory of pre-power-loss state

**Rationale:**

Stateless reset ensures predictable behavior after power restoration. The system does not need to recover previous state, simplifying design and improving safety.

**Acceptance Criteria:**
- After power cycle, system always initializes to IDLE state
- No residual state from before power loss
- System ready to accept commands immediately after initialization
- Behavior is identical to cold boot

**Dependencies:**
- SWE-REQ-001 (System Initialization)

**Verification Method:** Test (integration tests with power cycle simulation)

**Test Cases:** TC-013, IT-006

---

### SWE-REQ-010: Emergency Stop Detection

**Category:** Safety  
**Priority:** Mandatory  
**Derived From:** UC-04 (Emergency Stop)

**Description:**

The software shall detect emergency stop conditions:
- System fault detected (timeout, invalid state, hardware fault)

**Rationale:**

Rapid detection of emergency conditions is critical for safety. The ON/OFF/ON rocker switch interface eliminates the risk of conflicting button presses, so emergency stop is triggered only by system faults.

**Acceptance Criteria:**
- System fault conditions detected within one control loop cycle (typically < 50ms)
- Detection occurs regardless of current system state
- False positives minimized (< 0.01% rate)

**Dependencies:**
- None

**Verification Method:** Test (unit and integration tests)

**Test Cases:** TC-011, TC-012, IT-004

---

### SWE-REQ-011: Emergency Stop Execution

**Category:** Safety  
**Priority:** Mandatory  
**Derived From:** UC-04 (Emergency Stop)

**Description:**

The software shall immediately execute emergency stop upon detection:
- Call HAL_setMotorDirection(STOP) to de-energize motor
- Transition to ERROR state
- Activate error indicator LED (red)
- Deactivate all other indicator LEDs
- Ignore all button inputs until power cycle

**Rationale:**

Immediate response to emergency conditions minimizes risk of injury or equipment damage. System lockout prevents further operation until manual reset.

**Acceptance Criteria:**
- Motor de-energized within 50ms of emergency detection
- State transition to ERROR occurs within same control cycle
- Error LED activates within 50ms
- System remains locked in ERROR state (verified via attempted button presses)
- Only power cycle can clear ERROR state

**Dependencies:**
- SWE-REQ-010 (Emergency Stop Detection)
- SWE-REQ-019 (Emergency Stop Response Time)

**Verification Method:** Test (integration tests with timing verification)

**Test Cases:** TC-011, TC-012, TC-015, IT-004

---

### SWE-REQ-012: LED State Indication

**Category:** Interface  
**Priority:** Mandatory  
**Derived From:** UC-05 (Visual Feedback)

**Description:**

The software shall update indicator LEDs to reflect current system state:
- **IDLE state:** Ready LED ON (green), all others OFF
- **MOVING_UP state:** Up LED ON (blue), all others OFF
- **MOVING_DOWN state:** Down LED ON (yellow), all others OFF
- **ERROR state:** Error LED ON (red), all others OFF

**Rationale:**

Clear visual feedback allows users to understand system state at a glance, improving safety and usability.

**Acceptance Criteria:**
- Only one LED active at any time (mutual exclusion)
- LED state accurately reflects system state 100% of the time
- LED changes occur synchronously with state transitions
- LED colors match specification (verified by visual inspection)

**Dependencies:**
- SWE-REQ-020 (State Transition Integrity)
- SWE-REQ-013 (LED Update Timing)

**Verification Method:** Test (integration tests)

**Test Cases:** TC-007, TC-008, IT-005

---

### SWE-REQ-013: LED Update Timing

**Category:** Performance  
**Priority:** Mandatory  
**Derived From:** UC-05 (Visual Feedback)

**Description:**

The software shall update LED indicators within 50ms of any state change.

**Rationale:**

Timely visual feedback ensures users perceive the system as responsive and can react appropriately to state changes.

**Acceptance Criteria:**
- LED update latency ≤ 50ms (measured from state change to LED output)
- Consistent timing across all state transitions
- No perceptible delay to human observer

**Dependencies:**
- SWE-REQ-012 (LED State Indication)

**Verification Method:** Test (timing tests with oscilloscope or HAL timestamps)

**Test Cases:** TC-007, TC-008, IT-005

---

### SWE-REQ-014: Conflicting Input Handling (N/A for Rocker Switch)

**Category:** Safety  
**Priority:** Mandatory  
**Derived From:** UC-07 (Simultaneous Button Presses)

**Description:**

The ON/OFF/ON rocker switch interface prevents conflicting inputs by design. No software action is required to handle simultaneous up and down commands.

**Rationale:**

The hardware switch cannot be in both UP and DOWN positions simultaneously, eliminating this class of error.

**Acceptance Criteria:**
- No conflicting input state is possible with the rocker switch

**Dependencies:**
- None

**Verification Method:** N/A (hardware enforced)

**Test Cases:** N/A

---

### SWE-REQ-015: Error Detection

**Category:** Safety  
**Priority:** Mandatory  
**Derived From:** UC-08 (Error Indication and Recovery)

**Description:**

The software shall detect error conditions including:
- Movement timeout exceeded (> 30 seconds continuous movement)
- Both limit switches active simultaneously (invalid hardware state)
- Invalid state transitions (software fault detection)
- Any other system fault conditions

**Rationale:**

Error detection enables the system to recognize fault conditions and enter a safe state, preventing damage or unsafe operation.

**Acceptance Criteria:**
- All specified error conditions detected within one control cycle
- Timeout detection accurate to ±100ms
- Invalid state detection prevents undefined behavior
- Error detection rate: 100% for all test scenarios

**Dependencies:**
- SWE-REQ-018 (Movement Timeout)

**Verification Method:** Test (integration tests with fault injection)

**Test Cases:** TC-016, TC-017, IT-008

---

### SWE-REQ-016: Error Recovery

**Category:** Functional  
**Priority:** Mandatory  
**Derived From:** UC-08 (Error Indication and Recovery)

**Description:**

The software shall allow error recovery through power cycle:
- While in ERROR state, the software shall ignore all button inputs
- Upon power cycle, the software shall reinitialize (SWE-REQ-001)
- If error condition cleared, system shall enter IDLE state
- If error condition persists, system shall re-enter ERROR state

**Rationale:**

Power cycle reset is a simple, reliable recovery mechanism that ensures full system reinitialization. Persistent errors are re-detected, providing robust fault handling.

**Acceptance Criteria:**
- ERROR state lockout verified (button presses have no effect)
- Power cycle successfully clears transient errors (100% success rate)
- Persistent errors re-detected on restart (100% detection rate)
- System behavior after recovery identical to normal operation

**Dependencies:**
- SWE-REQ-001 (System Initialization)
- SWE-REQ-015 (Error Detection)

**Verification Method:** Test (integration tests with error injection and power cycle)

**Test Cases:** TC-016, TC-017, IT-008

---


### SWE-REQ-017: Switch State Reading (Rocker Switch)

**Category:** Functional  
**Priority:** Mandatory  
**Derived From:** UC-02 (Raise Desk), UC-03 (Lower Desk), all switch-related use cases

**Description:**

The software shall read the ON/OFF/ON rocker switch state via HAL_ReadSwitchState() function.

**Specific Requirements:**
- Switch state shall be read directly from the two switch pins (UP and DOWN)
- No software debouncing is required due to the hardware switch design
- Switch state shall be mapped to an enum: UP, OFF, DOWN

**Rationale:**

The rocker switch provides a stable, hardware-debounced interface, eliminating the need for software debouncing.

**Acceptance Criteria:**
- Switch state is always accurately detected
- No false triggers or bounce observed in operation
- Enum mapping is correct and documented

**Dependencies:**
- None (fundamental input handling)

**Verification Method:** Test (unit tests with simulated switch toggling)

**Test Cases:** TC-003, TC-005, IT-002, IT-003

---

### SWE-REQ-018: Movement Timeout (BTS7960/IBT-2)

**Category:** Safety  
**Priority:** Mandatory  
**Derived From:** UC-02 (Raise Desk), UC-03 (Lower Desk)

**Description:**

The software shall enforce a maximum continuous movement duration of 30 seconds.

If movement continues for 30 seconds without button release, the software shall:
- Set both RPWM and LPWM to LOW (motor stop), disables as required
- Transition to IDLE state (normal timeout, not error)
- Return to ready state
### SWE-REQ-021: Motor Current Sensing (Optional)

**Category:** Functional  
**Priority:** Optional  
**Derived From:** Hardware capability (BTS7960/IBT-2)


**Description:**

If current sense outputs (R_IS, L_IS) are connected to Arduino analog pins (A0, A1), the software shall monitor these pins to detect overcurrent or stall conditions and implement stall protection.


**Specific Requirements:**
- The software shall configure A0 and A1 as analog inputs if current sense is used
- The software shall periodically read A0 and A1 during movement
- If current exceeds a configurable threshold, the software shall stop the motor and enter ERROR state
- The software shall implement stall protection by monitoring current and stopping the motor if a stall is detected (current exceeds calibrated threshold)


**Rationale:**
Current sensing enables detection of stalls or mechanical faults, providing additional protection for the motor and driver. Stall protection prevents hardware damage due to mechanical jams or excessive load.


**Acceptance Criteria:**
- Overcurrent/stall detected within 100ms of event
- Motor stops and ERROR state entered on overcurrent or stall
- No false positives during normal operation
- Stall protection threshold is calibrated and documented

**Dependencies:**
- SWE-REQ-015 (Error Detection)

**Verification Method:** Test (integration tests with simulated overcurrent)


**Test Cases:** IT-008 (if implemented)
TC-016: Simulate motor stall (block movement) and verify system stops motor and enters ERROR state within 100ms
TC-017: Run motor under normal load and verify no false stall detection
TC-018: Calibrate stall threshold and verify detection is reliable across load conditions

**Rationale:**

Movement timeout prevents prolonged operation if button is stuck or user forgets to release. Protects motor and mechanical components from overheating or excessive wear.

**Acceptance Criteria:**
- Timeout triggers after 30 seconds (±500ms) of continuous movement
- Motor stops within 50ms of timeout expiration
- System returns to IDLE state (not ERROR, since this is normal operation)
- Timeout resets when button is released and re-pressed

**Dependencies:**
- SWE-REQ-007 (Upward Movement Termination)
- SWE-REQ-008 (Downward Movement Termination)

**Verification Method:** Test (integration tests with simulated long button press)

**Test Cases:** TC-009, TC-010, IT-002, IT-003

---

### SWE-REQ-019: Emergency Stop Response Time

**Category:** Performance  
**Priority:** Mandatory  
**Derived From:** UC-04 (Emergency Stop)

**Description:**

The software shall de-energize the motor within 50ms of detecting an emergency stop condition.

**Rationale:**

Rapid response to emergency conditions is critical for safety. 50ms response time limits desk movement to negligible distance, minimizing risk.

**Acceptance Criteria:**
- Emergency stop latency ≤ 50ms (measured from detection to motor stop command)
- Response time consistent across all emergency scenarios
- Verified via timing tests or HAL timestamps

**Dependencies:**
- SWE-REQ-010 (Emergency Stop Detection)
- SWE-REQ-011 (Emergency Stop Execution)

**Verification Method:** Test (timing tests with oscilloscope or HAL timestamps)

**Test Cases:** TC-011, TC-012, TC-015, IT-004

---

### SWE-REQ-020: State Transition Integrity

**Category:** Functional  
**Priority:** Mandatory  
**Derived From:** Derived requirement (all use cases)

**Description:**

The software shall ensure valid state transitions according to the state machine:
- IDLE → MOVING_UP (Up button pressed, conditions met)
- IDLE → MOVING_DOWN (Down button pressed, conditions met)
- MOVING_UP → IDLE (button released, timeout, or limit reached)
- MOVING_DOWN → IDLE (button released, timeout, or limit reached)
- Any state → ERROR (emergency condition detected)
- No other transitions are permitted

**Rationale:**

Strict state machine enforcement prevents undefined behavior, race conditions, and improves system predictability and safety.

**Acceptance Criteria:**
- Only valid transitions occur (100% compliance in all test scenarios)
- Invalid transition attempts are blocked or trigger error
- State transitions are atomic (no intermediate undefined states)
- Current state always determinable and valid

**Dependencies:**
- All state-related requirements

**Verification Method:** Test (state machine verification tests)

**Test Cases:** All test cases verify state transitions

---

## Traceability

This section provides bidirectional traceability:

### Use Case → Requirements (Backward Trace)

| Use Case | Requirements Derived |
|----------|---------------------|
| UC-01 | SWE-REQ-001, SWE-REQ-002 |
| UC-02 | SWE-REQ-003, SWE-REQ-005, SWE-REQ-007, SWE-REQ-017, SWE-REQ-018 |
| UC-03 | SWE-REQ-004, SWE-REQ-006, SWE-REQ-008, SWE-REQ-017, SWE-REQ-018 |
| UC-04 | SWE-REQ-010, SWE-REQ-011, SWE-REQ-019 |
| UC-05 | SWE-REQ-012, SWE-REQ-013 |
| UC-06 | SWE-REQ-009 |
| UC-07 | SWE-REQ-010, SWE-REQ-014 |
| UC-08 | SWE-REQ-015, SWE-REQ-016 |

### Requirements → Test Cases (Forward Trace)

See [Traceability Matrix](TraceabilityMatrix.md) for complete requirements-to-test-cases mapping.

---

## Requirements Metrics

### Summary Statistics
- **Total Requirements:** 20
- **Functional:** 12 (60%)
- **Safety:** 6 (30%)
- **Interface:** 1 (5%)
- **Performance:** 2 (10%)

### By Priority
- **Mandatory:** 20 (100%)

### By Status
- **Approved:** 20 (100%)
- **Draft:** 0
- **Under Review:** 0

### Verification Coverage
- **Test Verified:** 20 (100%)
- **Test Cases Defined:** 100%
- **Tests Passing:** See test execution reports

---

## Quality Attributes

All requirements in this document adhere to the following quality criteria:

### Clarity
- Written in clear, unambiguous language using "shall" for mandatory requirements
- Technical terms defined in [Glossary](Glossary.md)
- No subjective or interpretive language

### Testability
- Each requirement has defined acceptance criteria
- All requirements verifiable through testing
- Test cases defined for all requirements

### Atomicity
- Each requirement addresses a single concern
- No compound requirements (multiple behaviors in one requirement)
- Requirements can be implemented and tested independently

### Implementation Independence
- Requirements specify "what", not "how"
- Focus on externally observable behavior
- No design constraints unless necessary

### Traceability
- All requirements traced to use cases (backward trace)
- All requirements traced to test cases (forward trace)
- Dependencies between requirements documented

### Completeness
- All use case scenarios covered
- Normal, alternative, and exception flows addressed
- Performance and timing requirements specified

---

## Change Management

### Version History
| Version | Date | Changes | Author |
|---------|------|---------|--------|
| 1.0 | Initial | Initial requirements specification | Development Team |
| 1.1 | Jan 2026 | Added derived requirements, timing specs | Development Team |
| 1.2 | Jan 7, 2026 | Reformatted to IEEE 29148-2018 standard | Development Team |

### Change Process
- Requirements changes require approval from System Engineering
- Safety requirements (SWE-REQ-010, 011, 014, 015, 018, 019) require safety team review
- All changes must update traceability matrices
- Test cases must be updated for modified requirements

---

## Notes

- Requirements are subject to change based on future enhancements or hardware modifications
- All requirements comply with [INCOSE](https://www.incose.org/) best practices for systems engineering
- Safety requirements follow ISO 26262 principles for functional safety
- This document is maintained in sync with:
  - [System Use Cases](SystemUseCases.md)
  - [Software Test Cases Specification](SoftwareTestCasesSpecification.md)
  - [Traceability Matrix](TraceabilityMatrix.md)

---

*For questions or suggestions, open an issue or contact the project maintainers.*
