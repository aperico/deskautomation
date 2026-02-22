# Software Requirements

## Overview

This document defines functional software requirements derived from system requirements. These requirements specify **what** the software subsystem shall do, independent of hardware implementation. All requirements are designed to be unit-testable using mock hardware abstractions.

**Document Version:** 1.1  
**Date:** January 25, 2026  
**Status:** Updated

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


## Requirements Characteristics

All software requirements comply with INCOSE quality criteria:

- ✅ **Necessary**: Each requirement addresses a distinct software function
- ✅ **Unambiguous**: Uses "shall" with quantitative acceptance criteria
- ✅ **Verifiable**: Unit test methods specified using mocks/stubs
- ✅ **Feasible**: All requirements implementable in embedded software
- ✅ **Traceable**: Clear links to parent system requirements
- ✅ **Software-focused**: Describes software behavior, not hardware implementation

## Traceability

**Derived from:**
- [03_00_SystemRequirements.md](03_00_SystemRequirements.md)

**Verification:**
- [04_01_SoftwareVerificationSpecification.md](04_01_SoftwareVerificationSpecification.md)

---

## Requirements Specification

| SWReq-ID | Statement | Rationale | Derives From |
|----------|-----------|-----------|--------------|
| **SWReq-001** | The software subsystem shall detect UP button press within 100 ms and command motor activation in the upward direction. | Supports system responsiveness requirement (SysReq-002). Software input processing latency must be minimized to meet 1.0 sec total response budget. | SysReq-002 |
| **SWReq-002** | The software subsystem shall detect DOWN button press within 100 ms and command motor activation in the downward direction. | Supports system responsiveness requirement (SysReq-002). Software input processing latency must be minimized to meet 1.0 sec total response budget. | SysReq-002 |
| **SWReq-003** | The software subsystem shall detect button release within 50 ms and command motor deactivation. | Critical for meeting 500 ms halt requirement (SysReq-003). Software must respond faster than system-level budget to account for hardware response time. | SysReq-003 |
| **SWReq-004** | The software subsystem shall ignore motor activation commands when both UP and DOWN buttons are detected as pressed simultaneously. | Implements safety logic for conflicting inputs (SysReq-005). Prevents undefined motor behavior and potential hardware stress. | SysReq-005 |
| **SWReq-005** | The software subsystem shall command motor stop when upper limit sensor is active, even if UP button is pressed. | Enforces travel limit protection (SysReq-007). Software must override user input to protect mechanical system. | SysReq-007 |
| **SWReq-006** | The software subsystem shall command motor stop when lower limit sensor is active, even if DOWN button is pressed. | Enforces travel limit protection (SysReq-007). Software must override user input to protect mechanical system. | SysReq-007 |
| **SWReq-007** | The software subsystem shall maintain motor control state machine with at least three states: IDLE, MOVING_UP, and MOVING_DOWN. | Provides structured control logic for predictable behavior (SysReq-002, SysReq-003). State machine ensures unambiguous system response to all input combinations. | SysReq-002, SysReq-003 |
| **SWReq-008** | The software subsystem shall execute the main control loop with a cycle time of ≤ 50 ms. | Ensures timely input processing and output response to meet system timing requirements (SysReq-002, SysReq-003). Fast loop enables responsive button detection and motor commands. | SysReq-002, SysReq-003 |
| **SWReq-009** | The software subsystem shall debounce button inputs using a 20 ms debounce window. | Prevents false triggers from mechanical button bounce, ensuring reliable user intent detection (SysReq-002). Industry-standard debounce time for tactile switches. | SysReq-002 |
| **SWReq-010** | The software subsystem shall maintain operational state and command history for diagnostic purposes. | Supports reliability and maintainability goals (SysReq-008). Enables post-failure analysis and verification of state transitions during testing. | SysReq-008 |
| **SWReq-011** | The software subsystem shall implement a non-blocking scheduler that invokes `APP_Task()` at a fixed period of 250 ms (±10 ms). | Provides deterministic application-level timing while avoiding blocking calls in the main loop, enabling timely safety reactions and I/O servicing. | SysReq-009 |
| **SWReq-012** | The software subsystem shall command motor STOP when no valid UP/DOWN command is present. | Prevents unintended motion in the absence of user intent (SysReq-010). | SysReq-010 |
| **SWReq-013** | The software subsystem shall initialize to a safe STOP state after reset or brownout and require a new valid command before motion. | Prevents unintended motion on power recovery (SysReq-011). | SysReq-011 |
| **SWReq-014** | The software subsystem shall detect motor driver stuck-on or runaway behavior and enter a fault state that commands STOP and indicates error. | Mitigates electrical fault hazards (SysReq-012). | SysReq-012 |
| **SWReq-015** | The software subsystem shall support configurable motor driver implementation (MT_BASIC: L298N or MT_ROBUST: IBT_2) with identical functional behavior across both variants. | Enables product line support for multiple hardware configurations from unified codebase (SysReq-014). All control logic, safety features, and timing requirements shall be motor-type-agnostic; differences encapsulated in HAL. | SysReq-014 |

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
| SysReq-014: Motor Type Configuration | SWReq-015 |

---

## Test Strategy

All software verification specifications, test methods, test procedures, and test coverage requirements are defined in [04_01_SoftwareVerificationSpecification.md](04_01_SoftwareVerificationSpecification.md).

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
| Motor Configuration Module | SWReq-015 |




