# Software Requirements Specification

**Document Version:** 1.0  
**Last Updated:** January 19, 2026  
**Status:** Approved  
**Author:** Development Team  
**Reviewers:** System Engineering, Safety Team  
**Compliance:** IEEE 29148-2018, INCOSE, ISO 26262

---

## Document History

| Version | Date | Author | Status | Changes |
|---------|------|--------|--------|---------|
| 1.0 | Jan 19, 2026 | Dev Team | Approved | Initial v1.0 software requirements specification with 20 SWE-REQ entries |

---

## Overview

This document specifies the detailed software requirements (SWE-REQ-*) for the Automated Mechanical Desk Lift System. These requirements derive from [06_SystemRequirements.md](06_SystemRequirements.md) and [05_SystemUseCases.md](05_SystemUseCases.md), defining the functional, safety, and performance characteristics that software must implement.

All requirements are:
- **Derived from** System Use Cases and System Requirements
- **Traceable** to use cases, system requirements, and test cases
- **Testable** with defined acceptance criteria
- **Unambiguous** and implementation-independent
- **Compliant** with INCOSE and IEEE standards for requirements engineering

---

## Navigation

- [System Requirements](06_SystemRequirements.md) ← Requirement source
- [System Use Cases](05_SystemUseCases.md) ← Requirement source
 - [Software Architecture](08_SoftwareArchitecture.md) ← Implementation design
- [Traceability Matrix](12_TraceabilityMatrix.md) ← Cross-reference

---

## Requirements Summary

| Req ID | Name | Category | Priority | Derived From | Verification | Status |
|--------|------|----------|----------|--------------|--------------|--------|
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
| SWE-REQ-014 | Conflicting Input Handling | Safety | Mandatory | UC-07 | Test | Approved |
| SWE-REQ-015 | Error Detection | Safety | Mandatory | UC-08 | Test | Approved |
| SWE-REQ-016 | Error Recovery | Functional | Mandatory | UC-08 | Test | Approved |
| SWE-REQ-017 | Switch Debouncing | Functional | Mandatory | UC-02, UC-03 | Test | Approved |
| SWE-REQ-018 | Movement Timeout | Safety | Mandatory | UC-02, UC-03 | Test | Approved |
| SWE-REQ-019 | Emergency Stop Response Time | Performance | Mandatory | UC-04 | Test | Approved |
| SWE-REQ-020 | State Transition Integrity | Functional | Mandatory | Core | Test | Approved |
| SWE-REQ-021 | Current Monitoring | Safety | Mandatory | UC-09 | Test | Approved |
| SWE-REQ-022 | PWM Soft-Start for Motor | Functional | Mandatory | UC-10 | Test | Approved |

---

## Detailed Software Requirements

### SWE-REQ-001: System Initialization

**Category:** Functional  
**Classification:** Functional Requirement  
**Priority:** Mandatory  
**Derived From:** UC-01 (Power-On), SYS-REQ-005 (Power Supply Requirements)

**Description:**

The software shall initialize all system components and enter IDLE state when power is applied.

**Specific Requirements:**
- Initialize Arduino GPIO pins for input and output
- Configure PWM timers for motor control (1 kHz minimum)
- Initialize analog-to-digital converter for current sense reading
- Set all motor control pins to safe states (motors de-energized)
- Perform self-check routine (verify all I/O pins are readable)
- Transition to IDLE state within 2 seconds of power-up
- No movement initiated during initialization

**Acceptance Criteria:**
- System enters IDLE state within 2 seconds
- All GPIO pins configured correctly
- No spurious motor activation during startup
- System ready to accept user input after initialization

**Dependencies:**
- Arduino hardware functioning
- Power supply providing stable 5V to ECU

**Verification Method:** Automated test (functional verification)

---

### SWE-REQ-002: Ready State Indication

**Category:** Functional  
**Classification:** Functional Requirement  
**Priority:** Mandatory  
**Derived From:** UC-01 (Power-On), SYS-REQ-011 (User Feedback)

**Description:**

The software shall indicate system readiness for user commands (v1.0: implicit via lack of motor motion; v2.0+: explicit via LED).

**Specific Requirements:**
- v1.0: System remains silent and still when in IDLE state
- v1.0: No automatic movement or operation
- v1.0: Accept rocker switch input immediately upon entering IDLE
- v2.0+: Illuminate green LED when in IDLE state

**Acceptance Criteria:**
- v1.0: System clearly indicates readiness through absence of activity
- User can begin operation immediately upon power-up (after initialization)
- No false ready indications

**Verification Method:** Visual inspection and functional test

---

### SWE-REQ-003: Upward Movement Detection

**Category:** Functional  
**Classification:** Functional Requirement  
**Priority:** Mandatory  
**Derived From:** UC-02 (Raise Desk), SYS-REQ-001 (Desk Height Control Range), SYS-REQ-002 (User Input Interface)

**Description:**

The software shall detect when the rocker switch is moved to the UP position and interpret this as a command to raise the desk.

**Specific Requirements:**
- Read rocker switch GPIO pin (UP position)
- Debounce input signal (50ms minimum stable time)
- Detect UP position transition from OFF to UP
- Valid transition only from IDLE state
- Ignore UP if in MOVING_DOWN or ERROR state
- Validate no conflicting inputs (DOWN not simultaneously active)

**Acceptance Criteria:**
- UP position detected within 100ms of user action (including 50ms debounce)
- No false detections from electrical noise
- Debounced input prevents spurious transitions
- Conflicting inputs rejected safely

**Dependencies:**
- SWE-REQ-017 (Switch Debouncing)
- SWE-REQ-014 (Conflicting Input Handling)

**Verification Method:** Test (functional with signal injection)

---

### SWE-REQ-004: Downward Movement Detection

**Category:** Functional  
**Classification:** Functional Requirement  
**Priority:** Mandatory  
**Derived From:** UC-03 (Lower Desk), SYS-REQ-001 (Desk Height Control Range), SYS-REQ-002 (User Input Interface)

**Description:**

The software shall detect when the rocker switch is moved to the DOWN position and interpret this as a command to lower the desk.

**Specific Requirements:**
- Read rocker switch GPIO pin (DOWN position)
- Debounce input signal (50ms minimum stable time)
- Detect DOWN position transition from OFF to DOWN
- Valid transition only from IDLE state
- Ignore DOWN if in MOVING_UP or ERROR state
- Validate no conflicting inputs (UP not simultaneously active)

**Acceptance Criteria:**
- DOWN position detected within 100ms of user action (including 50ms debounce)
- No false detections from electrical noise
- Debounced input prevents spurious transitions
- Conflicting inputs rejected safely

**Dependencies:**
- SWE-REQ-017 (Switch Debouncing)
- SWE-REQ-014 (Conflicting Input Handling)

**Verification Method:** Test (functional with signal injection)

---

### SWE-REQ-005: Upward Movement Execution

**Category:** Functional  
**Classification:** Functional Requirement  
**Priority:** Mandatory  
**Derived From:** UC-02 (Raise Desk), SYS-REQ-001 (Desk Height Control Range), SYS-REQ-006 (Motor Control)

**Description:**

The software shall execute upward desk movement when UP command is detected and conditions permit.

**Specific Requirements:**
- Upon detecting valid UP input, transition to MOVING_UP state
- Set motor direction to UP (configure H-bridge enable pins R_EN/L_EN appropriately)
- Apply PWM soft-start ramp (SWE-REQ-022)
- Energize motor driver with PWM signal on appropriate pin (RPWM)
- Maintain MOVING_UP state while UP input is active
- Continuous monitoring of motor current (SWE-REQ-021)
- Hold motor energized as long as UP position maintained

**Acceptance Criteria:**
- Desk moves upward smoothly when UP activated
- Motor accelerates via soft-start ramp, no mechanical shock
- Desk continues moving while rocker in UP position
- Current monitoring active throughout movement
- Motor responds within 100ms of command

**Dependencies:**
- SWE-REQ-022 (PWM Soft-Start)
- SWE-REQ-021 (Current Monitoring)
- SWE-REQ-020 (State Transitions)

**Verification Method:** Test (functional with mechanical verification)

---

### SWE-REQ-006: Downward Movement Execution

**Category:** Functional  
**Classification:** Functional Requirement  
**Priority:** Mandatory  
**Derived From:** UC-03 (Lower Desk), SYS-REQ-001 (Desk Height Control Range), SYS-REQ-006 (Motor Control)

**Description:**

The software shall execute downward desk movement when DOWN command is detected and conditions permit.

**Specific Requirements:**
- Upon detecting valid DOWN input, transition to MOVING_DOWN state
- Set motor direction to DOWN (configure H-bridge enable pins R_EN/L_EN appropriately)
- Apply PWM soft-start ramp (SWE-REQ-022)
- Energize motor driver with PWM signal on appropriate pin (LPWM)
- Maintain MOVING_DOWN state while DOWN input is active
- Continuous monitoring of motor current (SWE-REQ-021)
- Hold motor energized as long as DOWN position maintained

**Acceptance Criteria:**
- Desk moves downward smoothly when DOWN activated
- Motor accelerates via soft-start ramp, no mechanical shock
- Desk continues moving while rocker in DOWN position
- Current monitoring active throughout movement
- Motor responds within 100ms of command

**Dependencies:**
- SWE-REQ-022 (PWM Soft-Start)
- SWE-REQ-021 (Current Monitoring)
- SWE-REQ-020 (State Transitions)

**Verification Method:** Test (functional with mechanical verification)

---

### SWE-REQ-007: Upward Movement Termination

**Category:** Functional  
**Classification:** Functional Requirement  
**Priority:** Mandatory  
**Derived From:** UC-02 (Raise Desk), SYS-REQ-003 (Movement Safety Interlocks)

**Description:**

The software shall stop upward desk movement when conditions require (user release, limit reached, timeout, fault).

**Specific Requirements:**
- Detect rocker switch return to OFF (center) position
- Upon OFF detection: set PWM to zero, transition to IDLE state
- Detect upper limit reached (if limit switch present, or timeout-based)
- Upon upper limit: set PWM to zero, transition to IDLE state
- Detect movement timeout (> 30 seconds continuous movement)
- Upon timeout: set PWM to zero, transition to ERROR state
- Detect fault condition (stall, overcurrent, invalid state)
- Upon fault: immediately set PWM to zero, transition to ERROR state
- De-energize motor within 50ms of stop condition detection

**Acceptance Criteria:**
- Desk stops immediately upon rocker switch return to OFF
- Desk stops at upper limit (no over-travel)
- Desk stops on timeout (safety feature)
- Desk stops on fault (safety feature)
- Motor is cleanly de-energized (PWM set to zero)
- Transition to appropriate state (IDLE or ERROR)

**Dependencies:**
- SWE-REQ-018 (Movement Timeout)
- SWE-REQ-015 (Error Detection)
- SWE-REQ-020 (State Transitions)

**Verification Method:** Test (functional timing verification)

---

### SWE-REQ-008: Downward Movement Termination

**Category:** Functional  
**Classification:** Functional Requirement  
**Priority:** Mandatory  
**Derived From:** UC-03 (Lower Desk), SYS-REQ-003 (Movement Safety Interlocks)

**Description:**

The software shall stop downward desk movement when conditions require (user release, limit reached, timeout, fault).

**Specific Requirements:**
- Detect rocker switch return to OFF (center) position
- Upon OFF detection: set PWM to zero, transition to IDLE state
- Detect lower limit reached (if limit switch present, or timeout-based)
- Upon lower limit: set PWM to zero, transition to IDLE state
- Detect movement timeout (> 30 seconds continuous movement)
- Upon timeout: set PWM to zero, transition to ERROR state
- Detect fault condition (stall, overcurrent, invalid state)
- Upon fault: immediately set PWM to zero, transition to ERROR state
- De-energize motor within 50ms of stop condition detection

**Acceptance Criteria:**
- Desk stops immediately upon rocker switch return to OFF
- Desk stops at lower limit (no over-travel)
- Desk stops on timeout (safety feature)
- Desk stops on fault (safety feature)
- Motor is cleanly de-energized (PWM set to zero)
- Transition to appropriate state (IDLE or ERROR)

**Dependencies:**
- SWE-REQ-018 (Movement Timeout)
- SWE-REQ-015 (Error Detection)
- SWE-REQ-020 (State Transitions)

**Verification Method:** Test (functional timing verification)

---

### SWE-REQ-009: Power Loss Handling

**Category:** Functional  
**Classification:** Functional Requirement  
**Priority:** Mandatory  
**Derived From:** UC-06 (Power-Off During Movement), SYS-REQ-004 (Emergency Stop Mechanism)

**Description:**

The software shall safely respond to power loss by allowing graceful shutdown and recovery.

**Specific Requirements:**
- Upon power loss, motor driver automatically de-energizes (no latch)
- System loses all state information (stateless reset on power cycle)
- No memory of previous position or operation
- Upon power restoration, system re-initializes per SWE-REQ-001
- No automatic movement after power recovery
- Desk position unknown after power cycle (user must manually verify)

**Acceptance Criteria:**
- Motor stops immediately upon power loss (hardware feature)
- System fully resets on power cycle
- No damage to mechanical or electrical components
- System ready for operation immediately after power-up

**Dependencies:**
- Motor driver hardware (automatic de-energize on power loss)
- SWE-REQ-001 (System Initialization)

**Verification Method:** Test (power cycle simulation)

---

### SWE-REQ-010: Emergency Stop Detection

**Category:** Safety  
**Classification:** Safety Requirement  
**Priority:** Mandatory  
**Derived From:** UC-04 (Emergency Stop), UC-07 (Conflicting Inputs), SYS-REQ-004 (Emergency Stop Mechanism)

**Description:**

The software shall detect emergency stop conditions and respond immediately.

**Specific Requirements:**
- Detect rocker switch in OFF (center) position for > 50ms during MOVING state
- Detect simultaneous UP and DOWN signals (conflicting input)
- Detect movement timeout (> 30 seconds)
- Detect motor stall (current monitoring threshold exceeded)
- Detect invalid state transitions
- Upon emergency condition detection: immediately transition to ERROR state
- Trigger emergency stop execution (SWE-REQ-011) within 50ms

**Acceptance Criteria:**
- All emergency conditions detected within 50ms
- Immediate transition to ERROR state
- Motor stopped within 50ms of detection
- System locked until power cycle

**Dependencies:**
- SWE-REQ-011 (Emergency Stop Execution)
- SWE-REQ-014 (Conflicting Input Handling)
- SWE-REQ-021 (Current Monitoring)

**Verification Method:** Test (simulated fault conditions)

---

### SWE-REQ-011: Emergency Stop Execution

**Category:** Safety  
**Classification:** Safety Requirement  
**Priority:** Mandatory  
**Derived From:** UC-04 (Emergency Stop), SYS-REQ-004 (Emergency Stop Mechanism)

**Description:**

The software shall execute emergency stop by de-energizing motor and entering ERROR state.

**Specific Requirements:**
- Set all motor PWM pins to zero (disable PWM)
- Set motor enable pins (R_EN, L_EN) to LOW (disable motor)
- Transition to ERROR state
- Set error flag preventing any further movement
- Ignore all user inputs until power cycle
- Maintain ERROR state indefinitely

**Acceptance Criteria:**
- Motor de-energized within 50ms of emergency condition
- All PWM and enable signals cleared
- System in ERROR state, refusing all commands
- System remains locked until power cycle

**Dependencies:**
- SWE-REQ-010 (Emergency Stop Detection)
- SWE-REQ-020 (State Transitions)

**Verification Method:** Test (functional verification with timers)

---

### SWE-REQ-014: Conflicting Input Handling

**Category:** Safety  
**Classification:** Safety Requirement  
**Priority:** Mandatory  
**Derived From:** UC-07 (Conflicting Rocker Switch Inputs), SYS-REQ-002 (User Input Interface)

**Description:**

The software shall detect and safely handle conflicting rocker switch inputs (simultaneous UP and DOWN signals).

**Specific Requirements:**
- Monitor both UP and DOWN input signals continuously
- Detect simultaneous active state (both UP and DOWN HIGH or detected within debounce window)
- Upon conflict detection: immediately transition to ERROR state
- Invoke emergency stop execution (SWE-REQ-011)
- Prevent any motion command from executing
- Log conflict condition for diagnostics

**Acceptance Criteria:**
- Conflicting inputs detected and flagged within 50ms
- System immediately enters ERROR state
- Motor de-energized
- No conflicting movement initiated
- System locked until power cycle

**Dependencies:**
- SWE-REQ-010 (Emergency Stop Detection)
- SWE-REQ-011 (Emergency Stop Execution)
- SWE-REQ-017 (Switch Debouncing)

**Verification Method:** Test (simulated conflicting inputs)

---

### SWE-REQ-015: Error Detection

**Category:** Safety  
**Classification:** Safety Requirement  
**Priority:** Mandatory  
**Derived From:** UC-08 (Error Detection and Recovery), SYS-REQ-007 (Fault Detection and Recovery)

**Description:**

The software shall detect runtime errors and system faults, transitioning to ERROR state.

**Specific Requirements:**
- Detect movement timeout: continuous movement > 30 seconds
- Detect motor stall: current reading exceeds configured threshold
- Detect overcurrent: current reading exceeds max safe threshold
- Detect invalid state transitions: attempt to transition from invalid states
- Detect conflicting inputs: simultaneous UP and DOWN
- Upon any error detection: immediately transition to ERROR state
- Set error flag (persistent until power cycle)
- Invoke emergency stop execution (SWE-REQ-011)
- Log error type and timestamp (if diagnostic logging available)

**Acceptance Criteria:**
- All error conditions detected and logged
- System immediately enters ERROR state
- Motor stopped within 50ms
- Error persistent (not auto-clearing)
- System provides no feedback until power cycle (v1.0)

**Dependencies:**
- SWE-REQ-018 (Movement Timeout)
- SWE-REQ-021 (Current Monitoring)
- SWE-REQ-011 (Emergency Stop Execution)

**Verification Method:** Test (simulated fault conditions)

---

### SWE-REQ-016: Error Recovery

**Category:** Functional  
**Classification:** Functional Requirement  
**Priority:** Mandatory  
**Derived From:** UC-08 (Error Detection and Recovery), SYS-REQ-007 (Fault Detection and Recovery)

**Description:**

The software shall provide recovery path from ERROR state via power cycle.

**Specific Requirements:**
- Once in ERROR state, no commands are accepted
- System ignores all user input (rocker switch)
- System ignores all sensor inputs (current sense)
- No automatic recovery; manual power cycle required
- Upon power cycle: re-initialize per SWE-REQ-001
- If error condition persists: system immediately re-enters ERROR
- If error condition cleared: system enters IDLE state and functions normally

**Acceptance Criteria:**
- ERROR state persists indefinitely until power cycle
- User has opportunity to inspect/fix problem
- System recovers cleanly if error condition resolved
- System re-enters ERROR if problem persists

**Dependencies:**
- SWE-REQ-001 (System Initialization)
- SWE-REQ-015 (Error Detection)

**Verification Method:** Test (power cycle after fault)

---

### SWE-REQ-017: Switch Debouncing

**Category:** Functional  
**Classification:** Functional Requirement  
**Priority:** Mandatory  
**Derived From:** UC-02 (Raise Desk), UC-03 (Lower Desk), SYS-REQ-002 (User Input Interface)

**Description:**

The software shall debounce the rocker switch input to filter electrical noise and spurious transitions.

**Specific Requirements:**
- Read rocker switch GPIO pin continuously (polling or interrupt)
- Debounce time: 50ms minimum stable time required before state acceptance
- Ignore transitions lasting < 50ms (treat as noise)
- Update internal state only after 50ms stable signal
- Apply debouncing to all three switch positions (UP, OFF, DOWN)
- Debouncing independent per position

**Acceptance Criteria:**
- Electrical noise does not cause false state transitions
- User perceives immediate response (within 100ms total: 50ms debounce + 50ms response)
- No missed valid inputs
- No false positives from hardware jitter

**Dependencies:**
- GPIO input driver (hardware)

**Verification Method:** Test (with simulated noise signal)

---

### SWE-REQ-018: Movement Timeout

**Category:** Safety  
**Classification:** Safety Requirement  
**Priority:** Mandatory  
**Derived From:** UC-02 (Raise Desk), UC-03 (Lower Desk), SYS-REQ-001 (Desk Height Control Range)

**Description:**

The software shall limit continuous movement duration to prevent extended operation that might indicate a jam or stall.

**Specific Requirements:**
- Monitor movement duration continuously during MOVING_UP or MOVING_DOWN
- Maximum movement duration: 30 seconds continuous
- If movement exceeds 30 seconds: immediately stop motor
- Transition to ERROR state on timeout
- Reset timer upon each movement start

**Acceptance Criteria:**
- Movement stops automatically after 30 seconds
- System safely enters ERROR state
- Motor de-energized immediately
- Timeout prevents indefinite operation (safety feature)

**Dependencies:**
- SWE-REQ-011 (Emergency Stop Execution)
- SWE-REQ-015 (Error Detection)

**Verification Method:** Test (functional timing verification)

---

### SWE-REQ-019: Emergency Stop Response Time

**Category:** Performance  
**Classification:** Performance Requirement  
**Priority:** Mandatory  
**Derived From:** UC-04 (Emergency Stop), SYS-REQ-004 (Emergency Stop Mechanism)

**Description:**

The software shall respond to emergency stop conditions within strict timing requirements.

**Specific Requirements:**
- Detect emergency condition: within 50ms of condition onset
- De-energize motor: within 50ms of detection
- Total response time: within 100ms maximum
- All safety interlocks: within 50ms response
- State transitions: within 50ms
- PWM set to zero: within 50ms of stop command

**Acceptance Criteria:**
- Motor stops within 100ms of emergency condition
- All responses measured and verified within timing spec
- Performance consistent across all emergency scenarios
- No delays in safety-critical operations

**Verification Method:** Test (measured with oscilloscope or high-speed logging)

---

### SWE-REQ-020: State Transition Integrity

**Category:** Functional  
**Classification:** Functional Requirement  
**Priority:** Mandatory  
**Derived From:** SYS-REQ-003 (Movement Safety Interlocks), SYS-REQ-008 (Operational State Management)

**Description:**

The software shall maintain valid state machine transitions and prevent invalid state combinations.

**Specific Requirements:**
- Four states only: IDLE, MOVING_UP, MOVING_DOWN, ERROR
- Valid transitions:
  - IDLE → MOVING_UP (on UP input, if not at upper limit)
  - IDLE → MOVING_DOWN (on DOWN input, if not at lower limit)
  - MOVING_UP → IDLE (on OFF position or upper limit reached)
  - MOVING_DOWN → IDLE (on OFF position or lower limit reached)
  - Any state → ERROR (on fault detection)
  - ERROR → IDLE (only on power cycle recovery)
- All other transitions forbidden
- State transition time: < 50ms
- No transient invalid states
- Motor control signals match current state

**Acceptance Criteria:**
- State machine operates correctly for all scenarios
- No invalid state transitions
- Motor control signals align with current state
- State stable during normal operation

**Dependencies:**
- State machine design and implementation

**Verification Method:** Test (state coverage testing)

---

### SWE-REQ-021: Current Monitoring

**Category:** Safety  
**Classification:** Safety Requirement  
**Priority:** Mandatory  
**Derived From:** UC-09 (Motor Current Monitoring), SYS-REQ-009 (Motor Current Monitoring)

**Description:**

The software shall continuously monitor motor current via BTS7960 current sense outputs to detect stall and overcurrent conditions.

**Specific Requirements:**
- Read analog inputs A0 (R_IS) and A1 (L_IS) from motor driver
- Sampling rate: minimum 100Hz during MOVING state
- ADC resolution: 10-bit (0-1023 counts representing 0-5V)
- Current sense ratio: 0.5V/A (per BTS7960 datasheet)
- Stall threshold: configurable (typical 400-600 ADC counts ≈ 2-3A)
- Overcurrent threshold: configurable (typical 800+ ADC counts ≈ 4+ A)
- Upon stall detection: immediately transition to ERROR state
- Upon overcurrent detection: immediately transition to ERROR state
- Stop motor within 50ms of fault detection
- Log fault type for diagnostics

**Acceptance Criteria:**
- Motor current accurately monitored throughout operation
- Stall detected reliably (with minimal false positives)
- Overcurrent detected reliably
- System responds immediately to overcurrent
- Thresholds calibrated for target motor and load

**Calibration Instructions:**
1. With desk unloaded, run motor and record typical current readings
2. Manually stall motor and record peak current value
3. Set stall threshold above normal running current but below peak
4. Test with various loads to ensure reliable detection without false positives

**Dependencies:**
- BTS7960 current sense hardware
- ADC hardware
- SWE-REQ-015 (Error Detection)

**Verification Method:** Test (with current sense monitoring)

---

### SWE-REQ-022: PWM Soft-Start for Motor

**Category:** Functional  
**Classification:** Functional Requirement  
**Priority:** Mandatory  
**Derived From:** UC-10 (PWM Soft-Start for Motor), SYS-REQ-006 (Motor Control), SYS-REQ-010 (Soft-Start Motor Control)

**Description:**

The software shall apply a PWM soft-start ramp when initiating motor movement to gradually increase power and reduce mechanical shock.

**Specific Requirements:**
- Upon movement command (UP or DOWN), begin PWM ramp
- Ramp duration: configurable 200-500ms (typical 300ms)
- Initial PWM duty cycle: configurable 20-30% (typical 25%)
- Final PWM duty cycle: 100% (full speed) or target speed
- Ramp profile: linear or proportional increase over time
- If movement stopped during ramp (button released, limit reached, error): immediately set PWM to zero
- If ramp fails to reach target (hardware fault): detect and trigger error state
- Apply soft-start to both UP and DOWN movements identically

**Acceptance Criteria:**
- Motor accelerates smoothly without mechanical shock
- Ramp timing verified (200-500ms configurable)
- Motor reaches full speed after ramp
- Immediate PWM zeroing on stop (no coast-down)
- Ramp parameters configurable without code recompilation (ideally)

**Example Implementation:**
```cpp
// Soft-start ramp for motor
const unsigned int RAMP_DURATION_MS = 300;
const unsigned int INITIAL_PWM = 64;  // 25% of 255
const unsigned int TARGET_PWM = 255;   // 100%

unsigned long ramp_start_time = millis();
unsigned int current_pwm = INITIAL_PWM;

while (moving && (millis() - ramp_start_time) < RAMP_DURATION_MS) {
  // Linear ramp: PWM increases linearly over ramp duration
  float progress = (float)(millis() - ramp_start_time) / RAMP_DURATION_MS;
  current_pwm = INITIAL_PWM + (TARGET_PWM - INITIAL_PWM) * progress;
  analogWrite(PWM_PIN, current_pwm);
  delay(10);  // Sample every 10ms
}

// After ramp complete, hold at target PWM
while (moving) {
  analogWrite(PWM_PIN, TARGET_PWM);
  // ... other control logic ...
}
```

**Dependencies:**
- PWM timer hardware
- SWE-REQ-005, SWE-REQ-006 (Movement Execution)

**Verification Method:** Test (timing and smoothness verification)

---

## Assumptions and Constraints

**Assumptions:**
- Arduino UNO (ATmega328P) has sufficient processing power for real-time control
- BTS7960/IBT-2 motor driver functioning as specified
- 31ZY-5840 motor operating within rated specifications
- Rocker switch providing clean digital signals
- Analog-to-digital converter resolution adequate (10-bit minimum)

**Constraints:**
- Single-threaded execution model (no multi-threading on Arduino)
- Limited SRAM (2KB) constrains data logging and diagnostics
- Fixed clock frequency (16MHz Arduino) limits sampling rate
- No interrupts beyond timer/PWM (blocking implementation acceptable)
- Movement timeout fixed at 30 seconds (not user-configurable in v1.0)

