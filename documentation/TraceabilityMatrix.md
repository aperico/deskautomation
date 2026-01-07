

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
| UC-01: Power the Desk Control System | [SR-01](SoftwareRequirements.md#sr-01) |
| UC-02: Raise Desk | [SR-02](SoftwareRequirements.md#sr-02) |
| UC-03: Lower Desk | [SR-03](SoftwareRequirements.md#sr-03) |
| UC-04: Emergency Stop (Software-Based or Manual) | [SR-04](SoftwareRequirements.md#sr-04), [SR-08](SoftwareRequirements.md#sr-08) |
| UC-05: Visual Feedback | [SR-05](SoftwareRequirements.md#sr-05) |
| UC-06: Power-Off During Movement | [SR-06](SoftwareRequirements.md#sr-06) |
| UC-07: Simultaneous Button Presses | [SR-07](SoftwareRequirements.md#sr-07) |
| UC-08: Error Indication and Recovery | [SR-08](SoftwareRequirements.md#sr-08) |
| UC-10: Dwell before reversal (Up → Down) | [SR-02](SoftwareRequirements.md#sr-02), [SR-03](SoftwareRequirements.md#sr-03) |

---

## Software Requirements <-> Software Test Cases
| Software Requirement | Software Test Case(s) |
|---|---|
| [SR-01: System Initialization](SoftwareRequirements.md#sr-01) | [SR01_TC01_PowerOn_InitializesToIdle](SoftwareTestCasesSpecification.md#sr01_tc01_poweron_initializestoidle); [SR01_TC02_NoButtonsPressed_IdleNoMovement](SoftwareTestCasesSpecification.md#sr01_tc02_nobuttonspressed_idlenomovement) |
| [SR-02: Upward Movement Command](SoftwareRequirements.md#sr-02) | [SR02_TC01_UpPressed_MovesUp](SoftwareTestCasesSpecification.md#sr02_tc01_uppressed_movesup); [SR02_TC02_UpPressed_AtUpperLimit_NoMovement](SoftwareTestCasesSpecification.md#sr02_tc02_uppressed_atupperlimit_nomovement); [SR02_TC03_UpReleased_StopsMovement](SoftwareTestCasesSpecification.md#sr02_tc03_upreleased_stopsmovement); [SR02_TC04_DwellBeforeReversal_UpToDown](SoftwareTestCasesSpecification.md#sr02_tc04_dwellbeforereversal_uptodown) |
| [SR-03: Downward Movement Command](SoftwareRequirements.md#sr-03) | [SR03_TC01_DownPressed_MovesDown](SoftwareTestCasesSpecification.md#sr03_tc01_downpressed_movesdown); [SR03_TC02_DownPressed_AtLowerLimit_NoMovement](SoftwareTestCasesSpecification.md#sr03_tc02_downpressed_atlowerlimit_nomovement); [SR03_TC03_DownReleased_StopsMovement](SoftwareTestCasesSpecification.md#sr03_tc03_downreleased_stopsmovement); [SR03_TC04_DwellBeforeReversal_DownToUp](SoftwareTestCasesSpecification.md#sr03_tc04_dwellbeforereversal_downtoup) |
| [SR-04: Emergency Stop](SoftwareRequirements.md#sr-04) | [SR04_TC01_BothButtonsPressed_EmergencyStop](SoftwareTestCasesSpecification.md#sr04_tc01_bothbuttonspressed_emergencystop); [SR04_TC02_FaultDetected_EmergencyStop](SoftwareTestCasesSpecification.md#sr04_tc02_faultdetected_emergencystop); [SR04_TC03_ClearErrorByPowerCycle](SoftwareTestCasesSpecification.md#sr04_tc03_clearerrorbypowercycle) |
| [SR-05: Visual Feedback](SoftwareRequirements.md#sr-05) | [SR05_TC01_StateLEDsReflectSystemState](SoftwareTestCasesSpecification.md#sr05_tc01_stateledsreflectsystemstate) |
| [SR-06: Power-Off Handling](SoftwareRequirements.md#sr-06) | [SR06_TC01_PowerOffDuringMovement_StopsMovement](SoftwareTestCasesSpecification.md#sr06_tc01_poweroffduringmovement_stopsmovement); [SR06_TC02_PowerRestore_ReinitializesToIdle](SoftwareTestCasesSpecification.md#sr06_tc02_powerrestore_reinitializestoidle) |
| [SR-07: Simultaneous Button Presses](SoftwareRequirements.md#sr-07) | [SR07_TC01_BothButtonsPressed_NoMovement](SoftwareTestCasesSpecification.md#sr07_tc01_bothbuttonspressed_nomovement); [SR07_TC02_BothButtonsPressed_WhileMoving_StopsMovement](SoftwareTestCasesSpecification.md#sr07_tc02_bothbuttonspressed_whilemoving_stopsmovement); [SR07_TC03_ReleaseBothButtons_ResumesNormalOperation](SoftwareTestCasesSpecification.md#sr07_tc03_releasebothbuttons_resumesnormaloperation) |
| [SR-08: Error Detection and Recovery](SoftwareRequirements.md#sr-08) | [SR08_TC01_BothLimitsActive_ErrorState](SoftwareTestCasesSpecification.md#sr08_tc01_bothlimitsactive_errorstate); [SR08_TC02_Overcurrent_ErrorState](SoftwareTestCasesSpecification.md#sr08_tc02_overcurrent_errorstate); [SR08_TC03_ClearErrorByPowerCycle](SoftwareTestCasesSpecification.md#sr08_tc03_clearerrorbypowercycle) |

---

*For questions or suggestions, open an issue or contact the project maintainers.*

---
