# Software Requirements

## Overview

This document defines functional software requirements derived from system requirements. These requirements specify **what** the software subsystem shall do, independent of hardware implementation. All requirements are designed to be unit-testable using mock hardware abstractions.

**Document Version:** 1.1  
**Date:** January 25, 2026  
**Status:** Updated

---

## Traceability

**Derived from:**
- [03_00_SystemRequirements.md](03_00_SystemRequirements.md)

---

## Requirements Specification

| SWReq-ID | Statement | Rationale | Test Method | Derives From |
|----------|-----------|-----------|-------------|--------------|
| **SWReq-001** | The software subsystem shall detect UP button press within 100 ms and command motor activation in the upward direction. | Supports system responsiveness requirement (SysReq-002). Software input processing latency must be minimized to meet 1.0 sec total response budget. | Unit test with mock GPIO: simulate button press event; verify motor command issued within 100 ms; confirm direction flag set to UP. | SysReq-002 |
| **SWReq-002** | The software subsystem shall detect DOWN button press within 100 ms and command motor activation in the downward direction. | Supports system responsiveness requirement (SysReq-002). Software input processing latency must be minimized to meet 1.0 sec total response budget. | Unit test with mock GPIO: simulate button press event; verify motor command issued within 100 ms; confirm direction flag set to DOWN. | SysReq-002 |
| **SWReq-003** | The software subsystem shall detect button release within 50 ms and command motor deactivation. | Critical for meeting 500 ms halt requirement (SysReq-003). Software must respond faster than system-level budget to account for hardware response time. | Unit test with mock GPIO: simulate button release event; verify motor stop command issued within 50 ms; confirm motor state transitions to IDLE. | SysReq-003 |
| **SWReq-004** | The software subsystem shall ignore motor activation commands when both UP and DOWN buttons are detected as pressed simultaneously. | Implements safety logic for conflicting inputs (SysReq-005). Prevents undefined motor behavior and potential hardware stress. | Unit test with mock GPIO: set both button states to pressed; call control logic; verify motor command remains STOP/IDLE; verify no state transition from IDLE. | SysReq-005 |
| **SWReq-005** | The software subsystem shall command motor stop when upper limit sensor is active, even if UP button is pressed. | Enforces travel limit protection (SysReq-007). Software must override user input to protect mechanical system. | Unit test with mock sensor: set upper limit sensor to active state; simulate UP button press; verify motor command is STOP; verify no upward motion commanded. | SysReq-007 |
| **SWReq-006** | The software subsystem shall command motor stop when lower limit sensor is active, even if DOWN button is pressed. | Enforces travel limit protection (SysReq-007). Software must override user input to protect mechanical system. | Unit test with mock sensor: set lower limit sensor to active state; simulate DOWN button press; verify motor command is STOP; verify no downward motion commanded. | SysReq-007 |
| **SWReq-007** | The software subsystem shall maintain motor control state machine with at least three states: IDLE, MOVING_UP, and MOVING_DOWN. | Provides structured control logic for predictable behavior (SysReq-002, SysReq-003). State machine ensures unambiguous system response to all input combinations. | Unit test: verify initial state is IDLE; simulate UP button press and verify transition to MOVING_UP; simulate button release and verify return to IDLE; repeat for DOWN. | SysReq-002, SysReq-003 |
| **SWReq-008** | The software subsystem shall execute the main control loop with a cycle time of ≤ 50 ms. | Ensures timely input processing and output response to meet system timing requirements (SysReq-002, SysReq-003). Fast loop enables responsive button detection and motor commands. | Unit test with mock timer: measure elapsed time for one control loop iteration; verify ≤ 50 ms; verify loop executes at consistent interval over 1000 iterations. | SysReq-002, SysReq-003 |
| **SWReq-009** | The software subsystem shall debounce button inputs using a 20 ms debounce window. | Prevents false triggers from mechanical button bounce, ensuring reliable user intent detection (SysReq-002). Industry-standard debounce time for tactile switches. | Unit test with mock GPIO: simulate rapid button state changes (bounce); verify only stable state after 20 ms is registered; confirm no spurious motor commands. | SysReq-002 |
| **SWReq-010** | The software subsystem shall maintain operational state and command history for diagnostic purposes. | Supports reliability and maintainability goals (SysReq-008). Enables post-failure analysis and verification of state transitions during testing. | Unit test: execute sequence of button commands; retrieve state history; verify all state transitions logged with timestamps; confirm motor commands recorded. | SysReq-008 |

| **SWReq-011** | The software subsystem shall implement a non-blocking scheduler that invokes `APP_Task()` at a fixed period of 250 ms (±10 ms). | Provides deterministic application-level timing while avoiding blocking calls in the main loop, enabling timely safety reactions and I/O servicing. | Unit/integration test with mock timer: advance time and verify `APP_Task()` is called at 4 Hz over ≥ 60 s; static analysis to confirm absence of blocking delays in the main loop. | SysReq-009 |
| **SWReq-012** | The software subsystem shall command motor STOP when no valid UP/DOWN command is present. | Prevents unintended motion in the absence of user intent (SysReq-010). | Unit test with mock GPIO: no buttons pressed -> motor command remains STOP; verify no state change. | SysReq-010 |
| **SWReq-013** | The software subsystem shall initialize to a safe STOP state after reset or brownout and require a new valid command before motion. | Prevents unintended motion on power recovery (SysReq-011). | Integration test with reset simulation: verify STOP state after reset; verify motion only after valid command. | SysReq-011 |
| **SWReq-014** | The software subsystem shall detect motor driver stuck-on or runaway behavior and enter a fault state that commands STOP and indicates error. | Mitigates electrical fault hazards (SysReq-012). | Integration test with fault injection: simulate stuck-on condition; verify STOP command and fault indication. | SysReq-012 |

---

## Requirements Traceability Matrix

### Upstream Traceability (System → Software)

| System Requirement | Derived Software Requirements |
|-------------------|------------------------------|
| SysReq-002: Response Time | SWReq-001, SWReq-002, SWReq-007, SWReq-008, SWReq-009 |
| SysReq-003: Motion Halt | SWReq-003, SWReq-007, SWReq-008 |
| SysReq-005: Conflicting Inputs | SWReq-004 |
| SysReq-007: Limit Protection | SWReq-005, SWReq-006 |
| SysReq-008: Reliability | SWReq-010 |
| SysReq-009: Scheduler Cadence | SWReq-011, SWReq-008 |
| SysReq-010: No Motion Without Valid Command | SWReq-012 |
| SysReq-011: Safe STOP After Reset/Brownout | SWReq-013 |
| SysReq-012: Stuck-On/Runaway Detection | SWReq-014 |

---

## Test Strategy

### Unit Testing Approach

All software requirements are unit-testable using the following mock hardware abstractions:

1. **Mock GPIO Interface**: Simulates button inputs and limit sensors
   - Set button state (pressed/released)
   - Set limit sensor state (active/inactive)
   - Query output states (motor commands)

2. **Mock Timer**: Simulates system time for timing measurements
   - Advance simulated time
   - Measure elapsed time
   - Trigger periodic events

3. **Mock Reset/Power Event**: Simulates reset or brownout conditions
   - Trigger reset event
   - Verify safe STOP initialization

4. **Mock Motor Controller**: Records motor commands for verification
   - Capture motor direction commands (UP/DOWN/STOP)
   - Log command timestamps
   - Verify command sequences

5. **Mock Fault Injector**: Simulates motor driver stuck-on behavior
   - Force motor output active without command
   - Verify fault handling and STOP response

### Test Coverage Requirements

- ✅ Each software requirement has at least one unit test
- ✅ All state transitions tested
- ✅ All input combinations tested (normal and edge cases)
- ✅ Timing requirements verified with mock timer
- ✅ Safety-critical requirements (SWReq-003, SWReq-004, SWReq-005, SWReq-006, SWReq-012, SWReq-013, SWReq-014) have multiple test cases

---

## Software Requirements Allocation

| Software Component | Allocated Requirements |
|-------------------|------------------------|
| Button Input Handler | SWReq-001, SWReq-002, SWReq-003, SWReq-009 |
| State Machine | SWReq-007, SWReq-010 |
| Safety Logic | SWReq-004, SWReq-005, SWReq-006, SWReq-012 |
| Control Loop Manager | SWReq-008 |
| Startup/Reset Handling | SWReq-013 |
| Fault Handling/Diagnostics | SWReq-014 |

---

## Requirements Characteristics

All software requirements comply with INCOSE quality criteria:

- ✅ **Necessary**: Each requirement addresses a distinct software function
- ✅ **Unambiguous**: Uses "shall" with quantitative acceptance criteria
- ✅ **Verifiable**: Unit test methods specified using mocks/stubs
- ✅ **Feasible**: All requirements implementable in embedded software
- ✅ **Traceable**: Clear links to parent system requirements
- ✅ **Software-focused**: Describes software behavior, not hardware implementation

---

## Abstraction Layer

Software requirements are intentionally **hardware-independent**:

- ❌ No pin numbers or GPIO addresses
- ❌ No motor driver chip specifications
- ❌ No PWM frequency or duty cycle details
- ✅ Focus on software logic, timing, and state management
- ✅ Hardware abstraction layer (HAL) isolates physical interface

**Hardware dependencies are encapsulated in HAL**, which provides:
- `HAL_readButton(button_id)` → button state
- `HAL_readLimitSensor(sensor_id)` → sensor state
- `HAL_setMotor(direction, speed)` → motor control
- `HAL_getTime()` → system time in ms


