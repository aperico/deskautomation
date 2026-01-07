# Automated Mechanical Desk Lift System

Welcome! This project implements a motorized height adjustment system for a mechanical desk using an Arduino-based controller. It is designed for safety, modularity, and future extensibility.

<p align="center">
  <img src="documentation/deskcrank.jpg" alt="Desk Crank System" width="220"/>
</p>

---

## Project Overview

- **Purpose:** Automate desk movement (up/down) with safety and reliability.
- **Technologies:** Arduino UNO, L298N motor driver, modular C++ codebase.
- **Features:**
  - Up/Down movement with button control
  - Host-based unit testing
- **Compliance:** ISO 25119, ASPICE

---

## Coding Standards

All contributors must follow the [Coding Guidelines](documentation/codingguidelines.md) to ensure code quality and consistency. These guidelines are enforced by automated linting and static analysis tools in CI.

---

## Quick Start

1. **Clone the repository:**
   ```sh
   git clone https://github.com/aperico/deskatuomation.git
   ```
2. **Install prerequisites:**
   - Arduino IDE or PlatformIO
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

---

## ASPICE Assessment Overview

See the [ASPICE Assessment](documentation/aspiceassessments.md) for a detailed checklist of compliance with ASPICE base practices, including system and software engineering. This document provides:

- A table-oriented checklist for SYS and SWE sections
- Status legend for process achievement (Not achieved, Partially achieved, Largely achieved, Fully achieved)
- Evidence mapping to project documentation
- Action items and recommendations for improvement
- Overall process maturity classification

## Documentation Index

Below is a table listing major `.md` documents and integration test specs/artifacts:

| Document                                                                                        | Description                                                                     |
| ----------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------- |
| [README.md](README.md)                                                                             | Project overview, quick start, file map, glossary, and contribution guidelines. |
| [HardwareConnections.md](documentation/HardwareConnections.md)                                     | Hardware setup, wiring, and pin assignments.                                    |
| [SoftwareRequirements.md](documentation/SoftwareRequirements.md)                                   | Functional, safety, and interface requirements.                                 |
| [SoftwareTestCasesSpecification.md](documentation/SoftwareTestCasesSpecification.md)               | Detailed software test cases (unit + system).                                   |
| [SoftwareIntegrationTestsSpecification.md](documentation/SoftwareIntegrationTestsSpecification.md) | Integration test specification (Application + HAL).                             |
| [SystemUseCases.md](documentation/SystemUseCases.md)                                               | User scenarios and flows driving requirements.                                  |
| [TraceabilityMatrix.md](documentation/TraceabilityMatrix.md)                                       | Mapping between use cases, requirements, and test cases.                        |
| [SoftwareArchitecture.md](documentation/SoftwareArchitecture.md)                                   | High-level software structure and architectural views.                          |
| [SoftwareDetailedDesign.md](documentation/SoftwareDetailedDesign.md)                               | In-depth design details and rationale.                                          |
| [aspiceassessments.md](documentation/aspiceassessments.md)                                         | ASPICE process maturity assessment and checklist.                               |
| [Glossary.md](documentation/Glossary.md)                                                           | Project glossary and term definitions.                                          |

---

## Project File Overview

| File/Folder                                                                                | Description                                             |
| ------------------------------------------------------------------------------------------ | ------------------------------------------------------- |
| [source/arduino/arduino.ino](source/arduino/arduino.ino)                                      | Main Arduino entry point (setup + loop).                |
| [source/arduino/PinConfig.h](source/arduino/PinConfig.h)                                      | Pin assignments.                                        |
| [source/arduino/HAL.h / HAL.cpp](source/arduino/HAL.h)                                        | Hardware Abstraction Layer (LEDs, buttons, motor).      |
| [source/arduino/DeskController.h / DeskController.cpp](source/arduino/DeskController.h)       | Application logic & state machine.                      |
| [source/arduino/hal_mock/HALMock.h / HALMock.cpp](source/arduino/hal_mock/HALMock.h)          | HAL mock for host tests (millis, pin state).            |
| [source/arduino/hal_mock/SerialMock.h / SerialMock.cpp](source/arduino/hal_mock/SerialMock.h) | Serial mock for tests.                                  |
| [tests/SoftwareTests.cpp](tests/SoftwareTests.cpp)                                            | Unit tests (gtest) for application logic.               |
| [tests/IntegrationTests.cpp](tests/IntegrationTests.cpp)                                      | Integration tests exercising Application + HAL (gtest). |
| [tests/Integration_SmokeTests.cpp](tests/Integration_SmokeTests.cpp)                          | Additional integration smoke tests (gtest).             |
| [build/](build/)                                                                              | CMake build output and test execution artifacts (CI).   |

---

## Integration Test Artifacts

- Integration test source:
  - tests/IntegrationTests.cpp
  - tests/Integration_SmokeTests.cpp
  - documentation/SoftwareIntegrationTestsSpecification.md
- Traceability & mapping:
  - documentation/TraceabilityMatrix.md
- CI / build artifacts (created by CI / ctest):
  - build/test-results-integration.xml
  - build/Testing/**/Test.xml (Integration labeled runs)
- How to run integration tests locally:
  1. cmake -S . -B build
  2. cmake --build build --config Release
  3. ctest --test-dir build -C Release -L Integration --output-on-failure

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
