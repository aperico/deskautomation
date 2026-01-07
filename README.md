# Automated Mechanical Desk Lift System

Welcome! This project implements a motorized height adjustment system for a mechanical desk using an Arduino-based controller. It is designed for safety, modularity, and future extensibility.

---

## Project Overview
- **Purpose:** Automate desk movement (up/down) with safety and reliability.
- **Technologies:** Arduino UNO, L298N motor driver, modular C++ codebase.
- **Features:**
	- Up/Down movement with button control
	- Indicator LEDs for status
	- Safety checks and error handling
	- Modular hardware abstraction
	- Host-based unit testing
- **Compliance:** ISO 25119, ASPICE

---

## Quick Start
1. **Clone the repository:**
	 ```sh
	 git clone https://github.com/aperico/deskatuomation.git
	 ```
2. **Install prerequisites:**
	 - Arduino IDE or PlatformIO
	 - CMake (for build/test automation)
	 - [Hardware setup](documentation/HardwareConnections.md)
3. **Build the project:**
	 - Use CMake tasks in VS Code or run:
		 ```sh
		 cmake -S . -B build
		 cmake --build build --config Release
		 ```
4. **Flash to Arduino:**
	 - Open `source/arduino/arduino.ino` in Arduino IDE and upload.
5. **Run unit tests:**
	 - Use CTest tasks or run:
		 ```sh
		 ctest --test-dir build -C Release --output-on-failure
		 ```

---


## Documentation Index

## Documentation Index

| Document | Description |
|----------|-------------|
| [Glossary](documentation/Glossary.md) | Key terminology used throughout the project. |
| [Hardware Connections & Pin Assignments](documentation/HardwareConnections.md) | Pin mapping, wiring diagrams, and hardware setup instructions. |
| [Safety Notes](documentation/SafetyNotes.md) | Safety standards, compliance, and operational safety guidelines. |
| [Software Requirements](documentation/SoftwareRequirements.md) | Functional and non-functional requirements for the system. |
| [Software Architecture](documentation/SoftwareArchitecture.md) | High-level architecture, code structure, and system actors. |
| [Software Detailed Design](documentation/SoftwareDetailedDesign.md) | Module decomposition, interfaces, state machine, and compliance details. |
| [System Use Cases](documentation/SystemUseCases.md) | User scenarios and system interactions. |
| [Traceability Matrix](documentation/TraceabilityMatrix.md) | Mapping of use cases, requirements, and test cases for traceability. |
| [Software Test Cases Specification](documentation/SoftwareTestCasesSpecification.md) | Detailed test cases for verifying requirements. |
| [Roadmap](documentation/Roadmap.md) | Planned features and future enhancements. |
| [ASPICE Assessment](documentation/aspiceassessments.md) | ASPICE compliance checklist, process maturity classification, and improvement recommendations. |


---

## ASPICE Assessment Overview

See the [ASPICE Assessment](documentation/aspiceassessments.md) for a detailed checklist of compliance with ASPICE base practices, including system and software engineering. This document provides:
- A table-oriented checklist for SYS and SWE sections
- Status legend for process achievement (Not achieved, Partially achieved, Largely achieved, Fully achieved)
- Evidence mapping to project documentation
- Action items and recommendations for improvement
- Overall process maturity classification

**Overall Process Maturity Statement:**
Most base practices for system and software engineering are fully or largely achieved, with strong evidence in requirements, architecture, and testing. Integration and validation steps are partially manual; further automation and documentation improvements are recommended for full ASPICE compliance.

## Project File Overview

This table provides an overview and description of the main project files and folders, with direct links for easy navigation.

| File/Folder | Description |
|-------------|-------------|
| [source/arduino/arduino.ino](source/arduino/arduino.ino) | Main application entry point for Arduino. Handles setup and main loop. |
| [source/arduino/PinConfig.h](source/arduino/PinConfig.h) | Pin assignments for all hardware connections. |
| [source/arduino/HAL.h](source/arduino/HAL.h), [HAL.cpp](source/arduino/HAL.cpp) | Hardware Abstraction Layer for controlling LEDs, buttons, and motor driver. |
| [source/arduino/DeskController.h](source/arduino/DeskController.h), [DeskController.cpp](source/arduino/DeskController.cpp) | Main logic and state machine for desk movement and safety. |
| [source/arduino/hal_mock/HALMock.h](source/arduino/hal_mock/HALMock.h), [HALMock.cpp](source/arduino/hal_mock/HALMock.cpp) | Mock hardware abstraction for unit testing. |
| [source/arduino/hal_mock/SerialMock.h](source/arduino/hal_mock/SerialMock.h) | Serial interface mock for testing. |
| [tests/UnitTests.cpp](tests/UnitTests.cpp) | Host-based unit tests for core logic and hardware abstraction. |

---
## Project Glossary

| Term            | Definition                                                                 |
|-----------------|---------------------------------------------------------------------------|
| ECU             | Electronic Control Unit; the microcontroller managing desk logic           |
| Desk Controller | The software and hardware module responsible for desk movement and safety  |
| Motor Driver    | Hardware component (e.g., L298N) that powers and controls the motor        |
| State Machine   | Software logic managing system states and transitions                      |
| Error           | A detected fault or unsafe condition that disables movement                |
| Fault           | A hardware or software abnormality (e.g., overcurrent, both limits active)|
| Indicator LED   | Visual feedback device showing system state (IDLE, MOVING_UP, MOVING_DOWN, ERROR) |
| Limit Switch    | Hardware sensor indicating upper or lower desk position                    |
| Dwell           | A short waiting period before reversing desk direction                     |
| Test Case       | A repeatable procedure to verify requirement compliance                    |
| Requirement     | A statement of system behavior, function, or constraint                   |
| Use Case        | A scenario describing user interaction with the system                     |

---

## Development & Contribution
- **Code Structure:**
	- `source/arduino/PinConfig.h` – Pin assignments
	- `source/arduino/HAL.cpp/.h` – Hardware Abstraction Layer
	- `source/arduino/DeskController.cpp/.h` – Main logic & state machine
	- `source/arduino/arduino.ino` – Application entry point
- **Modular Design:** Follow the existing structure for new features.
- **How to contribute:**
	- Fork, branch, and submit pull requests.
	- See [Roadmap](documentation/Roadmap.md) for open tasks.
	- Document any new hardware or features.
- **Coding standards:** Use clear, modular C++ and document public interfaces.

---

## Planned Enhancements
See [Roadmap](documentation/Roadmap.md) for details:
- Upper and lower limit switches
- Current sensing for stall detection
- Height presets
- EEPROM-based calibration
- Soft-start/stop PWM control

---

## Compliance & Safety
- Designed for ISO 25119 (agricultural machinery safety) and ASPICE (automotive software process improvement)
- See [Safety Notes](documentation/SafetyNotes.md) for wiring and operational safety

---

## License
This project is open-source under the MIT License. See [LICENSE](LICENSE) for details.

---

## Contact & Support
- **Questions or issues?** Open an issue on GitHub.


## Out of Scope (Initial Phase)
- Closed-loop position control
