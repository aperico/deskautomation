# Software Verification Specification

**Document ID:** SoftVerSpec-Desk-001  
**System Name:** Automated Mechanical Desk Lift System  
**Date:** February 21, 2026  
**Version:** 1.0  
**Status:** Draft  
**Derived From:** [04_SoftwareRequirements.md](04_SoftwareRequirements.md)

---

## 1. Purpose and Scope

This document defines the verification strategy and test specifications for all software requirements defined in [04_SoftwareRequirements.md](04_SoftwareRequirements.md). It specifies **how** each software requirement will be verified, using unit tests, integration tests, and mock hardware abstractions. This separation of requirements from verification ensures clear distinction between **what the software shall do** (requirements) and **how we verify it works** (verification specifications).

---

## 2. Verification Strategy Overview

### 2.1 Unit Testing Approach

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

### 2.2 Test Coverage Requirements

- ✅ Each software requirement has at least one unit test
- ✅ All state transitions tested
- ✅ All input combinations tested (normal and edge cases)
- ✅ Timing requirements verified with mock timer
- ✅ Safety-critical requirements (SWReq-003, SWReq-004, SWReq-005, SWReq-006, SWReq-012, SWReq-013, SWReq-014) have multiple test cases
- ✅ **Dual Motor Type Coverage:** All tests validate both MT_BASIC (L298N) and MT_ROBUST (IBT_2) motor configurations via `MotorConfig_getMotorType()` interface
- ✅ **Configuration Encapsulation Verification:** Tests use getter function instead of direct macro access, ensuring motor-type-agnostic application logic

---

### 2.3 Dual Motor Type Testing

#### Supported Motor Driver Configurations

The software verification suite validates both supported motor driver types:

| Motor Type | Model | Control Scheme | Driver | Test Status |
|-----------|-------|----------------|--------|-------------|
| **MT_BASIC** | L298N Dual H-Bridge | 4-wire + PWM | Arduino GPIO + PWM pins | ✅ All 37 tests passing |
| **MT_ROBUST** | IBT_2 Dual H-Bridge (FET) | 4-wire + PWM | Arduino GPIO + PWM pins | ✅ All 37 tests passing |

#### Testing Approach

Tests utilize the encapsulated `MotorConfig_getMotorType()` interface to execute all verification cases against both motor configurations:

1. **Compile-Time Configuration** (Default Build): Tests run against default MT_BASIC configuration
2. **Test-Time Override** (CI/CD): Tests can be compiled with `TEST_MOTOR_TYPE` preprocessor define to run all 37 tests against MT_ROBUST without recompilation
3. **Verification Result**: All 37 tests pass identically on both MT_BASIC and MT_ROBUST configurations

#### Motor Type Transparency in Application Logic

The application layer remains motor-type-agnostic through:
- Runtime `inputs->motor_type` struct member (populated from `MotorConfig_getMotorType()`)
- HAL layer uses `g_motor_type` variable (set once via `HAL_setMotorType(MotorConfig_getMotorType())`)
- All motor control differences abstracted in HAL implementation
- Application logic makes no assumptions about specific hardware capabilities

---

## 3. Software Requirement Verification Specifications

### 3.1 SWReq-001: UP Button Detection and Upward Motor Activation

**Requirement Statement:** The software subsystem shall detect UP button press within 100 ms and command motor activation in the upward direction.

**Derives From:** SysReq-002

**Test Method:**
- **Unit Test:** Simulate button press event using mock GPIO interface
- **Verification Steps:**
  1. Initialize mock GPIO with UP button in released state
  2. Set UP button to pressed state
  3. Execute control loop
  4. Verify motor command issued within 100 ms
  5. Confirm direction flag set to UP
  6. Verify state machine transitioned to MOVING_UP
- **Test Tool:** Google Test framework with HAL mock
- **Pass Criteria:** Motor command UP issued within ≤ 100 ms of button press detection

---

### 3.2 SWReq-002: DOWN Button Detection and Downward Motor Activation

**Requirement Statement:** The software subsystem shall detect DOWN button press within 100 ms and command motor activation in the downward direction.

**Derives From:** SysReq-002

**Test Method:**
- **Unit Test:** Simulate button press event using mock GPIO interface
- **Verification Steps:**
  1. Initialize mock GPIO with DOWN button in released state
  2. Set DOWN button to pressed state
  3. Execute control loop
  4. Verify motor command issued within 100 ms
  5. Confirm direction flag set to DOWN
  6. Verify state machine transitioned to MOVING_DOWN
- **Test Tool:** Google Test framework with HAL mock
- **Pass Criteria:** Motor command DOWN issued within ≤ 100 ms of button press detection

---

### 3.3 SWReq-003: Button Release Detection and Motor Deactivation

**Requirement Statement:** The software subsystem shall detect button release within 50 ms and command motor deactivation.

**Derives From:** SysReq-003

**Test Method:**
- **Unit Test:** Simulate button release event using mock GPIO interface
- **Verification Steps:**
  1. Initialize state machine to MOVING_UP state (simulating ongoing motion)
  2. Set UP button to released state
  3. Execute control loop
  4. Verify motor stop command issued within 50 ms
  5. Confirm motor state transitioned to IDLE
  6. Verify no further motion commands issued
- **Test Tool:** Google Test framework with HAL mock
- **Pass Criteria:** STOP command issued within ≤ 50 ms of button release detection

**Safety-Critical:** ✅ This requirement is essential for meeting 500 ms halt requirement

---

### 3.4 SWReq-004: Conflicting Input Rejection

**Requirement Statement:** The software subsystem shall ignore motor activation commands when both UP and DOWN buttons are detected as pressed simultaneously.

**Derives From:** SysReq-005

**Test Method:**
- **Unit Test:** Simultaneous button press simulation
- **Verification Steps:**
  1. Initialize state machine to IDLE
  2. Using mock GPIO, set both UP and DOWN buttons to pressed simultaneously
  3. Execute control loop
  4. Verify motor command remains STOP
  5. Verify no state transition from IDLE occurs
  6. Execute additional loop cycles to confirm sustained STOP state
- **Test Tool:** Google Test framework with HAL mock
- **Pass Criteria:** Motor remains STOP; no state change occurs; conflicting input is ignored

**Safety-Critical:** ✅ Prevents undefined motor behavior

---

### 3.5 SWReq-005: Upper Limit Enforcement

**Requirement Statement:** The software subsystem shall command motor stop when upper limit sensor is active, even if UP button is pressed.

**Derives From:** SysReq-007

**Test Method:**
- **Unit Test:** Limit sensor override verification
- **Verification Steps:**
  1. Initialize state machine to IDLE
  2. Using mock sensor interface, set upper limit sensor to active state
  3. Set UP button to pressed state
  4. Execute control loop
  5. Verify motor command is STOP
  6. Verify no upward motion is commanded
  7. Verify state machine does not transition to MOVING_UP
- **Test Tool:** Google Test framework with HAL mock
- **Pass Criteria:** Motor STOP command issued; no MOVING_UP state; limit protection active

**Safety-Critical:** ✅ Enforces travel limit protection

---

### 3.6 SWReq-006: Lower Limit Enforcement

**Requirement Statement:** The software subsystem shall command motor stop when lower limit sensor is active, even if DOWN button is pressed.

**Derives From:** SysReq-007

**Test Method:**
- **Unit Test:** Limit sensor override verification
- **Verification Steps:**
  1. Initialize state machine to IDLE
  2. Using mock sensor interface, set lower limit sensor to active state
  3. Set DOWN button to pressed state
  4. Execute control loop
  5. Verify motor command is STOP
  6. Verify no downward motion is commanded
  7. Verify state machine does not transition to MOVING_DOWN
- **Test Tool:** Google Test framework with HAL mock
- **Pass Criteria:** Motor STOP command issued; no MOVING_DOWN state; limit protection active

**Safety-Critical:** ✅ Enforces travel limit protection

---

### 3.7 SWReq-007: State Machine Implementation

**Requirement Statement:** The software subsystem shall maintain motor control state machine with at least three states: IDLE, MOVING_UP, and MOVING_DOWN.

**Derives From:** SysReq-002, SysReq-003

**Test Method:**
- **Unit Test:** State machine verification
- **Verification Steps:**
  1. Verify initial state is IDLE after initialization
  2. Simulate UP button press; verify transition to MOVING_UP
  3. Simulate button release; verify transition back to IDLE
  4. Simulate DOWN button press; verify transition to MOVING_DOWN
  5. Simulate button release; verify transition back to IDLE
  6. Execute 100+ state transition cycles to confirm robustness
- **Test Tool:** Google Test framework with HAL mock; state history analysis
- **Pass Criteria:** All state transitions occur correctly; no illegal transitions; consistent behavior over extended operation

---

### 3.8 SWReq-008: Control Loop Cycle Time

**Requirement Statement:** The software subsystem shall execute the main control loop with a cycle time of ≤ 50 ms.

**Derives From:** SysReq-002, SysReq-003

**Test Method:**
- **Unit Test:** Timing verification with mock timer
- **Verification Steps:**
  1. Initialize mock timer
  2. Execute control loop and measure elapsed time for each iteration
  3. Record timing for 1000+ iterations
  4. Verify all iterations execute within ≤ 50 ms
  5. Calculate mean and max cycle time
  6. Verify loop executes at consistent interval (jitter < 10%)
- **Test Tool:** Google Test with timing instrumentation
- **Pass Criteria:** Maximum cycle time ≤ 50 ms; mean < 40 ms; jitter acceptable

---

### 3.9 SWReq-009: Button Input Debouncing

**Requirement Statement:** The software subsystem shall debounce button inputs using a 20 ms debounce window.

**Derives From:** SysReq-002

**Test Method:**
- **Unit Test:** Debounce filter verification
- **Verification Steps:**
  1. Using mock GPIO, simulate rapid button state changes (bounce pattern: press-release-press)
  2. Execute control loop with time advancement to simulate 5 ms intervals
  3. Verify spurious state changes within 20 ms window are ignored
  4. Confirm stable state after 20 ms is properly registered
  5. Verify motor commands only generated on stable button state
  6. Execute multiple bounce patterns to confirm consistency
- **Test Tool:** Google Test with mock timer; GPIO simulation
- **Pass Criteria:** No spurious motor commands; only stable state registered; robust to 20 ms noise

---

### 3.10 SWReq-010: Operational State and Command History

**Requirement Statement:** The software subsystem shall maintain operational state and command history for diagnostic purposes.

**Derives From:** SysReq-008

**Test Method:**
- **Unit Test:** State history capture and retrieval
- **Verification Steps:**
  1. Execute sequence of button commands: UP, release, DOWN, release, simultaneous UP+DOWN
  2. After execution, retrieve state history log
  3. Verify all state transitions logged with timestamps
  4. Verify all motor commands recorded (direction and timing)
  5. Verify log contains at least 20 entries for extended test sequence
  6. Validate timestamp ordering (monotonically increasing)
- **Test Tool:** Google Test with history buffer inspection
- **Pass Criteria:** Complete history available; timestamps accurate; all transitions logged; max 1 entry missing allowed per 100 transitions

---

### 3.11 SWReq-011: Non-Blocking Scheduler

**Requirement Statement:** The software subsystem shall implement a non-blocking scheduler that invokes `APP_Task()` at a fixed period of 250 ms (±10 ms).

**Derives From:** SysReq-009

**Test Method:**
- **Unit Test + Static Analysis:**
  - **Timing Test:** Mock timer advancement with scheduler verification
    1. Initialize scheduler with 250 ms period
    2. Advance mock time in 50 ms increments
    3. Track `APP_Task()` invocation count
    4. Verify task invoked at 4 Hz (every 250 ms) over ≥ 60 seconds
    5. Measure actual inter-call intervals; verify 240-260 ms range
  - **Static Analysis:** Code review tool (cppcheck)
    - Scan for blocking calls (sleep, wait, busy-loops)
    - Verify no blocking I/O in main loop
    - Confirm non-blocking timer checks
- **Test Tool:** Google Test with mock timer; cppcheck static analysis
- **Pass Criteria:** `APP_Task()` called at exactly 4 Hz; jitter < 10%; no blocking calls detected

---

### 3.12 SWReq-012: Motor STOP When No Valid Command

**Requirement Statement:** The software subsystem shall command motor STOP when no valid UP/DOWN command is present.

**Derives From:** SysReq-010

**Test Method:**
- **Unit Test:** No-command safety verification
- **Verification Steps:**
  1. Initialize mock GPIO with UP and DOWN buttons in released state
  2. Execute control loop multiple times (no button press)
  3. Verify motor command remains STOP on every iteration
  4. Verify no state transition from IDLE occurs
  5. Simulate timeout scenario (long idle period)
  6. Confirm sustained STOP state
- **Test Tool:** Google Test with HAL mock
- **Pass Criteria:** Motor command is STOP; state remains IDLE; consistent over time

**Safety-Critical:** ✅ Prevents unintended motion

---

### 3.13 SWReq-013: Safe Initialization After Reset

**Requirement Statement:** The software subsystem shall initialize to a safe STOP state after reset or brownout and require a new valid command before motion.

**Derives From:** SysReq-011

**Test Method:**
- **Integration Test:** Reset and brownout handling
- **Verification Steps:**
  1. Simulate reset event using mock reset interface
  2. Verify system initializes to SAFE_STOP state (motor disabled, outputs de-energized)
  3. Set UP button to pressed state
  4. Verify motion only occurs after new valid command processed
  5. Repeat with simulated brownout event
  6. Verify same safe initialization behavior
  7. Confirm no accidental motion during recovery
- **Test Tool:** Google Test with reset/brownout injection
- **Pass Criteria:** SAFE_STOP state after reset; motion only after valid command; no unintended motion during recovery

**Safety-Critical:** ✅ Prevents unintended motion on power recovery

---

### 3.14 SWReq-014: Stuck-On/Runaway Detection

**Requirement Statement:** The software subsystem shall detect motor driver stuck-on or runaway behavior and enter a fault state that commands STOP and indicates error.

**Derives From:** SysReq-012

**Test Method:**
- **Integration Test:** Fault injection and detection
- **Verification Steps:**
  1. Using mock motor controller, simulate normal motion (motor responding to UP command)
  2. Inject fault condition: command STOP but simulate motor still active (stuck-on)
  3. Monitor current sensing diagnostics
  4. Verify fault detection within ≤ 100 ms of stuck-on condition
  5. Verify system enters FAULT_STOP state
  6. Verify error indicator (LED) is asserted
  7. Verify no further motion commands issued
  8. Test recovery requires explicit reset (no auto-recovery)
- **Test Tool:** Google Test with fault injection; motor controller mock
- **Pass Criteria:** Fault detected within 100 ms; FAULT_STOP entered; error indicated; requires manual reset to clear

**Safety-Critical:** ✅ Mitigates electrical fault hazards

---

## 4. Verification Traceability Matrix

### Verification Coverage by Requirement

| SWReq-ID | Test ID | Test Type | Coverage Status |
|----------|---------|-----------|-----------------|
| SWReq-001 | UT-SW-001 | Unit Test | ✅ Complete |
| SWReq-002 | UT-SW-002 | Unit Test | ✅ Complete |
| SWReq-003 | UT-SW-003 | Unit Test | ✅ Complete (Safety-Critical) |
| SWReq-004 | UT-SW-004 | Unit Test | ✅ Complete (Safety-Critical) |
| SWReq-005 | UT-SW-005 | Unit Test | ✅ Complete (Safety-Critical) |
| SWReq-006 | UT-SW-006 | Unit Test | ✅ Complete (Safety-Critical) |
| SWReq-007 | UT-SW-007 | Unit Test | ✅ Complete |
| SWReq-008 | UT-SW-008 | Unit Test + Timing Analysis | ✅ Complete |
| SWReq-009 | UT-SW-009 | Unit Test | ✅ Complete |
| SWReq-010 | UT-SW-010 | Unit Test | ✅ Complete |
| SWReq-011 | UT-SW-011 + SA-001 | Unit Test + Static Analysis | ✅ Complete |
| SWReq-012 | UT-SW-012 | Unit Test | ✅ Complete (Safety-Critical) |
| SWReq-013 | IT-SW-013 | Integration Test | ✅ Complete (Safety-Critical) |
| SWReq-014 | IT-SW-014 | Integration Test | ✅ Complete (Safety-Critical) |

---

## 5. Test Implementation Environment

### 5.1 Build and Test Infrastructure

- **Unit Test Framework:** Google Test (gtest)
- **Mock Hardware Library:** HALMock (custom implementation)
- **Build System:** CMake with MinGW-w64 GCC compiler
- **Test Execution:** CTest with automated test discovery
- **Coverage Measurement:** gcov/lcov (optional, for detailed coverage analysis)

### 5.2 Mock Interfaces

All tests use mock implementations of the Hardware Abstraction Layer (HAL):

**HALMock.h provides:**
- `HALMock_readButton(button_id)` - Get button state
- `HALMock_readLimitSensor(sensor_id)` - Get sensor state
- `HALMock_setMotor(direction, speed)` - Set motor command
- `HALMock_getTime()` - Get simulated time
- `HALMock_recordCommand()` - Log motor commands for analysis
- `HALMock_reset()` - Simulate reset event
- `HALMock_injectFault()` - Inject stuck-on condition

### 5.3 Test Execution Procedure

1. **Build tests:** `cmake --build build --config Release --target UnitTests`
2. **Run all tests:** `ctest --test-dir build -C Release`
3. **Run specific requirement test:** `ctest --test-dir build -C Release -R ^UT-SW-003$`
4. **View detailed output:** `ctest --test-dir build -C Release --output-on-failure`

---

## 6. Approvals

- **Software Test Engineer:** [Pending]
- **System Engineer:** [Pending]
- **Project Manager:** [Pending]
