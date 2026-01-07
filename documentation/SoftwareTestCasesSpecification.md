
# Software Test Cases Specification

This document lists the test cases for the Automated Mechanical Desk Lift system. Test cases are derived from [Software Requirements](SoftwareRequirements.md) and are designed to be clear, repeatable, and verifiable.

---


## Navigation
- [System Use Cases](SystemUseCases.md)
- [Software Requirements](SoftwareRequirements.md)
- [Traceability Matrix](TraceabilityMatrix.md)

---

## Test Case Structure
Each test case includes preconditions, steps, and expected results. Update this section as requirements and implementation evolve.

## SR-01: System Initialization
**Test Case: SR01_TC01_PowerOn_InitializesToIdle**
- Precondition: System is powered off.
- Steps:
  1. Power on the system.
- Expected Result:
  - All input/output variables are set to default.
  - System state is IDLE.
  - Movement outputs are inactive.
  - Ready status indicator is ON.

**Test Case: SR01_TC02_NoButtonsPressed_IdleNoMovement**
- Precondition: System is powered on and in IDLE.
- Steps:
  1. Ensure no buttons are pressed.
- Expected Result:
  - No movement is commanded.
  - No error is indicated.

---

## SR-02: Upward Movement Command
**Test Case: SR02_TC01_UpPressed_MovesUp**
- Precondition: System is in IDLE, not at upper limit.
- Steps:
  1. Press Up button.
- Expected Result:
  - Upward movement is commanded.
  - Up indicator LED is ON.

**Test Case: SR02_TC02_UpPressed_AtUpperLimit_NoMovement**
- Precondition: System is in IDLE, upper limit is active.
- Steps:

---

*For questions or suggestions, open an issue or contact the project maintainers.*
  1. Press Up button.
- Expected Result:
  - No movement is commanded.
  - System remains in IDLE.

**Test Case: SR02_TC03_UpReleased_StopsMovement**
- Precondition: System is moving up.
- Steps:
  1. Release Up button.
- Expected Result:
  - Movement stops.
  - Indicator LED returns to IDLE.

**Test Case: SR02_TC04_DwellBeforeReversal_UpToDown**
- Precondition: System is moving up.
- Steps:
  1. Release Up button.
  2. Immediately press Down button.
- Expected Result:
  - System dwells before reversing direction.
  - No immediate downward movement.

---

## SR-03: Downward Movement Command
**Test Case: SR03_TC01_DownPressed_MovesDown**
- Precondition: System is in IDLE, not at lower limit.
- Steps:
  1. Press Down button.
- Expected Result:
  - Downward movement is commanded.
  - Down indicator LED is ON.

**Test Case: SR03_TC02_DownPressed_AtLowerLimit_NoMovement**
- Precondition: System is in IDLE, lower limit is active.
- Steps:
  1. Press Down button.
- Expected Result:
  - No movement is commanded.
  - System remains in IDLE.

**Test Case: SR03_TC03_DownReleased_StopsMovement**
- Precondition: System is moving down.
- Steps:
  1. Release Down button.
- Expected Result:
  - Movement stops.
  - Indicator LED returns to IDLE.

**Test Case: SR03_TC04_DwellBeforeReversal_DownToUp**
- Precondition: System is moving down.
- Steps:
  1. Release Down button.
  2. Immediately press Up button.
- Expected Result:
  - System dwells before reversing direction.
  - No immediate upward movement.

---

## SR-04: Emergency Stop
**Test Case: SR04_TC01_BothButtonsPressed_EmergencyStop**
- Precondition: System is moving.
- Steps:
  1. Press both Up and Down buttons simultaneously.
- Expected Result:
  - All movement outputs are deactivated.
  - System state is ERROR.
  - Error indicator LED is ON.

**Test Case: SR04_TC02_FaultDetected_EmergencyStop**
- Precondition: System is moving.
- Steps:
  1. Simulate fault or timeout condition.
- Expected Result:
  - All movement outputs are deactivated.
  - System state is ERROR.
  - Error indicator LED is ON.

**Test Case: SR04_TC03_ClearErrorByPowerCycle**
- Precondition: System is in ERROR state.
- Steps:
  1. Cycle system power.
- Expected Result:
  - Error state is cleared.
  - System returns to IDLE.

---

## SR-05: Visual Feedback
**Test Case: SR05_TC01_StateLEDsReflectSystemState**
- Precondition: System is powered on.
- Steps:
  1. Change system state (IDLE, MOVING_UP, MOVING_DOWN, ERROR).
- Expected Result:
  - Corresponding LED is ON for each state.

---

## SR-06: Power-Off Handling
**Test Case: SR06_TC01_PowerOffDuringMovement_StopsMovement**
- Precondition: System is moving.
- Steps:
  1. Power off the system.
- Expected Result:
  - All movement outputs are deactivated.

**Test Case: SR06_TC02_PowerRestore_ReinitializesToIdle**
- Precondition: System was powered off during movement.
- Steps:
  1. Power on the system.
- Expected Result:
  - System reinitializes to IDLE.
  - No movement is commanded.

---

## SR-07: Simultaneous Button Presses
**Test Case: SR07_TC01_BothButtonsPressed_NoMovement**
- Precondition: System is in IDLE.
- Steps:
  1. Press both Up and Down buttons simultaneously.
- Expected Result:
  - No movement is commanded.
  - System remains in IDLE.

**Test Case: SR07_TC02_BothButtonsPressed_WhileMoving_StopsMovement**
- Precondition: System is moving.
- Steps:
  1. Press both Up and Down buttons simultaneously.
- Expected Result:
  - Movement stops.
  - System remains in IDLE or enters ERROR if limits are active.

**Test Case: SR07_TC03_ReleaseBothButtons_ResumesNormalOperation**
- Precondition: System stopped due to both buttons pressed.
- Steps:
  1. Release both buttons.
- Expected Result:
  - System resumes normal operation.

---

## SR-08: Error Detection and Recovery
**Test Case: SR08_TC01_BothLimitsActive_ErrorState**
- Precondition: System is powered on.
- Steps:
  1. Activate both upper and lower limits.
- Expected Result:
  - System state is ERROR.
  - Error indicator LED is ON.

**Test Case: SR08_TC02_Overcurrent_ErrorState**
- Precondition: System is powered on.
- Steps:
  1. Simulate overcurrent event.
- Expected Result:
  - System state is ERROR.
  - Error indicator LED is ON.

**Test Case: SR08_TC03_ClearErrorByPowerCycle**
- Precondition: System is in ERROR state.
- Steps:
  1. Cycle system power.
- Expected Result:
  - Error state is cleared.
  - System returns to IDLE.

---

## Notes
- All test cases are designed to be repeatable and verifiable.
- Each test case is traceable to a specific software requirement.
- Test cases should be updated if requirements change.
