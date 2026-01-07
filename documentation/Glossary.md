
# Project Glossary

This glossary defines key terms used throughout the Automated Mechanical Desk Lift documentation. Review this section before onboarding or contributing to ensure clarity and consistency across the project.

## How to Use
- Refer to this glossary whenever you encounter unfamiliar terminology in requirements, design, or code comments.
- If you add new terms to the project, update this file to keep documentation consistent.

## Terms
| Term                | Definition |
|---------------------|------------|
| **ECU**             | Electronic Control Unit; the microcontroller managing desk logic |
| **Desk Controller** | The software and hardware module responsible for desk movement and safety |
| **Motor Driver**    | Hardware component (e.g., L298N) that powers and controls the motor |
| **State Machine**   | Software logic managing system states and transitions |
| **Error**           | A detected fault or unsafe condition that disables movement |
| **Fault**           | A hardware or software abnormality (e.g., overcurrent, both limits active) |
| **Indicator LED**   | Visual feedback device showing system state (IDLE, MOVING_UP, MOVING_DOWN, ERROR) |
| **Limit Switch**    | Hardware sensor indicating upper or lower desk position |
| **Dwell**           | A short waiting period before reversing desk direction |
| **Test Case**       | A repeatable procedure to verify requirement compliance |
| **Requirement**     | A statement of system behavior, function, or constraint |
| **Use Case**        | A scenario describing user interaction with the system |

---

*If you have questions about terminology, open an issue or consult the project maintainers.*
