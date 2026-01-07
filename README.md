# Automated Mechanical Desk Lift System

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)]()
[![C++](https://img.shields.io/badge/C++-15-blue.svg)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-4.2+-064F8C.svg)](https://cmake.org/)
[![Arduino](https://img.shields.io/badge/Arduino-UNO-00979D.svg)](https://www.arduino.cc/)

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

## Getting Started

Choose your path based on your goals:

### 🔧 I Want to Build the Hardware

**Prerequisites:**
- Arduino UNO board
- L298N motor driver module
- DC motor (12V recommended)
- Push buttons (2x)
- LEDs and resistors
- Power supply (12V)
- See complete parts list in [Hardware Connections](documentation/HardwareConnections.md)

**Steps:**
1. Review the [Hardware Connections Guide](documentation/HardwareConnections.md) for wiring
2. Read the [Safety Notes](documentation/SafetyNotes.md) before assembly
3. Download the firmware: `source/arduino/arduino.ino`
4. Flash to Arduino using Arduino IDE
5. Test your setup!

### 💻 I Want to Contribute Code

**Prerequisites:**
- **Windows:** g++, CMake, Google Test ([Setup Guide](documentation/toolchain.md))
- *```
5. Review [Coding Guidelines](documentation/codingguidelines.md) before contributing

### 🧪 I Want to Test/Explore Without Hardware

You can run all unit and integration tests on your host machine without Arduino hardware:

1. Follow the "Contribute Code" prerequisites above
2. Build and run tests:
   ```sh
   cmake -S . -B build
   cmake --build build --config Release
   ctest --test-dir build -C Release --output-on-failure
   ```
3. Explore the mock HAL in `source/arduino/hal_mock/` to see how hardware is simulated

---

## Development Environment Setup

### Windows Users

For a complete guide on setting up the development toolchain (g++, CMake, Google Test) on Windows, see:

**[Windows Toolchain Setup Guide](documentation/toolchain.md)**

This guide covers:
- Installing MSYS2 via winget
- Setting up g++, CMake, and Google Test
- Configuring your PATH
- Building and testing the project

### Prerequisites

- Arduino IDE or PlatformIO (for flashing firmware to Arduino)
- See [Hardware Connections](documentation/HardwareConnections.md) for wiring setup
- For Windows development: Follow the [Toolchain Setup Guide](documentation/toolchain.md)

---

## Quick Start

1. **Clone the repository:**
   ```sh
   git clone https://github.com/aperico/deskatuomation.git
   ```
2. **Set up your development environment:**
   - **Windows:** Follow the [Windows Toolchain Setup Guide](documentation/toolchain.md)
   - **Linux/Mac:** Install CMake, g++, and Google Test via your package manager
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
| [toolchain.md](documentation/toolchain.md)                                                         | Windows toolchain setup guide (g++, CMake, Google Test via MSYS2).             |
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
ntributing

We welcome contributions! Before submitting code:

1. Read the [Coding Guidelines](documentation/codingguidelines.md) - enforced by CI linting
2. Ensure all tests pass locally
3. Add tests for new features
4. Update documentation as needed
5. Follow the existing code structure and naming conventions

### Code Quality Standards

- **Compliance:** ISO 25119, ASPICE process improvement
- **Testing:** Unit tests required for all new code
- **Documentation:** Keep docs in sync with code changes

---

## FAQ & Troubleshooting

### Build Issues

**Q: CMake configuration fails on Windows**
- Ensure g++ and CMake are in your PATH
- Follow the [Windows Toolchain Setup Guide](documentation/toolchain.md)
- Try cleaning: `Remove-Item -Path ".\build" -Recurse -Force`

**Q: Tests fail to build**
- Verify Google Test is installed
- Check that you're building with Release configuration

**Q: Arduino upload fails**
- Verify correct COM port selected
- Check USB cable connection
- Ensure Arduino drivers are installed

### Hardware Issues

**Q: Motor doesn't respond**
- Check L298N power supply (12V connected)
- Verify wiring against [Hardware Connections](documentation/HardwareConnections.md)
- Test button functionality with LED indicators

**Q: Desk moves but stutters**
- Check motor power supply voltage/amperage
- Inspect mechanical crank for binding
- Review [Safety Notes](documentation/SafetyNotes.md)

For more issues, check existing [GitHub Issues](https://github.com/aperico/deskatuomation/issues) or open a new one.

---

## Compliance & Safety

- Designed following ISO 25119 (agricultural machinery safety) principles
- ASPICE-aligned software development process
- **⚠️ Important:** Review [Safety Notes](documentation/SafetyNotes.md) before assembly and operation

---

## For Developers: Technical Documentation

### Project File Overview

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

## License

This project is open-source under the MIT License. See [LICENSE](LICENSE) for details.

---

## Contact & Support

- **Questions or issues?** Open an issue on [GitHub Issues](https://github.com/aperico/deskatuomation/issues)
- **Discussions:** Use GitHub Discussions for general questions
- **Security Issues:** Please report privately via GitHub Security Advisories