# Project Glossary

This glossary contains project-specific terms used throughout the repository. Keep entries concise and update README.md when adding new terms.

| Term            | Definition                                                                                                                                                                                                                                         |
| --------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| ECU             | Electronic Control Unit; the microcontroller managing desk logic                                                                                                                                                                                   |
| Debounce        | The process of filtering out rapid, unintended changes in a switch's electrical signal caused by mechanical bounce, ensuring only deliberate position changes are registered. Implemented in this project via the `HAL_debounceSwitch` function. |
| Desk Controller | The software and hardware module responsible for desk movement and safety                                                                                                                                                                          |
| Motor Driver    | Hardware component (e.g., BTS7960/IBT-2) that powers and controls the motor                                                                                                                                                                                |
| State Machine   | Software logic managing system states and transitions                                                                                                                                                                                              |
| Error           | A detected fault or unsafe condition that disables movement                                                                                                                                                                                        |
| Fault           | A hardware or software abnormality (e.g., overcurrent, both limits active)                                                                                                                                                                         |
| Rocker Switch   | Three-position input device (UP, OFF, DOWN) controlling desk movement direction                                                                                                                                                                                  |
| Limit Switch    | Hardware sensor indicating upper or lower desk position                                                                                                                                                                                            |
| Dwell           | A short waiting period before reversing desk direction                                                                                                                                                                                             |
| Test Case       | A repeatable procedure to verify requirement compliance                                                                                                                                                                                            |
| Requirement     | A statement of system behavior, function, or constraint                                                                                                                                                                                            |
| Use Case        | A scenario describing user interaction with the system                                                                                                                                                                                             |

---

*For questions about terminology, see the README or open an issue.*
