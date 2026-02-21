# Interaction Diagrams

**Document ID:** InteractionDiag-Desk-001  
**System Name:** Automated Mechanical Desk Lift System  
**Date:** February 21, 2026  
**Version:** 1.0  
**Status:** Draft  
**Derived From:** [06_DetailedDesign.md](06_DetailedDesign.md)

---

## Purpose

This document provides detailed sequence diagrams and data flow diagrams that illustrate the runtime interactions between system components in the Standing Desk Automation System. These diagrams complement the detailed design specification and help visualize how modules communicate during various operational scenarios.

---

## Sequence Diagrams

### Sequence 1: Normal Desk Raise (SysReq-002, SysReq-004)

```mermaid
sequenceDiagram
    autonumber
    participant User
    participant Main as Main Loop
    participant App as DeskApp
    participant HAL
    participant Motor as Motor Hardware
    
    User->>Main: Press UP button
    Note over Main: loop() iteration start
    Main->>HAL: HAL_readButton(BUTTON_UP)
    HAL-->>Main: true
    Main->>HAL: HAL_readLimitSensor(LIMIT_UPPER)
    HAL-->>Main: false (not at limit)
    Note over Main: Populate AppInput_t
    Main->>App: APP_Task(&inputs, &outputs)
    Note over App: State: IDLE -> MOVING_UP
    App-->>Main: motor_cmd=UP, speed=255
    Main->>HAL: HAL_setMotor(MOTOR_UP, 255)
    HAL->>Motor: analogWrite(PWM_PIN, 255)
    Note over Motor: Desk begins rising<br/>Time: +100ms from button press
    
    Note over Main: Subsequent loop iterations
    loop While UP held
        Main->>HAL: HAL_readButton(BUTTON_UP)
        HAL-->>Main: true
        Main->>App: APP_Task(&inputs, &outputs)
        Note over App: State: MOVING_UP (no change)
        App-->>Main: motor_cmd=UP, speed=255
        Main->>HAL: HAL_setMotor(MOTOR_UP, 255)
    end
    
    User->>Main: Release UP button
    Main->>HAL: HAL_readButton(BUTTON_UP)
    HAL-->>Main: false
    Main->>App: APP_Task(&inputs, &outputs)
    Note over App: State: MOVING_UP -> IDLE
    App-->>Main: motor_cmd=STOP, speed=0
    Main->>HAL: HAL_setMotor(MOTOR_STOP, 0)
    HAL->>Motor: analogWrite(PWM_PIN, 0)
    Note over Motor: Desk stops<br/>Time: +50ms from button release
```

---

### Sequence 2: Emergency Halt via Button Release (SysReq-003)

```mermaid
sequenceDiagram
    autonumber
    participant User
    participant Main as Main Loop
    participant App as DeskApp
    participant HAL
    participant Motor
    
    Note over App,Motor: Desk is moving up<br/>State: MOVING_UP
    
    User->>Main: Release UP button
    Note over Main: Detect button release within 50ms
    Main->>HAL: HAL_readButton(BUTTON_UP)
    HAL-->>Main: false (released)
    Main->>App: APP_Task(&inputs, &outputs)
    Note over App: State: MOVING_UP -> IDLE<br/>Safety transition
    App-->>Main: motor_cmd=STOP, speed=0
    Main->>HAL: HAL_setMotor(MOTOR_STOP, 0)
    HAL->>Motor: digitalWrite(EN_PIN, LOW)
    Note over Motor: Motor stops immediately<br/>Total time: <=50ms
    Note over Main: LED changes to IDLE
```

---

### Sequence 3: Conflicting Button Inputs (SysReq-005, SWReq-004)

```mermaid
sequenceDiagram
    autonumber
    participant User
    participant Main as Main Loop
    participant App as DeskApp
    participant HAL
    participant Motor
    
    Note over App: State: IDLE
    User->>Main: Press UP and DOWN simultaneously
    Main->>HAL: HAL_readButton(BUTTON_UP)
    HAL-->>Main: true
    Main->>HAL: HAL_readButton(BUTTON_DOWN)
    HAL-->>Main: true
    Note over Main: Both buttons pressed!
    Main->>App: APP_Task(&inputs, &outputs)
    Note over App: Detect conflicting inputs<br/>Safety rule: ignore command
    Note over App: State remains IDLE
    App-->>Main: motor_cmd=STOP, speed=0
    Main->>HAL: HAL_setMotor(MOTOR_STOP, 0)
    Note over Motor: Motor remains stopped<br/>System safe
```

---

### Sequence 4: Upper Limit Protection (SysReq-007, SWReq-005)

```mermaid
sequenceDiagram
    autonumber
    participant User
    participant Main as Main Loop
    participant App as DeskApp
    participant HAL
    participant Motor
    participant Sensor as Limit Sensor
    
    Note over App,Motor: Desk moving up<br/>State: MOVING_UP
    Note over Sensor: Desk approaches upper limit
    
    Sensor->>HAL: Limit sensor triggered
    Main->>HAL: HAL_readLimitSensor(LIMIT_UPPER)
    HAL-->>Main: true (limit reached)
    Note over Main: UP button still pressed
    Main->>App: APP_Task(&inputs, &outputs)
    Note over App: Safety override:<br/>Limit takes priority over button
    Note over App: State: MOVING_UP -> IDLE
    App-->>Main: motor_cmd=STOP, speed=0
    Main->>HAL: HAL_setMotor(MOTOR_STOP, 0)
    HAL->>Motor: digitalWrite(EN_PIN, LOW)
    Note over Motor: Motor stops at limit<br/>No over-travel
```

---

### Sequence 5: Full System Initialization (SysReq-001, SWReq-007)

```mermaid
sequenceDiagram
    autonumber
    participant Power
    participant Main as Main Loop
    participant App as DeskApp
    participant HAL
    participant HW as Hardware
    
    Power->>Main: Power on / Reset
    Main->>HAL: HAL_init()
    HAL->>HW: pinMode() for all pins
    HAL->>HW: Set safe defaults
    Note over HW: Motor: STOP<br/>LED: OFF
    HAL-->>Main: Init complete
    
    Main->>App: APP_Init()
    Note over App: Initialize state machine<br/>State: IDLE
    Note over App: Clear diagnostics
    App-->>Main: Init complete
    
    Note over Main: Enter main loop()
    Main->>HAL: HAL_setLED(LED_IDLE)
    Note over Main: System ready for user input
```

---

## Data Flow Diagram

### Data Flow Overview

```mermaid
graph LR
    subgraph Inputs
        BTN[Buttons<br/>UP/DOWN]
        LIM[Limit Sensors<br/>Upper/Lower]
    end
    
    subgraph HAL_Layer[HAL Layer]
        DB[Debounce<br/>20ms]
        READ[Read GPIO]
    end
    
    subgraph APP_Layer[APP Layer]
        SM[State Machine<br/>IDLE/UP/DOWN/FAULT]
        SAFE[Safety Logic<br/>Conflict/Limit Check]
    end
    
    subgraph Outputs
        MOT[Motor Driver<br/>PWM + Direction]
        LED[Status LED]
    end
    
    BTN --> READ
    LIM --> READ
    READ --> DB
    DB -->|AppInput_t| SM
    SM --> SAFE
    SAFE -->|AppOutput_t| HAL_Layer
    HAL_Layer --> MOT
    HAL_Layer --> LED
    
    style HAL_Layer fill:#87CEEB
    style APP_Layer fill:#90EE90
    style Inputs fill:#FFE6E6
    style Outputs fill:#FFD700
```

---

## Interaction Traceability

| Diagram | Requirements Covered | Purpose |
|---------|----------------------|---------|
| Sequence 1: Normal Desk Raise | SysReq-002, SysReq-004 | Shows motor activation within 1 sec; full stroke timing |
| Sequence 2: Emergency Halt | SysReq-003 | Demonstrates 500 ms halt on button release |
| Sequence 3: Conflicting Buttons | SysReq-005, SWReq-004 | Safety behavior when simultaneous buttons pressed |
| Sequence 4: Limit Protection | SysReq-007, SWReq-005/006 | Motor halt at travel limits even with button held |
| Sequence 5: System Init | SysReq-001, SWReq-007 | Safe startup state machine initialization |
| Data Flow Diagram | All | Overall module communication and data structure flow |

---

## Key Observations

### Component Interactions

1. **Main Loop**: Central orchestrator that reads inputs and writes outputs every ≤50 ms loop cycle
2. **HAL Layer**: Abstracts hardware details (GPIO, PWM, timing); provides clean interfaces to APP layer
3. **DeskApp Layer**: Implements state machine and safety logic; hardware-independent
4. **Debouncer**: Filters mechanical button noise (20 ms window) before feeding to state machine
5. **Safety Logic**: Priority hierarchy: Limits > Conflicting inputs > Normal commands

### Data Structures

- **AppInput_t**: Carries debounced button states, limit sensor states, motor current, and timestamp from HAL to APP
- **AppOutput_t**: Carries motor direction/speed and LED status from APP back to HAL

### Critical Timing Paths

- **Button Press → Motor Activation**: ~90 ms (well within 1 sec SysReq-002)
- **Button Release → Motor Stop**: ~88 ms (well within 500 ms SysReq-003)
- **Control Loop Execution**: ~15 ms typical (well within 50 ms budget)

---

## Approvals

- **System Architect:** [Pending]
- **Software Lead:** [Pending]
- **Project Manager:** [Pending]
