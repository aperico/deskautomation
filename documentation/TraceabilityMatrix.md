

# Traceability Matrix

This matrix maps use cases to requirements and requirements to test cases for the Automated Mechanical Desk Lift system. It ensures clear traceability and alignment with ASPICE standards.

---


## Navigation
- [System Use Cases](SystemUseCases.md)
- [Software Requirements](SoftwareRequirements.md)
- [Software Test Cases Specification](SoftwareTestCasesSpecification.md)

---

## Requirement ID Usage
All requirements are referenced by their unique IDs (SR-01, SR-02, etc.) throughout the documentation, diagrams, and design sections.

Refer to [Software Requirements](SoftwareRequirements.md) for full requirement text and rationale.

---

## Use Cases <-> Software Requirements
| Use Case | Software Requirement(s) |
|---|---|
| UC-01: Power the Desk Control System | SR-01 |
| UC-02: Raise Desk | SR-02 |
| UC-03: Lower Desk | SR-03 |
| UC-04: Emergency Stop (Software-Based or Manual) | SR-04, SR-08 |
| UC-05: Visual Feedback | SR-05 |
| UC-06: Power-Off During Movement | SR-06 |
| UC-07: Simultaneous Button Presses | SR-07 |
| UC-08: Error Indication and Recovery | SR-08 |
| UC-10: Dwell before reversal (Up → Down) | SR-02, SR-03 |

---

## Software Requirements <-> Software Test Cases
| Software Requirement | Software Test Case(s) |
|---|---|
| SR-01: System Initialization | SR01_TC01_PowerOn_InitializesToIdle; SR01_TC02_NoButtonsPressed_IdleNoMovement |
| SR-02: Upward Movement Command | SR02_TC01_UpPressed_MovesUp; SR02_TC02_UpPressed_AtUpperLimit_NoMovement; SR02_TC03_UpReleased_StopsMovement; SR02_TC04_DwellBeforeReversal_UpToDown |
| SR-03: Downward Movement Command | SR03_TC01_DownPressed_MovesDown; SR03_TC02_DownPressed_AtLowerLimit_NoMovement; SR03_TC03_DownReleased_StopsMovement; SR03_TC04_DwellBeforeReversal_DownToUp |
| SR-04: Emergency Stop | SR04_TC01_BothButtonsPressed_EmergencyStop; SR04_TC02_FaultDetected_EmergencyStop; SR04_TC03_ClearErrorByPowerCycle |
| SR-05: Visual Feedback | SR05_TC01_StateLEDsReflectSystemState |
| SR-06: Power-Off Handling | SR06_TC01_PowerOffDuringMovement_StopsMovement; SR06_TC02_PowerRestore_ReinitializesToIdle |
| SR-07: Simultaneous Button Presses | SR07_TC01_BothButtonsPressed_NoMovement; SR07_TC02_BothButtonsPressed_WhileMoving_StopsMovement; SR07_TC03_ReleaseBothButtons_ResumesNormalOperation |
| SR-08: Error Detection and Recovery | SR08_TC01_BothLimitsActive_ErrorState; SR08_TC02_Overcurrent_ErrorState; SR08_TC03_ClearErrorByPowerCycle |

---

*For questions or suggestions, open an issue or contact the project maintainers.*

---
