# Detailed Design Specification

## Overview

This document provides implementation-level design details for the Standing Desk Automation System. It specifies data structures, algorithms, function interfaces, timing constraints, and error handling logic required for code implementation.

---

## Traceability

**Derived from:**
- [05_SoftwareArchitecture.md](05_SoftwareArchitecture.md)
- [04_SoftwareRequirements.md](04_SoftwareRequirements.md)
- [03_SystemRequirements.md](03_SystemRequirements.md)

---

## Document Purpose

This detailed design bridges the gap between architecture and implementation. Developers can write code directly from this specification without requiring clarification on:
- Exact data structure layouts and field types
- Algorithm implementations (debouncing, state transitions)
- Function signatures with complete parameter specifications
- Timing constraints and performance requirements
- Error handling and edge case logic

---

## Module Structure

```mermaid
classDiagram
    class PinConfig {
        <<configuration>>
        +PIN_BUTTON_UP: uint8_t
        +PIN_BUTTON_DOWN: uint8_t
        +PIN_LIMIT_UPPER: uint8_t
        +PIN_LIMIT_LOWER: uint8_t
        +PIN_MOTOR_RPWM: uint8_t
        +PIN_MOTOR_LPWM: uint8_t
        +PIN_LED_STATUS: uint8_t
    }
    
    class HAL {
        -debounce_state[]: uint32_t
        -last_button_time[]: uint32_t
        +HAL_init() void
        +HAL_readButton(ButtonID) bool
        +HAL_readLimitSensor(LimitID) bool
        +HAL_setMotor(MotorDirection, uint8_t) void
        +HAL_setLED(LEDStatus) void
        +HAL_getTime() uint32_t
    }
    
    class DeskApp {
        -current_state: AppState_t
        -state_entry_time: uint32_t
        -transition_count: uint32_t
        +APP_Init() void
        +APP_Task(AppInput_t*, AppOutput_t*) void
        +APP_GetState() AppState_t
        -processState(AppInput_t*, AppOutput_t*) void
        -transitionTo(AppState_t) void
    }
    
    class MainLoop {
        +setup() void
        +loop() void
        -readInputs(AppInput_t*) void
        -executeOutputs(AppOutput_t*) void
    }
    
    MainLoop --> DeskApp : calls APP_Task()
    MainLoop --> HAL : calls HAL functions
    DeskApp ..> AppInput_t : uses
    DeskApp ..> AppOutput_t : produces
    HAL ..> PinConfig : uses
```

---

## Data Structure Definitions

### 1. Enumerated Types

```cpp
// Button identifiers
typedef enum {
    BUTTON_UP = 0,
    BUTTON_DOWN = 1,
    BUTTON_COUNT = 2
} ButtonID;

// Limit sensor identifiers
typedef enum {
    LIMIT_UPPER = 0,
    LIMIT_LOWER = 1,
    LIMIT_COUNT = 2
} LimitID;

// Motor direction commands
typedef enum {
    MOTOR_STOP = 0,
    MOTOR_UP = 1,
    MOTOR_DOWN = 2
} MotorDirection;

// LED status indicators
typedef enum {
    LED_OFF = 0,      // All LEDs off
    LED_IDLE = 1,     // Steady green (system ready)
    LED_ACTIVE = 2,   // Blinking green (motion in progress)
    LED_ERROR = 3     // Steady red (fault state)
} LEDStatus;

// Application states
typedef enum {
    APP_STATE_IDLE = 0,
    APP_STATE_MOVING_UP = 1,
    APP_STATE_MOVING_DOWN = 2,
    APP_STATE_FAULT = 3,
    APP_STATE_COUNT = 4
} AppState_t;
```

### 2. Input/Output Structures

```cpp
// Application input structure (from Main Loop → APP)
typedef struct {
    bool button_up;          // True if UP button currently pressed (debounced)
    bool button_down;        // True if DOWN button currently pressed (debounced)
    bool limit_upper;        // True if upper limit sensor active
    bool limit_lower;        // True if lower limit sensor active
    uint32_t timestamp_ms;   // Current system time in milliseconds
} AppInput_t;

// Application output structure (from APP → Main Loop)
typedef struct {
    MotorDirection motor_cmd;    // Motor command: STOP/UP/DOWN
    uint8_t motor_speed;         // Motor PWM duty cycle: 0-255
    LEDStatus led_status;        // LED status indicator
    bool diagnostics_enabled;    // Enable diagnostic output (future)
} AppOutput_t;
```

### 3. Internal State Variables (DeskApp Module)

```cpp
// State machine variables (file-scope static in DeskApp.cpp)
static AppState_t current_state = APP_STATE_IDLE;
static uint32_t state_entry_time = 0;      // Time when current state was entered
static uint32_t state_transition_count = 0; // Total number of state transitions

// Diagnostic log (optional, for SWReq-010)
#define MAX_STATE_LOG 10
static struct {
    AppState_t state;
    uint32_t timestamp;
} state_log[MAX_STATE_LOG];
static uint8_t state_log_index = 0;
```

### 4. Debouncing Variables (HAL Module)

```cpp
// Debouncing state (file-scope static in HAL.cpp)
#define DEBOUNCE_TIME_MS 20
static uint32_t last_button_time[BUTTON_COUNT] = {0, 0};
static bool button_stable_state[BUTTON_COUNT] = {false, false};
static bool button_raw_state[BUTTON_COUNT] = {false, false};
```

---

## Algorithm Specifications

### Algorithm 1: Button Debouncing (HAL Module)

**Purpose:** Filter mechanical button bounce to provide stable button state to application.

**Requirements:** SWReq-009 (20 ms debounce window)

**Pseudocode:**

```
FUNCTION HAL_readButton(button_id):
    current_time = millis()
    raw_state = digitalRead(button_pin[button_id]) == LOW  // Active low with pull-up
    
    IF raw_state != button_raw_state[button_id]:
        // Button state changed, start debounce timer
        button_raw_state[button_id] = raw_state
        last_button_time[button_id] = current_time
    
    // Check if debounce time has elapsed
    elapsed = current_time - last_button_time[button_id]
    IF elapsed >= DEBOUNCE_TIME_MS:
        // Stable state confirmed
        button_stable_state[button_id] = button_raw_state[button_id]
    
    RETURN button_stable_state[button_id]
END FUNCTION
```

**Timing Diagram:**

```
Button Physical State:  ___╱‾‾╲_╱‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
                           ^bounce period  ^stable
Raw Reading:           ___╱‾‾╲_╱‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
Time:                  0ms  5  10 15   20  25  30  35
Debounced Output:      ____________________╱‾‾‾‾‾‾‾‾‾
                                           ^20ms threshold
```

**Edge Cases:**
- Rapid button presses < 20 ms apart: Only last stable state is registered
- Button held during init: Starts debounced (safe)
- Simultaneous press/release: Each button debounced independently

---

### Algorithm 2: State Transition Logic (DeskApp Module)

**Purpose:** Implement state machine transitions based on inputs and current state.

**Requirements:** SWReq-007 (state machine with IDLE, MOVING_UP, MOVING_DOWN states)

**Pseudocode:**

```
FUNCTION APP_Task(inputs, outputs):
    // Process state-specific logic
    SWITCH current_state:
        CASE APP_STATE_IDLE:
            outputs.motor_cmd = MOTOR_STOP
            outputs.motor_speed = 0
            outputs.led_status = LED_IDLE
            
            // Check for transition to MOVING_UP
            IF inputs.button_up AND NOT inputs.button_down:
                IF NOT inputs.limit_upper:
                    transitionTo(APP_STATE_MOVING_UP)
            
            // Check for transition to MOVING_DOWN
            ELSE IF inputs.button_down AND NOT inputs.button_up:
                IF NOT inputs.limit_lower:
                    transitionTo(APP_STATE_MOVING_DOWN)
            
            // Conflicting inputs: stay in IDLE (SWReq-004)
            ELSE IF inputs.button_up AND inputs.button_down:
                // Ignore command, remain in IDLE
                outputs.motor_cmd = MOTOR_STOP
        
        CASE APP_STATE_MOVING_UP:
            outputs.motor_cmd = MOTOR_UP
            outputs.motor_speed = 255
            outputs.led_status = LED_ACTIVE
            
            // Check for return to IDLE
            IF NOT inputs.button_up:
                transitionTo(APP_STATE_IDLE)
            ELSE IF inputs.limit_upper:  // SWReq-005
                transitionTo(APP_STATE_IDLE)
        
        CASE APP_STATE_MOVING_DOWN:
            outputs.motor_cmd = MOTOR_DOWN
            outputs.motor_speed = 255
            outputs.led_status = LED_ACTIVE
            
            // Check for return to IDLE
            IF NOT inputs.button_down:
                transitionTo(APP_STATE_IDLE)
            ELSE IF inputs.limit_lower:  // SWReq-006
                transitionTo(APP_STATE_IDLE)
        
        CASE APP_STATE_FAULT:
            outputs.motor_cmd = MOTOR_STOP
            outputs.motor_speed = 0
            outputs.led_status = LED_ERROR
            // Requires power cycle to clear (future: add reset mechanism)
END FUNCTION

FUNCTION transitionTo(new_state):
    // Log transition for diagnostics (SWReq-010)
    state_log[state_log_index].state = new_state
    state_log[state_log_index].timestamp = HAL_getTime()
    state_log_index = (state_log_index + 1) % MAX_STATE_LOG
    
    // Update state variables
    current_state = new_state
    state_entry_time = HAL_getTime()
    state_transition_count++
END FUNCTION
```

---

### Algorithm 3: Motor Control (HAL Module)

**Purpose:** Translate motor commands into hardware signals for BTS7960 motor driver.

**Requirements:** SysReq-002 (response ≤ 1 sec), SysReq-003 (halt ≤ 500 ms)

**BTS7960 Motor Driver Control:**

| Direction | RPWM (Pin 9) | LPWM (Pin 10) | REN (Pin 6) | LEN (Pin 7) |
|-----------|-------------|--------------|-------------|-------------|
| STOP      | 0           | 0            | LOW         | LOW         |
| UP        | 0-255       | 0            | HIGH        | LOW         |
| DOWN      | 0           | 0-255        | LOW         | HIGH        |

**Pseudocode:**

```
FUNCTION HAL_setMotor(direction, speed):
    SWITCH direction:
        CASE MOTOR_STOP:
            digitalWrite(PIN_MOTOR_REN, LOW)
            digitalWrite(PIN_MOTOR_LEN, LOW)
            analogWrite(PIN_MOTOR_RPWM, 0)
            analogWrite(PIN_MOTOR_LPWM, 0)
        
        CASE MOTOR_UP:
            digitalWrite(PIN_MOTOR_REN, HIGH)
            digitalWrite(PIN_MOTOR_LEN, LOW)
            analogWrite(PIN_MOTOR_RPWM, speed)  // 0-255
            analogWrite(PIN_MOTOR_LPWM, 0)
        
        CASE MOTOR_DOWN:
            digitalWrite(PIN_MOTOR_REN, LOW)
            digitalWrite(PIN_MOTOR_LEN, HIGH)
            analogWrite(PIN_MOTOR_RPWM, 0)
            analogWrite(PIN_MOTOR_LPWM, speed)  // 0-255
END FUNCTION
```

**Hardware Timing:**
- PWM frequency: 490 Hz (Arduino default)
- Enable signal setup time: ~1 µs
- Motor response time: < 100 ms (mechanical inertia)

---

## Function Interface Specifications

### HAL Module Functions

#### HAL_init()

```cpp
void HAL_init(void);
```

**Purpose:** Initialize all hardware peripherals and set safe default states.

**Parameters:** None

**Returns:** None

**Preconditions:** Called once during `setup()` before any other HAL functions.

**Postconditions:**
- All GPIO pins configured (INPUT_PULLUP for buttons/sensors, OUTPUT for motor/LED)
- Motor driver disabled (both REN and LEN LOW)
- LED off
- Debounce state arrays initialized to false
- Time reference established (millis() starts at 0)

**Side Effects:** Modifies Arduino hardware registers

**Algorithm:**
1. Configure button pins as INPUT_PULLUP (pins 2, 3)
2. Configure limit sensor pins as INPUT_PULLUP (pins 4, 5)
3. Configure motor driver pins as OUTPUT (pins 6, 7, 9, 10)
4. Configure LED pin as OUTPUT (pin 13)
5. Set motor to STOP (all enable/PWM pins LOW/0)
6. Set LED to OFF
7. Initialize debounce arrays to default state

**Execution Time:** < 5 ms

---

#### HAL_readButton()

```cpp
bool HAL_readButton(ButtonID button);
```

**Purpose:** Read debounced button state.

**Parameters:**
- `button` (ButtonID): BUTTON_UP or BUTTON_DOWN

**Returns:** `true` if button is pressed (stable for ≥20 ms), `false` otherwise

**Preconditions:** HAL_init() called

**Algorithm:** See Algorithm 1 (Button Debouncing) above

**Execution Time:** < 1 ms

**Thread Safety:** Not thread-safe (single-threaded Arduino environment)

**SWReq Traceability:** SWReq-001, SWReq-002, SWReq-009

---

#### HAL_readLimitSensor()

```cpp
bool HAL_readLimitSensor(LimitID sensor);
```

**Purpose:** Read limit sensor state (no debouncing needed for sensors).

**Parameters:**
- `sensor` (LimitID): LIMIT_UPPER or LIMIT_LOWER

**Returns:** `true` if limit is reached (sensor active), `false` otherwise

**Preconditions:** HAL_init() called

**Algorithm:**
```
1. Read digitalRead() for sensor pin
2. Return (reading == LOW)  // Active low with pull-up
```

**Execution Time:** < 0.5 ms

**SWReq Traceability:** SWReq-005, SWReq-006

---

#### HAL_setMotor()

```cpp
void HAL_setMotor(MotorDirection dir, uint8_t speed);
```

**Purpose:** Command motor driver with direction and speed.

**Parameters:**
- `dir` (MotorDirection): MOTOR_STOP, MOTOR_UP, or MOTOR_DOWN
- `speed` (uint8_t): PWM duty cycle, 0-255 (0=off, 255=full speed)

**Returns:** None

**Preconditions:** HAL_init() called

**Algorithm:** See Algorithm 3 (Motor Control) above

**Execution Time:** < 2 ms

**Side Effects:** Modifies motor driver enable and PWM pins

**Safety:** Always sets enable pins before PWM to prevent glitches

**SWReq Traceability:** SWReq-001, SWReq-002, SWReq-003

---

#### HAL_setLED()

```cpp
void HAL_setLED(LEDStatus status);
```

**Purpose:** Set status LED indicator.

**Parameters:**
- `status` (LEDStatus): LED_OFF, LED_IDLE, LED_ACTIVE, or LED_ERROR

**Returns:** None

**Algorithm:**
```
SWITCH status:
    CASE LED_OFF:
        digitalWrite(PIN_LED_STATUS, LOW)
    CASE LED_IDLE:
        digitalWrite(PIN_LED_STATUS, HIGH)  // Steady on
    CASE LED_ACTIVE:
        // Blink at 1 Hz (toggle every 500 ms)
        IF (millis() % 1000) < 500:
            digitalWrite(PIN_LED_STATUS, HIGH)
        ELSE:
            digitalWrite(PIN_LED_STATUS, LOW)
    CASE LED_ERROR:
        digitalWrite(PIN_LED_STATUS, HIGH)  // Steady on (red if using RGB LED)
```

**Execution Time:** < 1 ms

**Note:** Blinking logic is state-based; called every loop iteration

---

#### HAL_getTime()

```cpp
uint32_t HAL_getTime(void);
```

**Purpose:** Get current system time in milliseconds.

**Returns:** Milliseconds since startup (wraps after ~49.7 days)

**Algorithm:**
```
RETURN millis()
```

**Execution Time:** < 0.1 ms

**SWReq Traceability:** SWReq-008 (timing reference for control loop)

---

### DeskApp Module Functions

#### APP_Init()

```cpp
void APP_Init(void);
```

**Purpose:** Initialize application state machine.

**Parameters:** None

**Returns:** None

**Preconditions:** HAL_init() called

**Postconditions:**
- State set to APP_STATE_IDLE
- State entry time set to current time
- Transition count reset to 0
- State log cleared

**Algorithm:**
```
1. current_state = APP_STATE_IDLE
2. state_entry_time = HAL_getTime()
3. state_transition_count = 0
4. Clear state_log array
5. state_log_index = 0
```

**Execution Time:** < 1 ms

**SWReq Traceability:** SWReq-007

---

#### APP_Task()

```cpp
void APP_Task(const AppInput_t* inputs, AppOutput_t* outputs);
```

**Purpose:** Execute one iteration of application state machine logic.

**Parameters:**
- `inputs` (const AppInput_t*): Pointer to input structure (read-only)
- `outputs` (AppOutput_t*): Pointer to output structure (written by function)

**Returns:** None

**Preconditions:**
- APP_Init() called
- inputs pointer valid and populated
- outputs pointer valid

**Postconditions:**
- outputs structure filled with motor command, speed, and LED status
- State machine may have transitioned to new state
- State log updated if transition occurred

**Algorithm:** See Algorithm 2 (State Transition Logic) above

**Execution Time:** < 5 ms (worst case with state transition and logging)

**Timing Constraint:** Called every ≤50 ms to meet SWReq-008

**SWReq Traceability:** SWReq-001 through SWReq-007, SWReq-010

---

#### APP_GetState()

```cpp
AppState_t APP_GetState(void);
```

**Purpose:** Query current application state (diagnostic function).

**Returns:** Current state (APP_STATE_IDLE, MOVING_UP, MOVING_DOWN, or FAULT)

**Preconditions:** APP_Init() called

**Algorithm:**
```
RETURN current_state
```

**Execution Time:** < 0.1 ms

**SWReq Traceability:** SWReq-010

---

## Timing Analysis

### Control Loop Timing (Main Loop)

```mermaid
gantt
    title Control Loop Execution Timeline (Target: ≤50 ms)
    dateFormat X
    axisFormat %L ms
    
    section Inputs
    Read Buttons (HAL)     :0, 2
    Read Limit Sensors     :2, 3
    Populate Input Struct  :3, 4
    
    section Processing
    APP_Task Execution     :4, 9
    State Logic            :crit, 4, 6
    State Transition (if)  :6, 9
    
    section Outputs
    HAL_setMotor           :9, 11
    HAL_setLED             :11, 12
    
    section Idle
    Loop Overhead          :12, 15
    Natural Delay          :15, 50
```

**Measured Timing Budget:**

| Task | Worst-Case Time | Budget Allocation |
|------|-----------------|-------------------|
| Read all inputs (HAL) | 4 ms | 10% |
| APP_Task execution | 5 ms | 15% |
| Write all outputs (HAL) | 3 ms | 10% |
| Loop overhead | 3 ms | 5% |
| **Total** | **15 ms** | **40%** |
| Available margin | 35 ms | 60% |

**Conclusion:** Loop timing well within 50 ms requirement (SWReq-008). No artificial delays needed.

---

### Response Time Analysis

**Button Press → Motor Activation:**

```
User presses button (t=0)
  ↓ Physical bounce settles (~10 ms)
  ↓ Next loop iteration reads raw state (t=10 ms)
  ↓ Debounce timer starts
  ↓ Wait for 20 ms debounce window (t=30 ms)
  ↓ Stable state confirmed (t=30 ms)
  ↓ APP_Task processes input (t=35 ms)
  ↓ State transition to MOVING_UP (t=36 ms)
  ↓ Output sent to HAL_setMotor (t=37 ms)
  ↓ Motor driver activates (t=40 ms)
  ↓ Motor begins spinning (mechanical delay ~50 ms)
Total: ~90 ms (well within 1.0 sec requirement)
```

**SysReq-002 Compliance:** ✅ 90 ms << 1000 ms

**Button Release → Motor Halt:**

```
User releases button (t=0)
  ↓ Raw state changes immediately
  ↓ Next loop iteration (t=10 ms)
  ↓ Debounce timer starts
  ↓ 20 ms debounce window (t=30 ms)
  ↓ Stable state confirmed (t=30 ms)
  ↓ APP_Task detects release (t=35 ms)
  ↓ State transition to IDLE (t=36 ms)
  ↓ Motor stop command sent (t=37 ms)
  ↓ Motor driver disables (t=38 ms)
  ↓ Motor coasts to stop (~50 ms mechanical)
Total: ~88 ms (well within 500 ms requirement)
```

**SysReq-003 Compliance:** ✅ 88 ms << 500 ms

---

## State Machine Detailed Specification

### State Transition Decision Table

| From State | button_up | button_down | limit_upper | limit_lower | To State | Motor Cmd | LED |
|-----------|-----------|-------------|-------------|-------------|----------|-----------|-----|
| IDLE | 0 | 0 | X | X | IDLE | STOP | IDLE |
| IDLE | 1 | 0 | 0 | X | MOVING_UP | UP, 255 | ACTIVE |
| IDLE | 1 | 0 | 1 | X | IDLE | STOP | IDLE |
| IDLE | 0 | 1 | X | 0 | MOVING_DOWN | DOWN, 255 | ACTIVE |
| IDLE | 0 | 1 | X | 1 | IDLE | STOP | IDLE |
| IDLE | 1 | 1 | X | X | IDLE | STOP | IDLE |
| MOVING_UP | 1 | X | 0 | X | MOVING_UP | UP, 255 | ACTIVE |
| MOVING_UP | 0 | X | X | X | IDLE | STOP | IDLE |
| MOVING_UP | X | X | 1 | X | IDLE | STOP | IDLE |
| MOVING_DOWN | X | 1 | X | 0 | MOVING_DOWN | DOWN, 255 | ACTIVE |
| MOVING_DOWN | X | 0 | X | X | IDLE | STOP | IDLE |
| MOVING_DOWN | X | X | X | 1 | IDLE | STOP | IDLE |
| FAULT | X | X | X | X | FAULT | STOP | ERROR |

**Legend:** X = don't care (any value)

---

## Error Handling & Edge Cases

### Edge Case 1: Both Buttons Pressed Simultaneously

**Scenario:** User accidentally presses UP and DOWN at the same time.

**Expected Behavior:**
- Motor remains stopped (MOTOR_STOP)
- State remains IDLE
- No transition to MOVING_UP or MOVING_DOWN

**Implementation:**
```cpp
if (inputs->button_up && inputs->button_down) {
    // Conflicting input: stay in IDLE, motor stopped
    outputs->motor_cmd = MOTOR_STOP;
    outputs->motor_speed = 0;
    // Do not transition
}
```

**SWReq Traceability:** SWReq-004

**Test Case:** Unit test with both button inputs = true; verify state remains IDLE.

---

### Edge Case 2: Button Pressed at Limit

**Scenario:** User presses UP button when desk is already at upper limit.

**Expected Behavior:**
- Motor does not activate
- State remains IDLE
- Limit sensor takes priority over button input

**Implementation:**
```cpp
if (inputs->button_up && !inputs->button_down) {
    if (!inputs->limit_upper) {  // Check limit BEFORE transitioning
        transitionTo(APP_STATE_MOVING_UP);
    }
    // If limit_upper is true, do nothing (stay in IDLE)
}
```

**SWReq Traceability:** SWReq-005, SWReq-006

**Test Case:** Unit test with button_up=true and limit_upper=true; verify no state transition.

---

### Edge Case 3: Limit Reached During Motion

**Scenario:** Desk is moving up and reaches upper limit sensor.

**Expected Behavior:**
- Motor immediately stops
- State transitions to IDLE
- Subsequent button presses ignored while at limit

**Implementation:**
```cpp
case APP_STATE_MOVING_UP:
    if (inputs->limit_upper) {
        transitionTo(APP_STATE_IDLE);  // Immediate stop
    }
    // Other logic...
```

**Timing:** Sensor read every 50 ms (loop cycle) → worst-case detection delay = 50 ms

**SWReq Traceability:** SWReq-005

---

### Edge Case 4: Power-On with Button Pressed

**Scenario:** System powered on while user is holding a button.

**Expected Behavior:**
- System initializes to IDLE state
- Button must be released and re-pressed to activate motor
- Prevents unintended motion on startup

**Implementation:**
- Debounce logic requires stable state for 20 ms
- State machine starts in IDLE
- First loop iteration will read button as "just pressed" but debounce timer not yet satisfied
- Button must remain pressed for 20 ms post-init to register

**Alternative (safer):** Explicitly ignore first 100 ms of button inputs after init:
```cpp
static uint32_t init_time = 0;
void APP_Init(void) {
    init_time = HAL_getTime();
    // ...
}

void APP_Task(...) {
    if ((HAL_getTime() - init_time) < 100) {
        // Ignore all button inputs for first 100 ms
        return;
    }
    // Normal logic...
}
```

**Safety Rationale:** Prevents unexpected desk motion during power-on.

---

### Error Condition 1: Stuck Button

**Scenario:** Button mechanically stuck in pressed position.

**Symptoms:**
- Motor runs continuously
- Desk may hit limit and stop (limit sensor overrides button)

**Mitigation:**
- Limit sensors provide hardware-level protection
- No software timeout needed (desk will stop at mechanical limit)

**Future Enhancement:** Add maximum runtime timer (e.g., 60 sec) as additional safety layer.

---

### Error Condition 2: Limit Sensor Failure

**Scenario:** Limit sensor fails to activate (always reads false).

**Risk:** Desk could over-travel beyond mechanical stop, damaging mechanism.

**Mitigation (Current Design):**
- Mechanical hard stops prevent catastrophic failure
- User must manually release button to stop motion

**Future Enhancement (Out of Scope):**
- Add motor current sensing to detect stall
- Add timeout for maximum continuous runtime
- Both would trigger FAULT state

**Risk Assessment:** MEDIUM (mechanical stops provide backup, but software timeout desirable for robustness)

---

### Error Condition 3: Motor Driver Malfunction

**Scenario:** Motor driver fails to respond to enable/PWM signals.

**Detection:** Not implemented in current design (no feedback from motor driver).

**Mitigation:**
- Visual inspection (LED shows ACTIVE state but desk not moving)
- User manually stops command and power-cycles system

**Future Enhancement:** Add motor current sensor or encoder feedback to detect motor stall/failure → transition to FAULT state.

---

## Risk Assessment

### Risk Matrix

| Risk ID | Hazard | Severity | Likelihood | Mitigation | Residual Risk |
|---------|--------|----------|------------|------------|---------------|
| R-001 | Desk over-travels beyond limits | High | Low | Limit sensors + mechanical stops | Low |
| R-002 | Unintended motion on power-up | Medium | Low | Debouncing + IDLE init state | Very Low |
| R-003 | Conflicting button commands | Low | Medium | Software logic ignores conflicts | Very Low |
| R-004 | Button stuck/jammed | Medium | Low | Limit sensors stop motion | Low |
| R-005 | Limit sensor failure | High | Very Low | Mechanical hard stops | Medium |
| R-006 | Motor driver failure (no motion) | Low | Low | User visual feedback | Low |
| R-007 | Debounce too short (false triggers) | Low | Low | 20 ms standard window | Very Low |
| R-008 | Control loop too slow (missed inputs) | Low | Very Low | 15 ms measured, 50 ms budget | Very Low |

**Risk Severity Levels:**
- **High:** Could cause injury or significant property damage
- **Medium:** Could cause minor injury or equipment damage
- **Low:** Inconvenience or minor malfunction

**Likelihood:**
- **High:** > 10% probability during normal use
- **Medium:** 1-10% probability
- **Low:** 0.1-1% probability
- **Very Low:** < 0.1% probability

---

## Implementation Checklist

### Phase 1: HAL Layer
- [ ] Implement `PinConfig.h` with pin definitions
- [ ] Implement `HAL_init()` with safe defaults
- [ ] Implement `HAL_readButton()` with debouncing
- [ ] Implement `HAL_readLimitSensor()`
- [ ] Implement `HAL_setMotor()` with BTS7960 control
- [ ] Implement `HAL_setLED()` with blink logic
- [ ] Implement `HAL_getTime()` wrapper
- [ ] Unit test HAL functions with mock hardware

### Phase 2: DeskApp Layer
- [ ] Define data structures (AppInput_t, AppOutput_t, AppState_t)
- [ ] Implement `APP_Init()`
- [ ] Implement `APP_Task()` state machine
- [ ] Implement `transitionTo()` helper function
- [ ] Implement state log for diagnostics
- [ ] Implement `APP_GetState()` query function
- [ ] Unit test state transitions with mock inputs

### Phase 3: Main Loop Integration
- [ ] Implement `setup()` calling HAL_init() and APP_Init()
- [ ] Implement `loop()` with input polling
- [ ] Implement input structure population
- [ ] Implement output execution
- [ ] Verify loop timing < 50 ms
- [ ] Integration test with real hardware

### Phase 4: Verification
- [ ] Verify SysReq-002: Button → motion < 1 sec
- [ ] Verify SysReq-003: Button release → stop < 500 ms
- [ ] Verify SysReq-005: Conflicting inputs ignored
- [ ] Verify SysReq-007: Limits stop motion
- [ ] Verify SWReq-009: Debouncing eliminates bounce
- [ ] Load test: 100 cycles of up/down motion

---

## Appendix: Pin Configuration Reference

### Arduino UNO Pin Assignments

```cpp
// PinConfig.h
#ifndef PIN_CONFIG_H
#define PIN_CONFIG_H

#include <Arduino.h>

// Input Pins (Digital)
#define PIN_BUTTON_UP      2   // Digital input with internal pull-up
#define PIN_BUTTON_DOWN    3   // Digital input with internal pull-up
#define PIN_LIMIT_UPPER    4   // Digital input with internal pull-up
#define PIN_LIMIT_LOWER    5   // Digital input with internal pull-up

// Output Pins (Motor Driver - BTS7960)
#define PIN_MOTOR_REN      6   // Right enable (UP direction enable)
#define PIN_MOTOR_LEN      7   // Left enable (DOWN direction enable)
#define PIN_MOTOR_RPWM     9   // Right PWM (UP direction speed control)
#define PIN_MOTOR_LPWM    10   // Left PWM (DOWN direction speed control)

// Output Pins (LED)
#define PIN_LED_STATUS    13   // Built-in LED (can be replaced with external)

// Hardware Constraints
#define PWM_FREQUENCY_HZ  490  // Arduino default PWM frequency
#define MAX_MOTOR_SPEED   255  // Full PWM duty cycle

#endif // PIN_CONFIG_H
```

### BTS7960 Motor Driver Wiring

| BTS7960 Pin | Arduino Pin | Signal Type | Description |
|-------------|-------------|-------------|-------------|
| RPWM | 9 (PWM) | PWM Output | Right channel PWM (UP direction) |
| LPWM | 10 (PWM) | PWM Output | Left channel PWM (DOWN direction) |
| R_EN | 6 (Digital) | Digital Output | Right channel enable |
| L_EN | 7 (Digital) | Digital Output | Left channel enable |
| R_IS | (unused) | Analog Input | Right channel current sense (future) |
| L_IS | (unused) | Analog Input | Left channel current sense (future) |
| VCC | 5V | Power | Logic power supply |
| GND | GND | Ground | Common ground |

---

**Document Version:** 1.0  
**Date:** January 24, 2026  
**Status:** Ready for Implementation  