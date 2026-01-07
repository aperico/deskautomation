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
- [Glossary](documentation/Glossary.md) – Review terminology before starting
- [Hardware Connections & Pin Assignments](documentation/HardwareConnections.md)
- [Safety Notes](documentation/SafetyNotes.md)
- [Software Requirements](documentation/SoftwareRequirements.md)
- [Software Architecture](documentation/SoftwareArchitecture.md)
- [Software Detailed Design](documentation/SoftwareDetailedDesign.md)
- [System Use Cases](documentation/SystemUseCases.md)
- [Traceability Matrix](documentation/TraceabilityMatrix.md)
- [Software Test Cases Specification](documentation/SoftwareTestCasesSpecification.md)
- [Roadmap](documentation/Roadmap.md)

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
- **Glossary:** See [Glossary](documentation/Glossary.md) for terminology.
# Automated Mechanical Desk Lift

This project implements a **motorized height adjustment system** for a mechanical desk using an **Arduino-based controller**.  
The architecture is designed for:
- Temporary hardware during early development (JGY370 + L298N)
- Future extensibility (limit switches, current sensing, height presets)

---


- [Glossary](documentation/Glossary.md)
- [Hardware Connections & Pin Assignments](documentation/HardwareConnections.md)
- [Safety Notes](documentation/SafetyNotes.md)
- [Planned Enhancements (Roadmap)](documentation/Roadmap.md)
---



## Out of Scope (Initial Phase)
- Closed-loop position control

---

## System Actors
See [Software Architecture](documentation/SoftwareArchitecture.md#system-actors) for a description of the main actors and their roles in the system.
---

- [Software Architecture](documentation/SoftwareArchitecture.md)
- [Software Detailed Design](documentation/SoftwareDetailedDesign.md)
- [Software Test Cases Specification](documentation/SoftwareTestCasesSpecification.md)
- [Traceability Matrix](documentation/TraceabilityMatrix.md)
---

- Upper and lower limit switches
- Current sensing for stall detection
- Height presets
- EEPROM-based calibration
---
## Architecture
See [Software Architecture](documentation/SoftwareArchitecture.md) for a detailed description of the system's modular structure, hardware abstraction, and control logic.



## Application States and State Transitions
---
## Wiring Diagrams
See [Hardware Connections & Pin Assignments](documentation/HardwareConnections.md#wiring-diagrams) for wiring diagrams and safety notes.
## Traceability
See [Traceability Matrix](documentation/TraceabilityMatrix.md) for requirement-to-test mapping and use case coverage.

---

- Document any new hardware or features.
- Follow the existing modular structure for new code.
