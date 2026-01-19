# Automated Mechanical Desk Lift System

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)]()
[![C++](https://img.shields.io/badge/C++-15-blue.svg)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-4.2+-064F8C.svg)](https://cmake.org/)
[![Arduino](https://img.shields.io/badge/Arduino-UNO-00979D.svg)](https://www.arduino.cc/)

Automated height adjustment system for a mechanical desk using an Arduino-based controller. Designed for safety, modularity, and extensibility.

**Key Features:** Motor-driven up/down movement, rocker switch control, comprehensive testing, ASPICE-aligned development.

---

## Quick Navigation

- **Want to understand the project?** → [Project Overview](#project-overview)
- **Building hardware?** → [Hardware Setup](#hardware-setup)
- **Writing code?** → [Development Setup](#development-setup)
- **Running tests?** → [Testing](#testing)
- **Need documentation?** → [Documentation Index](#documentation-index)

---

## Project Overview

This project automates desk height adjustment using:
- **Arduino UNO** microcontroller
- **BTS7960/IBT-2** motor driver (12V/24V)
- **Rocker switch** input (3-position)
- **Modular C++ architecture** with comprehensive tests

**Compliance:** ISO 25119 (safety), ASPICE (software process)

**Current Version:** v1.0 (basic movement + testing framework)

---

## Hardware Setup

### Prerequisites
- Arduino UNO board
- BTS7960/IBT-2 motor driver
- 31ZY-5840 DC Worm Gear Motor (12V/24V)
- Rocker switch (3-position)
- Power supply (12V or 24V, 5A+)

### Getting Started
1. Review [Hardware Schematic](documentation/Schematic.md) for wiring
2. Read [Safety Notes](documentation/SafetyNotes.md) before assembly
3. Download firmware: `source/arduino/arduino.ino`
4. Flash to Arduino using Arduino IDE
5. Test your setup

---

## Development Setup

### Windows Setup (Recommended)

Follow the [Windows Toolchain Setup Guide](documentation/13_Toolchain.md) to install:
- MSYS2 & g++
- CMake 3.x
- Google Test framework

### Other Platforms
- **Linux/Mac:** Install `g++`, `cmake`, `googletest` via your package manager

### Quick Start
```bash
# 1. Clone repository
git clone https://github.com/aperico/deskatuomation.git
cd deskatuomation

# 2. Create build directory
cmake -S . -B build

# 3. Build project
cmake --build build --config Release

# 4. Run tests (verify setup)
ctest --test-dir build -C Release --output-on-failure
```

---

## Testing

### Run All Tests
```bash
ctest --test-dir build -C Release --output-on-failure
```

### Test Types
- **Unit Tests (3):** Basic functionality
- **Component Tests (13):** DeskController logic
- **Integration Tests (9):** System-level workflows

**Result:** 25/25 tests passing ✅

### Running Without Hardware
All tests run on host machine with simulated hardware (no Arduino needed).

See [Component Test Spec](documentation/10_ComponentTestCasesSpecification.md) and [Integration Test Spec](documentation/11_SoftwareIntegrationTestsSpecification.md) for details.

---

## CI/CD Pipeline

The project uses an automated **CMake CI workflow** (GitHub Actions) that runs on every push and pull request to `main` branch. Here's what each stage does:

### Pipeline Stages

**1. Checkout & Dependencies**
- Clones repository code
- Installs build tools: `cmake`, `g++`, `gcc`, `lcov`, `cppcheck`
- Builds and installs Google Test framework from source

**2. Static Analysis (Cppcheck)**
- Runs `cppcheck --enable=all` on all source code
- Checks for MISRA-like violations and code quality issues
- Generates XML report and uploads as artifact
- Non-blocking: warnings don't fail the build

**3. CMake Configure**
- Runs `cmake -B build` to generate build system
- Enables code coverage flags (`--coverage` for gcov)
- Configures for Debug mode (necessary for coverage)

**4. Build**
- Compiles source code and test executable with `cmake --build`
- Links against Google Test framework
- Creates executable in `build/` directory

**5. Unit Tests**
- Runs only tests labeled "Unit" (basic functionality)
- Generates JUnit XML result file
- Output shows individual test results
- **Status:** 3/3 passing ✅

**6. Integration Tests**
- Runs only tests labeled "Integration" (component interaction)
- Generates separate JUnit XML result file
- Verifies system-level workflows
- **Status:** 9/9 passing ✅

**7. Component Tests**
- Runs component-level tests (DeskController logic)
- Part of overall test suite validation
- **Status:** 13/13 passing ✅

**8. Code Coverage Report**
- Runs `geninfo` and `lcov` to collect coverage metrics
- Generates `coverage.info` file showing line/branch coverage
- Filters to source code only (excludes tests and external libs)
- Lists coverage summary in build log

**9. Artifact Upload**
- Uploads Cppcheck XML report
- Uploads JUnit test result files (unit + integration)
- Uploads lcov coverage report
- All artifacts stored in GitHub Actions for 90 days

### Key Features

- **Automated on every commit:** Catches issues immediately
- **Parallel execution:** Unit and integration tests run separately for clarity
- **Coverage tracking:** Generates metrics for code quality assessment
- **Multiple quality gates:** Static analysis + unit tests + integration tests
- **Artifact preservation:** Results available for download and analysis

---



Before contributing:

1. Review [Coding Guidelines](documentation/14_Codingguidelines.md)
2. Ensure tests pass locally
3. Add tests for new features
4. Update documentation
5. Follow existing code style

**Code Quality:** Unit tests required, ASPICE-aligned process, comprehensive traceability.

---

## Documentation Index

| Document | Purpose |
|----------|---------|
| [01_MissionStatement.md](documentation/01_MissionStatement.md) | Project mission & vision |
| [02_SystemObjectives.md](documentation/02_SystemObjectives.md) | System-level objectives & goals |
| [03_ConOps.md](documentation/03_ConOps.md) | Concept of operations & use scenarios |
| [04_SystemContextDiagram.md](documentation/04_SystemContextDiagram.md) | System context & external interfaces |
| [05_SystemUseCases.md](documentation/05_SystemUseCases.md) | User scenarios & workflows |
| [06_SystemRequirements.md](documentation/06_SystemRequirements.md) | System-level requirements |
| [07_SoftwareRequirements.md](documentation/07_SoftwareRequirements.md) | Functional & safety requirements |
| [08_SoftwareArchitecture.md](documentation/08_SoftwareArchitecture.md) | System design & components |
| [09_SoftwareDetailedDesign.md](documentation/09_SoftwareDetailedDesign.md) | Implementation details |
| [10_ComponentTestCasesSpecification.md](documentation/10_ComponentTestCasesSpecification.md) | Component test guide & examples |
| [11_SoftwareIntegrationTestsSpecification.md](documentation/11_SoftwareIntegrationTestsSpecification.md) | Integration test guide & examples |
| [12_TraceabilityMatrix.md](documentation/12_TraceabilityMatrix.md) | Requirements ↔ Architecture ↔ Tests mapping |
| [13_Toolchain.md](documentation/13_Toolchain.md) | Windows development setup |
| [14_Codingguidelines.md](documentation/14_Codingguidelines.md) | Code style & standards |
| [aspice/aspiceassessments.md](documentation/aspice/aspiceassessments.md) | ASPICE compliance checklist |
| [Glossary.md](documentation/Glossary.md) | Term definitions |
| [SafetyNotes.md](documentation/SafetyNotes.md) | Safety & operational guidelines |
| [Schematic.md](documentation/Schematic.md) | Hardware wiring & connections |

---

## Project Structure

```
source/arduino/          # Arduino firmware & HAL
  ├── arduino.ino        # Entry point (setup/loop)
  ├── DeskController.*   # Application logic & state machine
  ├── HAL.*              # Hardware abstraction layer
  ├── PinConfig.h        # Pin definitions
  └── hal_mock/          # Test mocks (no hardware needed)
  
tests/                   # Comprehensive test suites
  ├── ComponentTests.cpp      # Component-level tests
  ├── IntegrationTests.cpp    # System integration tests
  └── UnitTests.cpp           # Basic unit tests

documentation/           # Complete project documentation
  ├── 05_SystemUseCases.md
  ├── 07_SoftwareRequirements.md
  ├── 08_SoftwareArchitecture.md
  ├── 10_ComponentTestCasesSpecification.md
  ├── 11_SoftwareIntegrationTestsSpecification.md
  ├── 12_TraceabilityMatrix.md
  └── ...
```

---

## Troubleshooting

### Build Fails
- **Windows:** Verify CMake/g++ in PATH, follow [Toolchain Setup](documentation/13_Toolchain.md)
- **Google Test issues:** Re-run `cmake -S . -B build`
- **Clean rebuild:** `Remove-Item -Path ".\build" -Recurse -Force`

### Tests Fail
- Ensure Release build: `cmake --build build --config Release`
- Check no hardware is required (all tests use mocks)

### Motor Issues
- Verify 12V power supply to BTS7960
- Check wiring against [Hardware Schematic](documentation/Schematic.md)
- Test rocker switch with multimeter

---

## Compliance & Safety

- **ISO 25119:** Agricultural machinery safety principles
- **ASPICE:** Software engineering process (SWE.3-SWE.6)
- **Safety Critical:** Review [Safety Notes](documentation/SafetyNotes.md) before assembly

---

## Roadmap

See [Roadmap](documentation/Roadmap.md) for v2.0 enhancements:
- Upper/lower limit switches
- Current sensing for stall detection
- Height presets
- EEPROM calibration
- Soft-start/stop control

---

## License & Support

- **License:** MIT License ([LICENSE](LICENSE))
- **Issues:** [GitHub Issues](https://github.com/aperico/deskatuomation/issues)
- **Discussions:** GitHub Discussions
- **Security:** Report privately via GitHub Security Advisories