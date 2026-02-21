# Software Architecture

## Overview

This document defines the software architecture for the Standing Desk Automation System. The architecture follows a layered design pattern with clear separation of concerns to enable testability, maintainability, and safety.

**Document Purpose and Detail Level**
- **Purpose:** Define the software structure, responsibilities, and interfaces needed to implement the system requirements.
- **Detail Level:** Software layers, modules, data flow, and state machine behavior; no system hardware decomposition.
- **Use:** Input to detailed design and test development; aligns with system architecture constraints.

**Document Version:** 1.0  
**Date:** January 24, 2026  
**Status:** Approved

---

## Traceability

**Derived from:**
- [03_00_SystemRequirements.md](03_00_SystemRequirements.md)
- [03_01_SystemArchitecture.md](03_01_SystemArchitecture.md)
- [04_SoftwareRequirements.md](04_SoftwareRequirements.md)

---

## Architectural Principles

1. **Testability**: Application logic is hardware-independent and unit-testable with mocks
2. **Simplicity**: Suitable for resource-constrained Arduino environment
3. **Safety**: State machine enforces safety rules and deterministic behavior
4. **Real-time**: Software design meets system-level timing constraints

---

## Timing and Safety Constraints

System-level timing and safety requirements are defined in the System Architecture and Technical Safety Concept. The software design implements these constraints via the control loop, state machine, and safety logic.

---

## Layer Architecture

```mermaid
graph TB
    subgraph "Layer 4: Main Loop / Signal Layer"
        Main[Main Loop<br/>arduino.ino]
    end
    
    subgraph "Layer 3: Application Layer"
        App[DeskApp<br/>DeskController.cpp/h]
    end
    
    subgraph "Layer 2: Hardware Abstraction Layer"
        HAL[HAL Module<br/>HAL.cpp/h]
    end
    
    subgraph "Layer 1: Hardware"
        HW[Physical Hardware<br/>GPIO, PWM, Timers]
    end
    
    Main -->|APP_Task| App
    App -->|Motor Commands| Main
    Main -->|HAL_read/write| HAL
    HAL -->|digitalWrite, analogWrite| HW
    
    style Main fill:#FFE6CC
    style App fill:#90EE90
    style HAL fill:#87CEEB
    style HW fill:#FFE6E6
```

### Layer 1: Hardware (External)

**Responsibility:** Provide physical inputs/outputs to the software via GPIO, PWM, and ADC signals.

**Description:** Hardware composition and wiring are defined at the system level; see System Architecture for component details.

---

### Layer 2: Hardware Abstraction Layer (HAL)

**Responsibility:** Encapsulate all hardware-specific code; provide hardware-independent API to upper layers.

**Files:** `HAL.h`, `HAL.cpp`, `PinConfig.h`

**Key Interfaces:** Hardware I/O functions for buttons, limits, motor, LEDs, and time services. Detailed signatures and data types are defined in the Detailed Design Specification.

**Description:** HAL is the only layer with direct access to Arduino libraries (digitalWrite, analogWrite, millis). Pin assignments and concrete data types are specified in the Detailed Design.

---

### Layer 3: Application Layer (DeskApp)

**Responsibility:** Implement desk control logic, state machine, and safety rules; hardware-agnostic.

**File:** `DeskController.h`, `DeskController.cpp`

**Description:** DeskApp contains the state machine and business logic. It is fully testable with mock inputs/outputs.

**Interfaces:** Function signatures and data structures are defined in the Detailed Design Specification.

---

### Layer 4: Main Loop / Signal Layer

**Responsibility:** Poll hardware via HAL, populate input structure, call APP_Task, execute output commands.

**File:** `arduino.ino`

**Description:** Main loop orchestrates I/O coordination and schedules the application task. Implementation details are specified in the Detailed Design.

---

## State Machine Design

### State Diagram

```mermaid
stateDiagram-v2
    [*] --> IDLE: Power On / APP_Init()
    
    IDLE --> MOVING_UP: UP pressed AND NOT upper_limit
    IDLE --> MOVING_DOWN: DOWN pressed AND NOT lower_limit
    IDLE --> IDLE: Both buttons pressed (conflicting input)
    
    MOVING_UP --> IDLE: UP released
    MOVING_UP --> IDLE: upper_limit reached
    MOVING_UP --> FAULT: Unexpected error
    
    MOVING_DOWN --> IDLE: DOWN released
    MOVING_DOWN --> IDLE: lower_limit reached
    MOVING_DOWN --> FAULT: Unexpected error
    
    FAULT --> IDLE: Fault cleared (power cycle)
    
    note right of IDLE
        Motor: STOP
        LED: IDLE (steady green)
    end note
    
    note right of MOVING_UP
        Motor: UP (speed 255)
        LED: ACTIVE (blinking)
    end note
    
    note right of MOVING_DOWN
        Motor: DOWN (speed 255)
        LED: ACTIVE (blinking)
    end note
    
    note right of FAULT
        Motor: STOP
        LED: ERROR (steady red)
    end note
```

### State Transition Table

| Current State | Input Condition | Next State | Motor Command | LED Status |
|--------------|----------------|-----------|---------------|-----------|
| IDLE | UP pressed AND NOT upper_limit | MOVING_UP | UP, speed=255 | ACTIVE |
| IDLE | DOWN pressed AND NOT lower_limit | MOVING_DOWN | DOWN, speed=255 | ACTIVE |
| IDLE | Both buttons pressed | IDLE | STOP | IDLE |
| IDLE | No buttons pressed | IDLE | STOP | IDLE |
| MOVING_UP | UP still pressed AND NOT upper_limit | MOVING_UP | UP, speed=255 | ACTIVE |
| MOVING_UP | UP released | IDLE | STOP | IDLE |
| MOVING_UP | upper_limit reached | IDLE | STOP | IDLE |
| MOVING_DOWN | DOWN still pressed AND NOT lower_limit | MOVING_DOWN | DOWN, speed=255 | ACTIVE |
| MOVING_DOWN | DOWN released | IDLE | STOP | IDLE |
| MOVING_DOWN | lower_limit reached | IDLE | STOP | IDLE |
| Any State | Unexpected error | FAULT | STOP | ERROR |

### State Entry/Exit Actions

**IDLE State:**
- **Entry:** Motor STOP, LED IDLE
- **In-State:** Monitor button inputs
- **Exit:** None

**MOVING_UP State:**
- **Entry:** Motor UP (speed=255), LED ACTIVE
- **In-State:** Monitor button release and upper limit
- **Exit:** Motor STOP

**MOVING_DOWN State:**
- **Entry:** Motor DOWN (speed=255), LED ACTIVE
- **In-State:** Monitor button release and lower limit
- **Exit:** Motor STOP

**FAULT State:**
- **Entry:** Motor STOP, LED ERROR, log fault
- **In-State:** System halted
- **Exit:** Requires power cycle to clear

---

## Interaction Diagrams

Interaction sequences and data flow diagrams are maintained in the Detailed Design Specification to keep implementation-level behavior in a single location.

---

## Requirements Traceability Matrix

### System Requirements → Architecture Elements

| System Req | Software Req | Architecture Element | Layer | Verification |
|-----------|-------------|---------------------|-------|--------------|
| SysReq-001 | SWReq-005, SWReq-006 | Limit sensor input + Safety logic | APP, HAL | Unit test with mock sensors |
| SysReq-002 | SWReq-001, SWReq-002, SWReq-008 | Button input handler + State transitions | APP, HAL | Timing test with mock GPIO |
| SysReq-003 | SWReq-003 | Button release detection + Motor stop | APP, HAL | Unit test with mock timer |
| SysReq-004 | SWReq-008 | Control loop timing (≤50ms cycle) | Main Loop | Loop timing measurement |
| SysReq-005 | SWReq-004 | Conflicting input detection | APP | Unit test with both buttons |
| SysReq-006 | — | Motor speed control (constant 255) | HAL | (Future: PWM ramping) |
| SysReq-007 | SWReq-005, SWReq-006 | Limit override logic | APP | Unit test with limit active |
| SysReq-008 | SWReq-010 | State transition logging | APP | Diagnostic log verification |

### Software Requirements → Architecture Functions

| Software Req | Function/Module | API | Layer |
|-------------|----------------|-----|-------|
| SWReq-001 | Button UP handler | `HAL_readButton(BUTTON_UP)` | HAL |
| SWReq-002 | Button DOWN handler | `HAL_readButton(BUTTON_DOWN)` | HAL |
| SWReq-003 | Button release handler | `HAL_readButton()` + debounce | HAL |
| SWReq-004 | Conflicting input check | Safety logic in `APP_Task()` | APP |
| SWReq-005 | Upper limit check | `HAL_readLimitSensor(LIMIT_UPPER)` | HAL |
| SWReq-006 | Lower limit check | `HAL_readLimitSensor(LIMIT_LOWER)` | HAL |
| SWReq-007 | State machine | `APP_Task()` state transitions | APP |
| SWReq-008 | Control loop | `loop()` execution | Main |
| SWReq-009 | Debouncing | `HAL_readButton()` internal logic | HAL |
| SWReq-010 | Diagnostics | `APP_GetState()`, state log | APP |

---

## Interface Specifications

Detailed interface definitions, pin assignments, and timing budgets are specified in the Detailed Design Specification.

## Architectural Decisions

### AD-001: Layered Architecture
**Decision:** Use 4-layer architecture (Hardware, HAL, APP, Main Loop).  
**Rationale:** Enables unit testing of application logic without hardware; supports future platform portability.  
**Traceability:** All software requirements (SWReq-001 through SWReq-010).

### AD-002: State Machine Pattern
**Decision:** Implement control logic as explicit state machine with 4 states.  
**Rationale:** Provides deterministic, verifiable behavior; simplifies safety analysis.  
**Traceability:** SWReq-007, SysReq-002, SysReq-003.

### AD-003: Input/Output Structures
**Decision:** Use C structures for APP_Task interface instead of individual parameters.  
**Rationale:** Simplifies unit testing; easier to extend with new signals; clear API boundary.  
**Traceability:** SWReq-001 through SWReq-006.

### AD-004: HAL Owns All Hardware Access
**Decision:** Only HAL layer calls Arduino functions (digitalWrite, analogWrite, millis).  
**Rationale:** Enables mock-based testing; single point for hardware dependencies.  
**Traceability:** All software requirements.

### AD-005: Constant Motor Speed (No Ramping)
**Decision:** Motor speed fixed at 255 (full speed); no acceleration/deceleration profile.  
**Rationale:** Simplifies initial implementation; meets system requirements; ramping can be added later.  
**Traceability:** SysReq-004, SysReq-006.

### AD-006: Debouncing in HAL
**Decision:** Button debouncing implemented inside HAL_readButton().  
**Rationale:** Isolates timing complexity from APP layer; reusable across projects.  
**Traceability:** SWReq-009.

### AD-007: Limit Sensors Override Buttons
**Decision:** Limit sensors take priority over button inputs in safety logic.  
**Rationale:** Prevents mechanical over-travel; safety-critical requirement.  
**Traceability:** SysReq-007, SWReq-005, SWReq-006.

### AD-008: Non-Blocking Main Loop with Time-Based Scheduler
**Decision:** Main loop is non-blocking and executes application control logic at 4 Hz (every 250 ms).  
**Rationale:** 
- Prevents blocking delays that can impair safety-critical button response
- 4 Hz scheduler provides predictable timing for application task invocation
- Arduino loop() is called frequently; explicit timing check prevents excessive application task calls
- Allows I/O operations (HAL calls) to complete without waiting
**Traceability:** Foundational to responsive button handling (SysReq-002: ≤1 sec response) and halt timing (SysReq-003: ≤500 ms halt).

---

## Design Constraints

1. **Memory:** Arduino UNO has 2 KB SRAM; minimize global variables
2. **Timing:** 
    - Meet system-level response and halt timing constraints
    - No blocking delays in main loop; utilize non-blocking scheduler (see AD-008)
3. **Safety:** All motor commands must be fail-safe (default to STOP)
4. **Testability:** APP layer must be testable without Arduino hardware

---

## Future Enhancements (Out of Scope)

- PWM ramping for smooth acceleration/deceleration
- Position sensing and closed-loop control
- EEPROM storage for height presets
- Serial command interface for diagnostics
- Watchdog timer for fault detection


