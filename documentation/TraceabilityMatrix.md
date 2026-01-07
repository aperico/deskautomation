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

| Use Case                                         | Software Requirement(s)                                                  |
| ------------------------------------------------ | ------------------------------------------------------------------------ |
| UC-01: Power the Desk Control System             | [SR-01](SoftwareRequirements.md#sr-01)                                      |
| UC-02: Raise Desk                                | [SR-02](SoftwareRequirements.md#sr-02)                                      |
| UC-03: Lower Desk                                | [SR-03](SoftwareRequirements.md#sr-03)                                      |
| UC-04: Emergency Stop (Software-Based or Manual) | [SR-04](SoftwareRequirements.md#sr-04), [SR-08](SoftwareRequirements.md#sr-08) |
| UC-05: Visual Feedback                           | [SR-05](SoftwareRequirements.md#sr-05)                                      |
| UC-06: Power-Off During Movement                 | [SR-06](SoftwareRequirements.md#sr-06)                                      |
| UC-07: Simultaneous Button Presses               | [SR-07](SoftwareRequirements.md#sr-07)                                      |
| UC-08: Error Indication and Recovery             | [SR-08](SoftwareRequirements.md#sr-08)                                      |
| UC-10: Dwell before reversal (Up → Down)        | [SR-02](SoftwareRequirements.md#sr-02), [SR-03](SoftwareRequirements.md#sr-03) |
| **All button-related use cases**           | [SR-09](SoftwareRequirements.md#sr-09)                                      |

---

## Software Requirements ↔ Test Cases (Unit + Integration)

| Software Requirement                                              | Unit Test ID (doc)                                                                                                                                                                                                 | Integration Test ID (code / doc)                                                                                                    |
| ----------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ | ----------------------------------------------------------------------------------------------------------------------------------- |
| [SR-01: System Initialization](SoftwareRequirements.md#sr-01)        | [SR01_TC01_PowerOn_InitializesToIdle](SoftwareTestCasesSpecification.md#sr01_tc01_poweron_initializestoidle)                                                                                                          | [INT_PowerOn_InitializesToIdle](tests/IntegrationTests.cpp)                                                                            |
| [SR-02: Upward Movement Command](SoftwareRequirements.md#sr-02)      | [SR02_TC01_UpPressed_MovesUp](SoftwareTestCasesSpecification.md#sr02_tc01_uppressed_movesup); [SR02_TC04_DwellBeforeReversal_UpToDown](SoftwareTestCasesSpecification.md#sr02_tc04_dwellbeforereversal_uptodown)         | [Smoke_UpMovement_SR02_UC02](tests/IntegrationTests.cpp) ; [Limit_StopOnUpperLimitDuringMoveUp_SR02_UC02](tests/IntegrationTests.cpp)     |
| [SR-03: Downward Movement Command](SoftwareRequirements.md#sr-03)    | [SR03_TC01_DownPressed_MovesDown](SoftwareTestCasesSpecification.md#sr03_tc01_downpressed_movesdown); [SR03_TC04_DwellBeforeReversal_DownToUp](SoftwareTestCasesSpecification.md#sr03_tc04_dwellbeforereversal_downtoup) | [Smoke_DownMovement_SR03_UC03](tests/IntegrationTests.cpp) ; [Limit_StopOnLowerLimitDuringMoveDown_SR03_UC03](tests/IntegrationTests.cpp) |
| [SR-04: Emergency Stop](SoftwareRequirements.md#sr-04)               | [SR04_TC01_BothButtonsPressed_EmergencyStop](SoftwareTestCasesSpecification.md#sr04_tc01_bothbuttonspressed_emergencystop)                                                                                            | [Safety_SimultaneousButtons_NoMovement_SR07_UC07](tests/IntegrationTests.cpp)                                                          |
| [SR-05: Visual Feedback](SoftwareRequirements.md#sr-05)              | [SR05_TC01_StateLEDsReflectSystemState](SoftwareTestCasesSpecification.md#sr05_tc01_stateledsreflectsystemstate)                                                                                                      | [INT_LED_Indicator_Consistency](SoftwareTestCasesSpecification.md#int_led_indicator_consistency)                                       |
| [SR-06: Power-Off Handling](SoftwareRequirements.md#sr-06)           | [SR06_TC01_PowerOffDuringMovement_StopsMovement](SoftwareTestCasesSpecification.md#sr06_tc01_poweroffduringmovement_stopsmovement)                                                                                    | [PowerCycle_StopsAndReturnsIdle](tests/IntegrationTests.cpp)                                                                           |
| [SR-07: Simultaneous Button Presses](SoftwareRequirements.md#sr-07)  | [SR07_TC01_BothButtonsPressed_NoMovement](SoftwareTestCasesSpecification.md#sr07_tc01_bothbuttonspressed_nomovement)                                                                                                  | [Safety_SimultaneousButtons_NoMovement_SR07_UC07](tests/IntegrationTests.cpp)                                                          |
| [SR-08: Error Detection and Recovery](SoftwareRequirements.md#sr-08) | [SR08_TC01_BothLimitsActive_ErrorState](SoftwareTestCasesSpecification.md#sr08_tc01_bothlimitsactive_errorstate)                                                                                                      | [Error_BothLimits_Asserted_SR08_UC08](tests/IntegrationTests.cpp)                                                                      |
| [SR-09: Button Debouncing](SoftwareRequirements.md#sr-09)            | [SR09_TC01_Debounce_PreventsFalseTrigger](SoftwareTestCasesSpecification.md#sr09_tc01_debounce_preventsfalsetrigger)                                                                                                  | [INT_Debounce_PreventsFalseTrigger](SoftwareTestCasesSpecification.md#sr09_tc01_debounce_preventsfalsetrigger)                         |

Notes:

- Unit Test ID links point to the formal test case definitions in SoftwareTestCasesSpecification.md.
- Integration Test ID links point to the integration test source file (tests/IntegrationTests.cpp) or integration test summaries in docs.
- Update IDs/anchors if test names or document headings change.

---

*For questions or suggestions, open an issue or contact the project maintainers.*

---
