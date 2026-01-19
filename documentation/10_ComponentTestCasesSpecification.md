# Software Component Test Cases Specification

**Document Version:** 2.0  
**Last Updated:** January 19, 2026  
**Status:** Approved  
**Standards:** IEEE 29148-2018, ISTQB Test Case Format, ISO 25119  
**Implementation:** tests/SoftwareTests.cpp  
**Test Framework:** Google Test (gtest)  
**Compliance:** ASPICE SWE.4 (Software Unit Verification)

---

## Overview

This document specifies unit and component test cases for the Automated Mechanical Desk Lift system. Each test case is:
- **Derived from** requirements (SWE-REQ-XXX) and design elements (FUNC-XXX, MODULE-XXX)
- **Traceable** with unique identifiers (TC-XXX)
- **Executable** with clear preconditions, steps, and expected results
- **Verifiable** through automated test execution

**Target Audience:** Test Engineers, Software Developers, QA Team

**Implementation Scope:**
- v1.0 tests focus on rocker switch control and basic motor commands
- v2.0 tests will add FSM, timeout, and safety features (currently documented but not implemented)

---

## Navigation
- [07_SoftwareRequirements.md](07_SoftwareRequirements.md)
- [09_SoftwareDetailedDesign.md](09_SoftwareDetailedDesign.md)
- [SoftwareIntegrationTestsSpecification.md](11_SoftwareIntegrationTestsSpecification.md)
- [TraceabilityMatrix.md](12_TraceabilityMatrix.md)

---

## Test Case Structure
Each test case includes:
- **Test ID:** Unique identifier (TC-XXX)
- **Requirement:** Traceability to SWE-REQ-XXX
- **Precondition:** Initial system state
- **Steps:** Actions to perform
- **Expected Result:** Verification criteria
- **Implementation:** Test function name in SoftwareTests.cpp

---

## Component Test Example

**Purpose:** Component tests verify individual software components (modules/functions) in isolation from hardware.

**Characteristics:**
- Test single component: DeskController (MODULE-003)
- Mock or stub external dependencies
- Focus on function behavior and logic
- Verify requirements and design specifications
- Use naming: `Component_TC###_DescriptiveName`

**Example Test Case:**

```cpp
/**
 * @test Component_TC003_SwitchUp_CommandsUpward
 * @req SWE-REQ-003, SWE-REQ-005, SWE-REQ-020
 * @function FUNC-017 (DeskApp_task)
 * 
 * Objective: Verify that SWITCH_STATE_UP commands upward motor movement
 * 
 * Test Level: COMPONENT
 * - Tests: DeskController component logic
 * - Inputs: Mocked switch state
 * - Outputs: Motor control commands
 * - No hardware interaction
 */
TEST(DeskController, Component_TC003_SwitchUp_CommandsUpward) {
    // Arrange: Setup test inputs and outputs
    DeskAppInputs_t inputs = {};
    DeskAppOutputs_t outputs = {};
    DeskApp_task_init(&inputs, &outputs);
    
    inputs.switch_state = SWITCH_STATE_UP;
    
    // Act: Execute component under test
    DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);
    
    // Assert: Verify expected behavior
    EXPECT_EQ(ret, APP_TASK_SUCCESS) << "Task should return success";
    EXPECT_TRUE(outputs.motor_enable) << "Motor should be enabled";
    EXPECT_FALSE(outputs.motor_direction) << "Direction should be UP (false)";
    EXPECT_EQ(outputs.motor_pwm, 255) << "PWM should be full speed";
}
```

**Key Points for Component Tests:**
1. **Naming:** Prefix with `Component_` + test case ID + descriptive name
2. **Documentation:** Include @test, @req, @function tags for traceability
3. **Structure:** Follow Arrange-Act-Assert pattern
4. **Isolation:** Test only the component, mock dependencies
5. **Assertions:** Use descriptive messages explaining what's being verified
6. **Coverage:** Test normal cases, edge cases, and error conditions

---

## TC-001: System Initialization (SWE-REQ-001, SWE-REQ-002)

**Module:** MODULE-003 (DeskController)  
**Function:** FUNC-016 (DeskApp_task_init)  
**Priority:** High (Safety-Critical)

### Test Case: TC-001-1 Init_SetsSafeDefaults
**Implementation:** `TEST(DeskController, TC001_Init_SetsSafeDefaults)`  
**Requirement:** SWE-REQ-001, SWE-REQ-002
- Precondition: None.
- Steps:
  1. Call DeskApp_task_init(&inputs, &outputs).
  2. Verify output structure.
- Expected Result:
  - motor_enable = false
  - motor_direction = false (doesn't matter when disabled)
  - motor_pwm = 0
  - Return code: (void function)

### Test Case: TC-001-2 SwitchOff_NoMovement
**Implementation:** `TEST(DeskController, TC001_SwitchOff_NoMovement)`  
**Requirement:** SWE-REQ-002, SWE-REQ-007, SWE-REQ-008
- Precondition: System initialized.
- Steps:
  1. Set switch_state = SWITCH_STATE_OFF.
  2. Call DeskApp_task(&inputs, &outputs).
- Expected Result:
  - motor_enable = false
  - motor_pwm = 0
  - Return code: APP_TASK_SUCCESS

---

## TC-003: Upward Movement (SWE-REQ-003, SWE-REQ-005)

**Module:** MODULE-003 (DeskController)  
**Function:** FUNC-017 (DeskApp_task)  
**Priority:** High

### Test Case: TC-003-1 SwitchUp_CommandsUpward
**Implementation:** `TEST(DeskController, TC003_SwitchUp_CommandsUpward)`  
**Requirement:** SWE-REQ-003, SWE-REQ-005
- Precondition: System initialized.
- Steps:
  1. Set switch_state = SWITCH_STATE_UP.
  2. Call DeskApp_task(&inputs, &outputs).
- Expected Result:
  - motor_enable = true
  - motor_direction = false (UP direction)
  - motor_pwm = 255 (full speed)
  - Return code: APP_TASK_SUCCESS

### Test Case: TC-003-2 SwitchUp_ConsistentOutput
**Implementation:** `TEST(DeskController, TC003_SwitchUp_ConsistentOutput)`  
**Requirement:** SWE-REQ-003, SWE-REQ-005, SWE-REQ-020
- Precondition: System initialized.
- Steps:
  1. Set switch_state = SWITCH_STATE_UP.
  2. Call DeskApp_task(&inputs, &outputs) multiple times.
  3. Verify output remains consistent.
- Expected Result:
  - Outputs remain stable across multiple calls
  - motor_enable = true, motor_direction = false, motor_pwm = 255

---

## TC-005: Downward Movement (SWE-REQ-004, SWE-REQ-006)

**Module:** MODULE-003 (DeskController)  
**Function:** FUNC-017 (DeskApp_task)  
**Priority:** High

### Test Case: TC-005-1 SwitchDown_CommandsDownward
**Implementation:** `TEST(DeskController, TC005_SwitchDown_CommandsDownward)`  
**Requirement:** SWE-REQ-004, SWE-REQ-006
- Precondition: System initialized.
- Steps:
  1. Set switch_state = SWITCH_STATE_DOWN.
  2. Call DeskApp_task(&inputs, &outputs).
- Expected Result:
  - motor_enable = true
  - motor_direction = true (DOWN direction)
  - motor_pwm = 255 (full speed)
  - Return code: APP_TASK_SUCCESS

### Test Case: TC-005-2 SwitchDown_ConsistentOutput
**Implementation:** `TEST(DeskController, TC005_SwitchDown_ConsistentOutput)`  
**Requirement:** SWE-REQ-004, SWE-REQ-006, SWE-REQ-020
- Precondition: System initialized.
- Steps:
  1. Set switch_state = SWITCH_STATE_DOWN.
  2. Call DeskApp_task(&inputs, &outputs) multiple times.
  3. Verify output remains consistent.
- Expected Result:
  - Outputs remain stable across multiple calls
  - motor_enable = true, motor_direction = true, motor_pwm = 255

---

## TC-007: Switch State Transitions (SWE-REQ-020)

**Module:** MODULE-003 (DeskController)  
**Function:** FUNC-017 (DeskApp_task)  
**Priority:** High

### Test Case: TC-007-1 SwitchTransitions_AllCombinations
**Implementation:** `TEST(DeskController, TC007_SwitchTransitions_AllCombinations)`  
**Requirement:** SWE-REQ-020
- Precondition: System initialized.
- Steps:
  1. Test OFF → UP → OFF transition.
  2. Test OFF → DOWN → OFF transition.
  3. Test UP → OFF → DOWN transition.
  4. Test DOWN → OFF → UP transition.
- Expected Result:
  - All transitions produce correct outputs
  - No invalid states or glitches
  - Immediate response to switch changes

---

## TC-009: Dwell Before Reversal (CONST-003) [v2.0 DEFERRED]

**Status:** Not implemented in v1.0  
**Planned for:** v2.0 with full FSM

### Test Case: TC-009 DwellBeforeReversal_UpToDown
**Implementation:** Deferred  
**Requirement:** CONST-003, SWE-REQ-014
- Precondition: System is moving up (v2.0 FSM).
- Steps:
  1. Switch from UP to OFF to DOWN rapidly.
- Expected Result (v2.0):
  - System dwells before reversing direction (DWELL state).
  - Brief pause prevents mechanical stress.

---

## TC-008: Null Pointer Handling (SWE-REQ-020)

**Module:** MODULE-003 (DeskController)  
**Function:** FUNC-017 (DeskApp_task)  
**Priority:** High (Robustness)

### Test Case: TC-008-1 NullInputs_ReturnsError
**Implementation:** `TEST(DeskController, TC008_NullInputs_ReturnsError)`  
**Requirement:** SWE-REQ-020 (defensive programming)
- Precondition: None.
- Steps:
  1. Call DeskApp_task(NULL, &outputs).
- Expected Result:
  - Return code: APP_TASK_ERROR
  - System remains stable

### Test Case: TC-008-2 NullOutputs_ReturnsError
**Implementation:** `TEST(DeskController, TC008_NullOutputs_ReturnsError)`  
**Requirement:** SWE-REQ-020
- Precondition: None.
- Steps:
  1. Call DeskApp_task(&inputs, NULL).
- Expected Result:
  - Return code: APP_TASK_ERROR
  - No crash or undefined behavior

---

## TC-011: Emergency Stop Detection (SWE-REQ-010, SWE-REQ-011) [v2.0 DEFERRED]

**Status:** Not implemented in v1.0  
**Planned for:** v2.0 with error state machine

### Test Case: TC-011-1 EmergencyCondition_StopsMotor
**Implementation:** Deferred  
**Requirement:** SWE-REQ-010, SWE-REQ-011, SWE-REQ-015
- Precondition: System operating (v2.0).
- Steps:
  1. Trigger emergency condition (to be defined in v2.0).
- Expected Result (v2.0):
  - Motor stops immediately (< 50ms)
  - Error state entered
  - Manual reset required

---

## TC-013: Power-Off Recovery (SWE-REQ-009)

**Module:** MODULE-003 (DeskController), MODULE-004 (Main)  
**Function:** FUNC-016 (DeskApp_task_init)  
**Priority:** High (Safety)

### Test Case: TC-013 PowerCycle_ReinitializesClean
**Implementation:** `TEST(DeskController, TC013_PowerCycle_ReinitializesClean)`  
**Requirement:** SWE-REQ-009, SWE-REQ-001
- Precondition: System was commanding movement.
- Steps:
  1. Set outputs to active state (motor_enable=true).
  2. Call DeskApp_task_init (simulates power cycle).
  3. Verify outputs reset.
- Expected Result:
  - motor_enable = false
  - motor_pwm = 0
  - Safe state restored

---

## TC-014: Input Validation (SWE-REQ-010, SWE-REQ-014) [v2.0 DEFERRED]

**Status:** Not applicable to v1.0 rocker switch  
**Planned for:** v2.0 if dual buttons re-introduced

### Test Case: TC-014 ConflictingInputs_SafeHandling
**Implementation:** Not applicable  
**Requirement:** SWE-REQ-010, SWE-REQ-014
- Note: Single rocker switch cannot have conflicting inputs
- Future dual-button design would need conflict detection

---

## TC-016: Error Detection - Both Limits (SWE-REQ-015, SWE-REQ-016)

### Test Case: TC-016 BothLimitsActive_ErrorDetection
**Implementation:** Verified in TC-011, TC-012  
**Requirement:** SWE-REQ-015, SWE-REQ-016
- Precondition: System is powered on.
- Steps:
  1. Activate both upper and lower limits (upperLimitActive=true, lowerLimitActive=true).
- Expected Result:
  - System detects error condition.
  - Error indicated (error=true).
  - APP_TASK_ERROR returned.

---

## TC-018: Movement Timeout (SWE-REQ-018)

### Test Case: TC-018 TimeoutCondition_Documentation
**Implementation:** `SWEREQ018_TimeoutCondition_Documentation`  
**Requirement:** SWE-REQ-018

- Note: Full timeout testing (30 seconds) requires integration tests with time simulation.
- This test documents the requirement.
- Actual timeout verification in IT-002 and IT-003 (IntegrationTests.cpp).

---

## TC-020: State Transition Integrity (SWE-REQ-020)

### Test Case: TC-020 StateTransitionIntegrity_AllStatesValid
**Implementation:** `SWEREQ020_StateTransitionIntegrity_AllStatesValid`  
**Requirement:** SWE-REQ-020

- Precondition: System is initialized.
- Steps:
  1. Verify IDLE → MOVING_UP transition.
  2. Verify MOVING_UP → IDLE transition.
  3. Verify IDLE → MOVING_DOWN transition.
  4. Verify MOVING_DOWN → IDLE transition.
- Expected Result:
  - All state transitions execute correctly.
  - Outputs match expected state.
  - No invalid state transitions occur.

---

## Test Summary

### v1.0 Implemented Tests

| Test ID | Requirement | Test Name | Priority | Status |
|---------|------------|-----------|----------|--------|
| TC-001-1 | SWE-REQ-001, 002 | Init_SetsSafeDefaults | High | ✓ Impl |
| TC-001-2 | SWE-REQ-002, 007, 008 | SwitchOff_NoMovement | High | ✓ Impl |
| TC-003-1 | SWE-REQ-003, 005 | SwitchUp_CommandsUpward | High | ✓ Impl |
| TC-003-2 | SWE-REQ-003, 005, 020 | SwitchUp_ConsistentOutput | High | ✓ Impl |
| TC-005-1 | SWE-REQ-004, 006 | SwitchDown_CommandsDownward | High | ✓ Impl |
| TC-005-2 | SWE-REQ-004, 006, 020 | SwitchDown_ConsistentOutput | High | ✓ Impl |
| TC-007-1 | SWE-REQ-020 | SwitchTransitions_AllCombinations | High | ✓ Impl |
| TC-008-1 | SWE-REQ-020 | NullInputs_ReturnsError | High | ✓ Impl |
| TC-008-2 | SWE-REQ-020 | NullOutputs_ReturnsError | High | ✓ Impl |
| TC-013 | SWE-REQ-009, 001 | PowerCycle_ReinitializesClean | High | ✓ Impl |

### v2.0 Deferred Tests

| Test ID | Requirement | Test Name | Status |
|---------|------------|-----------|--------|
| TC-009 | CONST-003, SWE-REQ-014 | Dwell Before Reversal | ⏳ v2.0 |
| TC-011 | SWE-REQ-010, 011, 015 | Emergency Stop | ⏳ v2.0 |
| TC-014 | SWE-REQ-010, 014 | Conflicting Inputs | ⏳ v2.0 |
| TC-016 | SWE-REQ-015, 016 | Error Detection & Recovery | ⏳ v2.0 |
| TC-018 | SWE-REQ-018 | Movement Timeout | ⏳ v2.0 |

### Test Coverage Analysis

**v1.0 Requirements Coverage:**
- ✓ SWE-REQ-001: System Initialization (TC-001-1, TC-013)
- ✓ SWE-REQ-002: Ready State (TC-001-2)
- ✓ SWE-REQ-003: Upward Detection (TC-003-1, TC-003-2)
- ✓ SWE-REQ-004: Downward Detection (TC-005-1, TC-005-2)
- ✓ SWE-REQ-005: Upward Execution (TC-003-1, TC-003-2)
- ✓ SWE-REQ-006: Downward Execution (TC-005-1, TC-005-2)
- ✓ SWE-REQ-007: Upward Termination (TC-001-2)
- ✓ SWE-REQ-008: Downward Termination (TC-001-2)
- ✓ SWE-REQ-009: Power Loss Handling (TC-013)
- ⏳ SWE-REQ-010: Emergency Stop (v2.0)
- ⏳ SWE-REQ-011: Emergency Execution (v2.0)
- ⏳ SWE-REQ-014: Conflicting Inputs (v2.0)
- ⏳ SWE-REQ-015: Error Detection (v2.0)
- ⏳ SWE-REQ-016: Error Recovery (v2.0)
- ⏳ SWE-REQ-018: Movement Timeout (v2.0)
- ✓ SWE-REQ-020: State Integrity (TC-007-1, TC-008-1/2)
- ⏳ SWE-REQ-021: Current Monitoring (Integration tests)

**Module Coverage:**
- MODULE-003 (DeskController): 100% of v1.0 functions
  - FUNC-016 (DeskApp_task_init): TC-001-1, TC-013
  - FUNC-017 (DeskApp_task): All other tests

---

## Implementation Notes

- **Test Framework:** Google Test (gtest) v1.12+
- **Test File:** tests/SoftwareTests.cpp
- **Build Command:** `cmake --build build --config Release`
- **Run Command:** `ctest --test-dir build -C Release --output-on-failure`
- **Coverage Target:** 95%+ branch coverage for v1.0 implemented code

### Test Execution Requirements

1. All tests must pass on host platform (Windows/Linux/Mac)
2. Tests must execute in < 1 second total
3. No hardware dependencies (HAL mocked)
4. Deterministic results (no timing dependencies)
5. Clear failure messages with expected vs actual values

### Adding New Tests

1. Document test case in this specification first
2. Assign unique TC-XXX identifier
3. Map to requirements (SWE-REQ-XXX)
4. Map to design elements (FUNC-XXX, MODULE-XXX)
5. Implement in SoftwareTests.cpp
6. Update traceability matrix

---

## References

- [07_SoftwareRequirements.md](07_SoftwareRequirements.md) - Requirements source
- [09_SoftwareDetailedDesign.md](09_SoftwareDetailedDesign.md) - Design specification
- [SoftwareIntegrationTestsSpecification.md](11_SoftwareIntegrationTestsSpecification.md) - Integration tests
- [TraceabilityMatrix.md](12_TraceabilityMatrix.md) - Complete traceability
