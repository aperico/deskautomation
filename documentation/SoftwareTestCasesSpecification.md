# Software Test Cases Specification

This document lists the unit test cases for the Automated Mechanical Desk Lift system. Test cases are derived from [Software Requirements](SoftwareRequirements.md) and are designed to be clear, repeatable, and verifiable.

**Standards:** IEEE 29148-2018 (Requirements Engineering), ISTQB Test Case Format  
**Implementation:** tests/SoftwareTests.cpp  
**Test Framework:** Google Test (gtest)  
**Traceability:** Each test case maps to specific SWE-REQ-XXX requirements

---

## Navigation
- [System Use Cases](SystemUseCases.md)
- [Software Requirements](SoftwareRequirements.md)
- [Software Integration Tests Specification](SoftwareIntegrationTestsSpecification.md)
- [Traceability Matrix](TraceabilityMatrix.md)

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

## TC-001: System Initialization (SWE-REQ-001)

### Test Case: TC-001-1 PowerOn_InitializesToIdle
**Implementation:** `SR01_Power_IdleStop_NoError`  
**Requirement:** SWE-REQ-001, SWE-REQ-002
- Precondition: System is powered off.
- Steps:
  1. Power on the system.
- Expected Result:
  - All input/output variables are set to default.
  - System state is IDLE.
  - Movement outputs are inactive (moveUp=false, moveDown=false).
  - No error indicated (error=false).

### Test Case: TC-001-2 NoButtonsPressed_IdleNoMovement
**Implementation:** `SWEREQ001_NoButtonsPressed_IdleNoMovement`  
**Requirement:** SWE-REQ-001, SWE-REQ-002
- Precondition: System is powered on and in IDLE.
- Steps:
  1. Ensure no buttons are pressed.
- Expected Result:
  - No movement is commanded (moveUp=false, moveDown=false).
  - No error is indicated (error=false).

---

## TC-003: Upward Movement (SWE-REQ-003, SWE-REQ-005, SWE-REQ-007)

### Test Case: TC-003-1 UpPressed_MovesUp_NotAtLimit
**Implementation:** `SWEREQ003_UpPressed_MovesUp_WhenNotupperLimitActive`  
**Requirement:** SWE-REQ-003, SWE-REQ-005, SWE-REQ-017
- Precondition: System is in IDLE, not at upper limit.
- Steps:
  1. Press Up button (btUPPressed=true).
- Expected Result:
  - Upward movement is commanded (moveUp=true).
  - No downward movement (moveDown=false).
  - No error (error=false).

### Test Case: TC-004 UpPressed_AtUpperLimit_NoMovement
**Implementation:** `SWEREQ005_UpPressed_DoesNotMoveUp_WhenupperLimitActive`  
**Requirement:** SWE-REQ-005, SWE-REQ-007
- Precondition: System is in IDLE, upper limit is active.
- Steps:
  1. Press Up button (btUPPressed=true, upperLimitActive=true).
- Expected Result:
  - No movement is commanded (moveUp=false, moveDown=false).
  - System remains in IDLE.
  - No error (error=false).

---

## TC-005: Downward Movement (SWE-REQ-004, SWE-REQ-006, SWE-REQ-008)

### Test Case: TC-005-1 DownPressed_MovesDown_NotAtLimit
**Implementation:** `SWEREQ004_DownPressed_MovesDown_WhenNotlowerLimitActive`  
**Requirement:** SWE-REQ-004, SWE-REQ-006, SWE-REQ-017
- Precondition: System is in IDLE, not at lower limit.
- Steps:
  1. Press Down button (btDOWNPressed=true).
- Expected Result:
  - Downward movement is commanded (moveDown=true).
  - No upward movement (moveUp=false).
  - No error (error=false).

### Test Case: TC-006 DownPressed_AtLowerLimit_NoMovement
**Implementation:** `SWEREQ006_DownPressed_DoesNotMoveDown_WhenlowerLimitActive`  
**Requirement:** SWE-REQ-006, SWE-REQ-008
- Precondition: System is in IDLE, lower limit is active.
- Steps:
  1. Press Down button (btDOWNPressed=true, lowerLimitActive=true).
- Expected Result:
  - No movement is commanded (moveUp=false, moveDown=false).
  - System remains in IDLE.
  - No error (error=false).

---

## TC-009: Dwell Before Reversal (CONST-003)

### Test Case: TC-009 DwellBeforeReversal_UpToDown
**Implementation:** `CONST003_DwellBeforeReversal_UpToDown`  
**Requirement:** CONST-003 (Architectural constraint), SWE-REQ-014
- Precondition: System is moving up.
- Steps:
  1. Release Up button (btUPPressed=false).
  2. Immediately press Down button (btDOWNPressed=true).
- Expected Result:
  - System dwells before reversing direction (DWELL state).
  - No immediate downward movement (moveDown=false).
  - Movement inhibited during dwell period.

---

## TC-011: Emergency Stop - Both Limits Active (SWE-REQ-010, SWE-REQ-011)

### Test Case: TC-011-1 EmergencyStop_MovingUp_BothLimitsActive
**Implementation:** `SWEREQ010_EmergencyStop_FromUp_WhenBothLimitsActive`  
**Requirement:** SWE-REQ-010, SWE-REQ-011, SWE-REQ-015, SWE-REQ-019
- Precondition: System is initialized, both limits active (fault condition).
- Steps:
  1. Set btUPPressed=true, upperLimitActive=true, lowerLimitActive=true.
  2. Call DeskApp_task.
- Expected Result:
  - All movement outputs deactivated (moveUp=false, moveDown=false).
  - Error indicated (error=true).
  - Return code APP_TASK_ERROR.

### Test Case: TC-012 EmergencyStop_MovingDown_BothLimitsActive
**Implementation:** `SWEREQ011_EmergencyStop_FromDown_WhenBothLimitsActive`  
**Requirement:** SWE-REQ-010, SWE-REQ-011, SWE-REQ-015
- Precondition: System is initialized, both limits active (fault condition).
- Steps:
  1. Set btDOWNPressed=true, upperLimitActive=true, lowerLimitActive=true.
  2. Call DeskApp_task.
- Expected Result:
  - All movement outputs deactivated (moveUp=false, moveDown=false).
  - Error indicated (error=true).

---

## TC-013: Power-Off Recovery (SWE-REQ-009)

### Test Case: TC-013 PowerOffDuringMovement_ReinitializesToIdle
**Implementation:** `SWEREQ009_PowerOffDuringMovement_ReinitializesToIdle`  
**Requirement:** SWE-REQ-009, SWE-REQ-001, SWE-REQ-016
- Precondition: System is moving up (moveUp=true).
- Steps:
  1. Simulate power-off by calling DeskApp_task_init.
  2. Verify outputs after reinitialization.
- Expected Result:
  - System reinitializes to IDLE.
  - All movement outputs inactive (moveUp=false, moveDown=false).
  - No error (error=false).

---

## TC-014: Simultaneous Button Presses (SWE-REQ-010, SWE-REQ-014)

### Test Case: TC-014 BothButtonsPressed_NoMovement
**Implementation:** `SWEREQ014_BothButtonsPressed_NoMovement_Commanded`  
**Requirement:** SWE-REQ-010, SWE-REQ-014
- Precondition: System is in IDLE.
- Steps:
  1. Press both Up and Down buttons simultaneously (btUPPressed=true, btDOWNPressed=true).
- Expected Result:
  - No movement is commanded (moveUp=false, moveDown=false).
  - System remains in IDLE or treats as non-fatal condition.
  - No error indicated (error=false).

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

| Test ID | Requirement | Test Name | Priority |
|---------|------------|-----------|----------|
| TC-001 | SWE-REQ-001, 002 | System Initialization | High |
| TC-003 | SWE-REQ-003, 005, 017 | Upward Movement | High |
| TC-004 | SWE-REQ-005 | Upward at Limit | High |
| TC-005 | SWE-REQ-004, 006 | Downward Movement | High |
| TC-006 | SWE-REQ-006 | Downward at Limit | High |
| TC-009 | CONST-003, SWE-REQ-014 | Dwell Before Reversal | Medium |
| TC-011 | SWE-REQ-010, 011, 015 | Emergency Stop (Both Limits) | High |
| TC-012 | SWE-REQ-010, 011 | Emergency Stop from Down | High |
| TC-013 | SWE-REQ-009 | Power-Off Recovery | Medium |
| TC-014 | SWE-REQ-010, 014 | Simultaneous Buttons | High |
| TC-016 | SWE-REQ-015, 016 | Error Detection | High |
| TC-018 | SWE-REQ-018 | Timeout (Documentation) | Medium |
| TC-020 | SWE-REQ-020 | State Integrity | High |

---

## Notes
- All test cases are implemented in tests/SoftwareTests.cpp using Google Test framework.
- Each test case is traceable to specific SWE-REQ-XXX requirements.
- Integration tests (IT-001 to IT-010) verify system-level behavior in IntegrationTests.cpp.
- Test specifications aligned with implementation as of January 2026.
- For HAL integration verification, see [Software Integration Tests Specification](SoftwareIntegrationTestsSpecification.md).
