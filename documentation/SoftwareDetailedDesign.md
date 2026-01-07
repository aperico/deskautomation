# Software Detailed Design

This document provides the detailed design for the Automated Mechanical Desk Lift software. It is intended for developers, testers, and stakeholders to understand the implementation, rationale, and compliance with safety and industry standards.

---

## Navigation

- [System Use Cases](SystemUseCases.md)
- [Software Requirements](SoftwareRequirements.md)
- [Software Architecture](SoftwareArchitecture.md)
- [Traceability Matrix](TraceabilityMatrix.md)
- [Software Test Cases Specification](SoftwareTestCasesSpecification.md)

---

## Assumptions

The following assumptions, derived from SoftwareRequirements.md, are reflected in the design and test cases:

- System hardware (ECU, motor driver, indicator LEDs, limit switches, buttons) is available and functions as specified.
- The user is trained to operate the desk safely.
- The system operates in a typical office environment with standard power supply.
- Regulatory and safety standards for motorized furniture are met.
- Up/Down buttons and limit switches are functional and reliably detected.
- Fault detection and error logic are implemented and reliable.
- Power supply is stable and system can detect power loss.
  Any change in assumptions may require design or test updates.

---

## Design Constraints

- Initial operation without limit switches
- Motor must stop on reset or fault
- Direction changes require a stop before reversal
- Motor must never start automatically on power-up
- Must operate on Arduino UNO or compatible ECU
- Motor driver: L298N
- Limit switches and buttons: digital inputs
- Indicator LEDs: digital outputs
- Power supply: regulated, office environment
- Timing: non-blocking, responsive (<100ms reaction)
- Regulatory: must comply with ISO 25119, ASPICE

---

## Design Rationale

- State machine chosen for clarity, safety, and traceability
- Non-blocking loop ensures responsiveness and safety
- Modular decomposition supports maintainability and extensibility
- Structured data (inputs/outputs) enables testability and clear interfaces

---

## Module Decomposition

### 1. PinConfig

- **File:** PinConfig.h
- **Purpose:** Defines all hardware pin assignments for LEDs, buttons, and motor driver.
- **Key Data:** Pin numbers for each hardware function.
- **Usage:** Included by HAL and application logic for hardware abstraction.

### 2. Hardware Abstraction Layer (HAL)

- **Files:** HAL.h, HAL.cpp
- **Purpose:** Abstracts direct hardware access and provides initialization and control functions for LEDs, buttons, and motor driver.
- **Key Functions:**
  - `HAL_init()`: Initializes hardware components.
  - `HAL_setLED(pin, state)`: Controls LED state.
  - `HAL_readButton(pin)`: Reads button state.
  - `HAL_setMotor(direction, speed)`: Controls motor driver.
- **Interfaces:** Exposes functions to application logic. Uses pin assignments from PinConfig.h.

### 3. Application Logic (DeskController)

- **Files:** DeskController.h, DeskController.cpp
- **Purpose:** Implements the main state machine, control logic, and error handling.
- **Key Data Structures:**
  - `DeskAppInputs_t`: Struct for all input signals (button states, limit switches).
  - `DeskAppOutputs_t`: Struct for all output signals (motor commands, LED states, error flags).
- **Key Functions:**
  - `DeskApp_task_init(inputs, outputs)`: Initializes input/output structs and sets system to IDLE.
  - `DeskApp_task(inputs, outputs)`: Main control function; processes inputs, updates outputs, manages state transitions.
- **State Machine:** States: IDLE, MOVING_UP, MOVING_DOWN, DWELL, ERROR. Transitions: Based on button presses, limit switches, faults, and timeouts.
- **Error Handling:** Emergency stop on faults or both buttons pressed. Error state latched until safe conditions detected.

### 4. Main Entry Point

- **File:** arduino.ino
- **Purpose:** Initializes system, runs main loop, integrates HAL and application logic.
- **Control Flow:**
  - Setup: Initializes hardware and application logic.
  - Loop: Reads inputs, calls DeskApp_task, updates outputs via HAL.

---

## Data Structures

### DeskAppInputs_t

- `btUPPressed`: Boolean, Up button state
- `btDOWNPressed`: Boolean, Down button state
- `upperLimitActive`: Boolean, upper limit switch state
- `lowerLimitActive`: Boolean, lower limit switch state

### DeskAppOutputs_t

- `moveUp`: Boolean, command to move up
- `moveDown`: Boolean, command to move down
- `stop`: Boolean, command to stop motor
- `error`: Boolean, error state flag

---

## Interfaces

### HAL <-> Application Logic

Application logic interacts with the HAL using the following interface functions (as defined in `HAL.h`):

```cpp
// PinConfig.h
extern const int ERROR_LED;
extern const int LED_LEFT_PIN;
extern const int LED_RIGHT_PIN;
extern const int BUTTON_UP_PIN;
extern const int BUTTON_DOWN_PIN;
extern const int IN1;
extern const int IN2;
extern const int ENA;

// HAL.h
void HAL_init();
void HAL_SetErrorLED(bool state);
void HAL_SetMovingUpLED(bool state);
void HAL_SetMovingDownLED(bool state);
void HAL_SetWarningLED(bool state);
void HAL_SetPowerLED(bool state);
void HAL_SetMotorDirection(int direction);
void HAL_MoveUp(const unsigned char speed);
void HAL_MoveDown(const unsigned char speed);
void HAL_StopMotor();
void HAL_BlinkErrorLED();
void HAL_BlinkUPLED();
void HAL_BlinkDOWNLED();
bool HAL_GetMovingDownLED();
bool HAL_GetMovingUpLED();
bool HAL_GetErrorLED();
bool HAL_readButton(int pin);
bool HAL_debounceButton(const int pin, DebounceState &state, const unsigned long debounceDelay);
```

### Application Logic <-> Main Loop

The main loop passes input and output structs to the application logic (as defined in `DeskController.h`):

```cpp
typedef struct {
    bool btUPPressed;
    bool btDOWNPressed;
    bool upperLimitActive;
    bool lowerLimitActive;
} DeskAppInputs_t;

typedef struct {
    bool moveUp;
    bool moveDown;
    bool stop;
    bool error;
} DeskAppOutputs_t;

void DeskApp_task_init(DeskAppInputs_t* inputs, DeskAppOutputs_t* outputs);
DeskAppTask_Return_t DeskApp_task(const DeskAppInputs_t* inputs, DeskAppOutputs_t* outputs);
```

### Data Types

- All pin assignments: `int`
- Button/limit states: `bool`
- Motor direction: `int` (e.g., 0 = stop, 1 = up, -1 = down)
- Motor speed: `unsigned char` (0–255 for PWM)
- Debounce state: `DebounceState` struct (for button debouncing)

---

## Application States and State Transitions

The application uses a simple state machine to manage desk movement and safety. Below are the main states and their transitions:

### States

- **IDLE**: Desk is stationary, waiting for user input.
- **MOVING_UP**: Desk is moving upward.
- **MOVING_DOWN**: Desk is moving downward.
- **DWELL**: Waits before allowing reversal.
- **ERROR**: A fault or unsafe condition has occurred; movement is disabled.

### State Transitions

- **IDLE → MOVING_UP**: User presses the Up button (and not at upper limit).
- **IDLE → MOVING_DOWN**: User presses the Down button (and not at lower limit).
- **MOVING_UP → IDLE**: Up button released, timeout occurs, or upper limit reached.
- **MOVING_DOWN → IDLE**: Down button released, timeout occurs, or lower limit reached.
- **MOVING_UP or MOVING_DOWN → ERROR**: Fault detected (e.g., overcurrent, both limits active).
- **ERROR → IDLE**: Error condition cleared (e.g., reset or safe state detected).

See [Software Detailed Design](SoftwareDetailedDesign.md#state-machine-diagram) for the full state machine and transitions.

---

## Control Flow

1. **Initialization:**
   - PinConfig.h sets hardware mapping.
   - HAL_init() initializes hardware.
   - DeskApp_task_init() sets system to IDLE.
2. **Main Loop:**
   - Read button and limit switch states via HAL.
   - Populate DeskAppInputs_t.
   - Call DeskApp_task(inputs, outputs).
   - Update hardware outputs via HAL based on DeskAppOutputs_t.
3. **State Machine Logic:**
   - IDLE: Waits for user input.
   - MOVING_UP: Moves desk up if allowed.
   - MOVING_DOWN: Moves desk down if allowed.
   - DWELL: Waits before allowing reversal.
   - ERROR: Disables movement, indicates fault.
4. **Error Handling:**
   - Emergency stop on faults or both buttons pressed.
   - Error state latched until all inputs and limits are safe.

---

## Error Handling Details

- Emergency stop: triggers on both buttons pressed or fault
- Error state: disables movement, activates error LED
- Recovery: only via power cycle, all outputs reset
- Faults detected: overcurrent, both limits active, timeout
- Error handling logic is centralized in DeskController

---

## Testability Hooks

- HAL can be mocked for host-based unit tests
- DeskAppInputs_t and DeskAppOutputs_t structs allow injection and observation of test data
- Diagnostic outputs (e.g., error flags) can be logged or monitored
- All state transitions and error conditions are covered by unit tests

---

## Maintainability and Extensibility Notes

- New states or transitions: add to DeskController state machine and update diagrams
- Hardware changes: update PinConfig.h and HAL only
- Additional safety features: extend error handling and safety analysis sections
- Future enhancements: current sensing, presets, calibration, remote control

---

## Traceability to Software Requirements

This section maps each design module, feature, and behavioral logic to the corresponding requirements in SoftwareRequirements.md. This ensures every design element is justified and traceable.

| Design Element / Module                 | Requirement ID(s)   | Description / Rationale                                          |
| --------------------------------------- | ------------------- | ---------------------------------------------------------------- |
| PinConfig.h (hardware mapping)          | SR-01, SR-02        | Hardware abstraction, pin assignments for safety and flexibility |
| HAL (hardware abstraction)              | SR-01, SR-03        | Safe hardware control, error handling, modularity                |
| DeskController (state machine logic)    | SR-04, SR-05, SR-06 | Implements state transitions, error handling, and control logic  |
| DeskAppInputs_t / DeskAppOutputs_t      | SR-02, SR-04        | Structured data for inputs/outputs, supports testability         |
| Main Loop (arduino.ino integration)     | SR-01, SR-07        | Non-blocking, event-driven control, integrates all modules       |
| Error Handling (emergency stop, faults) | SR-03, SR-06        | Ensures safety, disables movement on fault                       |
| Dwell Timing / Non-blocking Design      | SR-07               | Responsive, safe reversal, prevents unsafe direction changes     |
| Testability (unit tests, coverage)      | SR-08               | All logic is unit tested and mapped to requirements              |

Refer to [Software Requirements](SoftwareRequirements.md) for full requirement text and rationale. All design elements are reviewed for compliance and traceability.

---

## Safety Analysis and Rationale

This section provides explicit safety analysis and rationale for each safety-related requirement, supporting ASPICE CL2+ compliance.

| Safety Requirement                  | Rationale                                                                                           | Safety Mechanism / Design Feature                                                                                              |
| ----------------------------------- | --------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------ |
| SR-04: Emergency Stop               | Prevents unsafe movement in case of simultaneous button press or fault.                             | Immediate deactivation of all movement outputs; system enters ERROR state; error LED activated; recovery only via power cycle. |
| SR-05: Visual Feedback              | Ensures user is always aware of system state, including errors.                                     | LEDs indicate IDLE, MOVING_UP, MOVING_DOWN, ERROR; error LED is prominent during faults.                                       |
| SR-08: Error Detection and Recovery | Detects hazardous conditions (e.g., both limits active, overcurrent) and prevents further movement. | System enters ERROR state; movement disabled; error LED activated; recovery via power cycle.                                   |

All safety mechanisms are implemented in DeskController and HAL modules, with test cases verifying correct behavior under fault and error conditions. Safety features are designed to meet regulatory and industry standards for motorized furniture.

---

## Timing and Non-blocking Design

- Main loop is non-blocking; all logic is event-driven and responsive.
- Dwell timing managed via timestamp comparison in DeskController.cpp.

---

## Safety Features

- Motor stops on reset, fault, or both buttons pressed.
- Direction changes require a stop and dwell before reversal.
- Motor never starts automatically on power-up.
- Error state disables all movement until cleared.

---

## Extensibility

- PinConfig.h and HAL support easy hardware changes.
- Application logic is modular for future enhancements (limit switches, current sensing, presets).

---

## Testability

- All logic is unit tested using host-based tests (g++), mapped to requirements and use cases.

---

## Diagrams

### Sequence Diagram (Desk Movement)

```mermaid
sequenceDiagram
    participant User
    participant ECU
    participant DeskController
    participant HAL
    participant MotorDriver
    User->>ECU: Press Up/Down Button
    ECU->>HAL: HAL_debounceButton (read debounced button)
    HAL->>ECU: Debounced Button State
    ECU->>DeskController: DeskApp_task(inputs, outputs)
    DeskController->>HAL: HAL_MoveUp / HAL_MoveDown / HAL_StopMotor
    HAL->>MotorDriver: Set Motor Pins/Speed
    DeskController->>HAL: HAL_SetMovingUpLED / HAL_SetMovingDownLED / HAL_SetErrorLED
    HAL->>ECU: LED State Feedback
    MotorDriver-->>HAL: Status
```

### State Machine Diagram

```mermaid
stateDiagram-v2
    [*] --> IDLE
    IDLE --> MOVING_UP: Up button pressed (debounced)
    IDLE --> MOVING_DOWN: Down button pressed (debounced)
    MOVING_UP --> IDLE: Up button released / timeout / upper limit
    MOVING_DOWN --> IDLE: Down button released / timeout / lower limit
    MOVING_UP --> ERROR: Fault detected
    MOVING_DOWN --> ERROR: Fault detected
    IDLE --> ERROR: Fault detected
    ERROR --> IDLE: Error cleared
    MOVING_UP --> DWELL: Down button pressed after up released
    MOVING_DOWN --> DWELL: Up button pressed after down released
    DWELL --> MOVING_UP: Dwell time elapsed, up pressed
    DWELL --> MOVING_DOWN: Dwell time elapsed, down pressed
    DWELL --> IDLE: No button pressed after dwell
```

### Class Diagram

```mermaid
classDiagram
    class PinConfig {
        <<static>>
        +int ERROR_LED
        +int LED_LEFT_PIN
        +int LED_RIGHT_PIN
        +int BUTTON_UP_PIN
        +int BUTTON_DOWN_PIN
        +int IN1
        +int IN2
        +int ENA
    }
    class DebounceState {
        +lastState: bool
        +lastDebounceTime: unsigned long
    }
    class HAL {
        +HAL_Init()
        +HAL_ProcessAppState(ret, outputs)
        +HAL_SetErrorLED(state)
        +HAL_SetWarningLED(state)
        +HAL_SetPowerLED(state)
        +HAL_SetMotorDirection(state)
        +HAL_SetMovingDownLED(state)
        +HAL_SetMovingUpLED(state)
        +HAL_GetMovingDownLED()
        +HAL_GetMovingUpLED()
        +HAL_GetErrorLED()
        +HAL_MoveUp(speed)
        +HAL_MoveDown(speed)
        +HAL_StopMotor()
        +HAL_BlinkErrorLED()
        +HAL_BlinkUPLED()
        +HAL_BlinkDOWNLED()
        +HAL_debounceButton(pin, state, debounceDelay)
    }
    class DeskAppInputs_t {
        +bool btUPPressed
        +bool btDOWNPressed
        +bool upperLimitActive
        +bool lowerLimitActive
    }
    class DeskAppOutputs_t {
        +bool moveUp
        +bool moveDown
        +bool stop
        +bool error
    }
    class DeskController {
        +DeskApp_task_init(inputs, outputs)
        +DeskApp_task(inputs, outputs)
        -process_move_up()
        -process_move_down()
        -process_stop()
        -process_dwell()
        -now_ms()
        -appState: AppState_t
        -dwellStartMs: unsigned long
    }
    class DeskAppTask_Return_t {
        <<enum>>
        APP_TASK_SUCCESS
        APP_TASK_ERROR
    }
    class AppState_t {
        <<enum>>
        APP_STATE_IDLE
        APP_STATE_MOVE_UP
        APP_STATE_MOVE_DOWN
        APP_STATE_DWELL
        APP_STATE_ERROR
    }
    DeskAppInputs_t <.. DeskController
    DeskAppOutputs_t <.. DeskController
    DeskAppTask_Return_t <.. DeskController
    AppState_t <.. DeskController
```

---

## Compliance Statement

This design complies with ISO 25119 (Safety-related parts of control systems for agricultural and forestry machinery) and ASPICE (Automotive SPICE) for software engineering best practices, including traceability, safety analysis, and testability.

---

## Open Issues and Future Work

- Add current sensing for enhanced fault detection
- Implement remote control interface
- Expand safety analysis for new hazards
- Integrate calibration and presets
- Review for additional regulatory requirements

---

## Maintainability Notes

- Detailed design is subject to change with future requirements or hardware updates.
- All design elements follow industry standards for embedded and safety-critical systems.

---

*For questions or suggestions, open an issue or contact the project maintainers.*
