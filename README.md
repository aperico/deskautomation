# Automated Mechanical Desk Lift System

## Quick Start

### Automated Development Pipeline (Recommended)

The fastest way to build and test the project:

```bash
# Complete pipeline: build + dual motor validation + static analysis
python toolchain/run-pipeline.py all

# Build only
python toolchain/run-pipeline.py clean-build

# Run tests (validates both MT_BASIC and MT_ROBUST motor types)
python toolchain/run-pipeline.py test

# Quick unit test during development
python toolchain/run-pipeline.py unit

# Get help
python toolchain/run-pipeline.py help
```

**📖 Detailed Instructions:**
- **Pipeline Guide:** [toolchain/README.md](toolchain/README.md) - Complete pipeline manual
- **Build Guide:** [src/README.md](src/README.md) - Manual build and test instructions
- **Test Overview:** [tests/README.md](tests/README.md) - Test suite quick reference

---

## Project Overview

The Automated Mechanical Desk Lift System is a motorized solution for height-adjustable desks, enabling users to easily switch between sitting and standing positions. The system features responsive controls, safety mechanisms to prevent accidents, and reliable operation over the product lifetime. Key features include:
- **Configurable motor drivers:** Support for both L298N (cost-optimized) and IBT_2 (high-capacity) motor controllers
- Motorized height adjustment from 30 cm to 120 cm
- Support for up to 20 kg load (monitor, keyboard, etc.)
- UP/DOWN buttons for intuitive control
- Immediate halt on button release or obstruction detection
- Robust safety features including limit sensors and current sensing for jam detection
- Smooth, predictable motion for user comfort
- Designed for office environments to improve health and productivity
- Modular architecture for maintainability and future feature expansion
- Comprehensive documentation and testing to ensure quality and reliability
- Open-source hardware and software design for community collaboration and innovation

### Motor Driver Options

The system supports two motor driver configurations, selectable at compile-time:

| Driver | Model | Cost | Capacity | Use Case |
|--------|-------|------|----------|----------|
| **MT_BASIC** | L298N Dual H-Bridge | Low (~$2-5) | Standard (~50kg) | Cost-optimized office desks |
| **MT_ROBUST** | IBT_2 Intelligent Driver | Moderate (~$10-15) | High (~100kg+) | Heavy-duty commercial desks |

Both drivers share the same software (unified HAL interface) and provide equivalent safety and functionality. See [Motor Driver Configuration Guide](documentation/07_MotorDriverConfiguration.md) for build and switching instructions.

## Motor Configuration Architecture

The system implements a robust motor configuration encapsulation pattern to support multiple motor driver types:

### Design Approach

- **Compile-Time Configuration (Current)**: Motor type is defined at compile-time in `src/motor_config.cpp` with validation
- **Runtime-Ready (Future)**: Architecture is prepared for NVM-based runtime configuration without code changes
- **Encapsulation Interface**: `MotorConfig_getMotorType()` function provides the sole accessor for motor configuration
- **Application Transparency**: All application logic is motor-type-agnostic; HAL layer handles driver-specific details

### Configuration Files

| File | Role | Details |
|------|------|---------|
| `src/motor_config.h` | Public interface | Type definitions, feature documentation, function declaration |
| `src/motor_config.cpp` | Private implementation | MOTOR_TYPE macro, validation, getter function, NVM migration comments |

### Switching Motor Drivers

```bash
# Default: MT_BASIC (L298N)
cmake -S . -B build && cmake --build build

# To use MT_ROBUST (IBT_2): Edit src/motor_config.cpp line 44
# #define MOTOR_TYPE MT_BASIC        <- Change to MT_ROBUST
# Then rebuild:
cmake --build build
```

### Test Coverage

All 37 unit and integration tests validate both motor configurations:
- **MT_BASIC**: L298N driver (default)
- **MT_ROBUST**: IBT_2 driver

Tests can be run against either driver by changing the MOTOR_TYPE definition. See [05_SoftwareArchitecture.md](documentation/05_SoftwareArchitecture.md) (AD-009: Motor Configuration Encapsulation) and [06_00_DetailedDesign.md](documentation/06_00_DetailedDesign.md) (Motor Configuration Module) for detailed technical documentation.

---

## Documentation Index

### Development Guidelines

| Document | Description |
|----------|-------------|
| [toolchain/coding_guides_and_checks.md](toolchain/coding_guides_and_checks.md) | **⚠️ Coding standards for safety-critical code (NASA Power of Ten, MISRA C:2012, ISO 25119)** |
| [toolchain/README.md](toolchain/README.md) | Development pipeline and automated testing guide |
| [toolchain/ARCHITECTURE.md](toolchain/ARCHITECTURE.md) | Pipeline architecture and design details |

### Requirements and Design

| Document | Description |
|----------|-------------|
| [01_MissionStatement.md](documentation/01_MissionStatement.md) | Project mission and overall purpose |
| [01_SystemObjectives.md](documentation/01_SystemObjectives.md) | High-level system objectives and goals |
| [02_ConceptOfOperations.md](documentation/02_ConceptOfOperations.md) | Operational concept and use cases |
| [02_01_HARA-prep.md](documentation/02_01_HARA-prep.md) | HARA preparation (draft) |
| [02_02_HARA-complete.md](documentation/02_02_HARA-complete.md) | Hazard analysis and risk assessment (HARA) |
| [02_03_SafetyGoals.md](documentation/02_03_SafetyGoals.md) | Safety goals derived from HARA |
| [03_00_SystemRequirements.md](documentation/03_00_SystemRequirements.md) | System-level requirements specification |
| [03_01_SystemArchitecture.md](documentation/03_01_SystemArchitecture.md) | System architecture and interfaces |
| [03_01_01_TechnicalSafetyConcept.md](documentation/03_01_01_TechnicalSafetyConcept.md) | Technical safety concept |
| [03_02_RequirementsTraceabilityMatrix.md](documentation/03_02_RequirementsTraceabilityMatrix.md) | Requirements traceability matrix |
| [04_SoftwareRequirements.md](documentation/04_SoftwareRequirements.md) | Software requirements and functional specifications |
| [04_01_SoftwareVerificationSpecification.md](documentation/04_01_SoftwareVerificationSpecification.md) | Software test specifications and verification methods |
| [05_SoftwareArchitecture.md](documentation/05_SoftwareArchitecture.md) | Software architecture design and component structure |
| [06_00_DetailedDesign.md](documentation/06_00_DetailedDesign.md) | Detailed software design and implementation details |
| [06_01_DetailedDesign_InteractionDiagrams.md](documentation/06_01_DetailedDesign_InteractionDiagrams.md) | Sequence diagrams and data flow visualizations |
| [07_MotorDriverConfiguration.md](documentation/07_MotorDriverConfiguration.md) | Motor driver configuration guide (L298N vs IBT_2)
| [10_SystemTestSpecification.md](documentation/10_SystemTestSpecification.md) | System test plan and test specifications |
| [10_01_SystemTestReport.md](documentation/10_01_SystemTestReport.md) | System test report and results |
| [11_HardwareDocumentation.md](documentation/11_HardwareDocumentation.md) | Hardware components and wiring documentation |
| [02_ConceptOfOperations_Reviews.md](documentation/02_ConceptOfOperations_Reviews.md) | ConOps review protocol and records |
| [03_00_SystemRequirements_Reviews.md](documentation/03_00_SystemRequirements_Reviews.md) | System requirements review protocol and records |
| [03_01_01_TechnicalSafetyConcept_Reviews.md](documentation/03_01_01_TechnicalSafetyConcept_Reviews.md) | Technical safety concept review protocol and records |
| [03_01_SystemArchitecture_Reviews.md](documentation/03_01_SystemArchitecture_Reviews.md) | System architecture review protocol and records |
| [04_SoftwareRequirements_Reviews.md](documentation/04_SoftwareRequirements_Reviews.md) | Software requirements review protocol and records |
| [05_SoftwareArchitecture_Reviews.md](documentation/05_SoftwareArchitecture_Reviews.md) | Software architecture review protocol and records |
| [06_DetailedDesign_Reviews.md](documentation/06_DetailedDesign_Reviews.md) | Detailed design review protocol and records |
| [10_SystemTestSpecification_Reviews.md](documentation/10_SystemTestSpecification_Reviews.md) | System test specification review protocol and records |
| [feature-pdrs/](documentation/feature-pdrs/) | System feature concept reviews |

## Project Structure

### Root Directory

| File/Directory | Description |
|----------------|-------------|
| `CMakeLists.txt` | CMake build configuration file |
| `CMakePresets.json` | CMake preset configurations |
| `README.md` | Project overview and instructions (this file) |
| `.github/` | GitHub workflows and CI/CD configuration |

### Source Code (`src/`)

| File | Description |
|------|-------------|
| `desk_app.cpp/h` | Main application logic and state machine |
| `desk_types.h` | Type definitions and data structures |
| `hal.cpp/h` | Hardware Abstraction Layer (HAL) interface |
| `motor_config.h` | Motor driver type configuration (MT_BASIC or MT_ROBUST) |
| `motor_controller.cpp/h` | Motor control logic and algorithms |
| `pin_config.h` | Arduino pin assignments (configurable per motor type) |
| `src.ino` | Arduino firmware entry point |

### Tests (`tests/`)

| File | Description |
|------|-------------|
| `UnitTests.cpp` | Unit tests for individual functions and classes |
| `ComponentTests.cpp` | Component-level integration tests |
| `IntegrationTests.cpp` | Full system integration tests |
| `hal_mock/` | Mock implementations of HAL for testing on host |
| `└── HALMock.cpp/h` | Mock HAL implementation |
| `└── SerialMock.cpp/h` | Mock Serial communication for testing |


### Training Materials (`00_training_context/`)

| File | Description |
|------|-------------|
| `00_agenda.md` | Training overview and agenda |
| `01_hello-world.md` | Getting started guide |
| `02_aisetup.md` | AI/Copilot setup instructions |
| `03_prompting-techniques.md` | AI prompting best practices |
| `04_00_deskautomation-syseng.md` | Systems engineering overview |
| `04_01_00_system-definition.md` | System definition guide |
| `04_01_01_system-definition-architecture.md` | Architecture definition guide |
| `04_02_software-construction.md` | Software construction practices |
| `04_03_00_component-verification.md` | Component testing guide |
| `04_03_01_software_integration-verification.md` | Integration testing guide |
| `04_03_03_system-testing.md` | System testing guide |
| `05_traceability-matrix.md` | Requirements traceability |
| `06_workflow-pipelines.md` | CI/CD pipeline documentation |
| `07_process-assessment.md` | Process quality assessment |

### Schematics (`schematics/`)

| Directory | Description |
|-----------|-------------|
| `schmatics_desk_automation/` | KiCad project files for desk automation |
| `└── DeskAutomation_MotorControl/` | Motor control circuit schematics |
| `└── DeskAutomation.pretty/` | Custom KiCad footprints |
