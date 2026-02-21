# Automated Mechanical Desk Lift System

## Documentation Index

| Document | Description |
|----------|-------------|
| [01_MissionStatement.md](documentation/01_MissionStatement.md) | Project mission and overall purpose |
| [01_SystemObjectives.md](documentation/01_SystemObjectives.md) | High-level system objectives and goals |
| [02_ConceptOfOperations.md](documentation/02_ConceptOfOperations.md) | Operational concept and use cases |
| [02_01_HARA.md](documentation/02_01_HARA.md) | Hazard analysis and risk assessment (HARA) |
| [03_00_SystemRequirements.md](documentation/03_00_SystemRequirements.md) | System-level requirements specification |
| [03_01_SystemArchitecture.md](documentation/03_01_SystemArchitecture.md) | System architecture and interfaces |
| [03_02_RequirementsTraceabilityMatrix.md](documentation/03_02_RequirementsTraceabilityMatrix.md) | Requirements traceability matrix |
| [04_SoftwareRequirements.md](documentation/04_SoftwareRequirements.md) | Software requirements and functional specifications |
| [05_SoftwareArchitecture.md](documentation/05_SoftwareArchitecture.md) | Software architecture design and component structure |
| [06_DetailedDesign.md](documentation/06_DetailedDesign.md) | Detailed software design and implementation details |
| [10_SystemTestSpecification.md](documentation/10_SystemTestSpecification.md) | System test plan and test specifications |
| [10_01_SystemTestReport.md](documentation/10_01_SystemTestReport.md) | System test report and results |
| [11_HardwareDocumentation.md](documentation/11_HardwareDocumentation.md) | Hardware components and wiring documentation |
| [feature-pdrs/](documentation/feature-pdrs/) | System feature concept reviews |

## Building

### Prerequisites
- Windows with MSYS2 installed
- CMake
- MinGW-w64 toolchain (via MSYS2)
- C++17 compiler

### Build Commands (Command Line)

#### Clean Build Directory
```bash
Remove-Item -Path "build" -Recurse -Force -ErrorAction SilentlyContinue
```

Or using CMake:
```bash
cmake --build build --target clean
```

#### Configure
```bash
cmake -S . -B build
```

#### Build
```bash
cmake --build build --config Release
```

#### Full Build Sequence (Clean → Configure → Build)
```bash
Remove-Item -Path "build" -Recurse -Force -ErrorAction SilentlyContinue
cmake -S . -B build
cmake --build build --config Release
```

### Build Using VS Code Tasks

The project includes predefined VS Code tasks for building:

1. **CMake: Clean** - Removes the build directory
2. **CMake: Configure** - Configures the CMake project (depends on Clean)
3. **CMake: Build** - Builds the project (depends on Configure)

To run these tasks:
- Press `Ctrl+Shift+P` and type "Run Task"
- Select the desired task from the list
- Or use the Terminal menu → Run Task

The tasks automatically chain dependencies, so running "CMake: Build" will clean and configure first if needed.

### Build Artifacts

After a successful build, you'll find:
- `build/UnitTests.exe` - Unit test executable (5 tests)
- `build/ComponentTests.exe` - Component test executable (12 tests)
- `build/IntegrationTests.exe` - Integration test executable (6 tests)
- Build artifacts in `build/` directory

## Configuration

Safety-related thresholds and ADC conversion constants are defined in [src/safety_config.h](src/safety_config.h).

```cpp
static const uint16_t MOTOR_SENSE_THRESHOLD_MA = 150U;
static const uint32_t MOTOR_SENSE_FAULT_TIME_MS = 100U;
static const uint16_t ADC_REF_MV = 5000U;
static const uint16_t SHUNT_MILLIOHMS = 500U;
```

**Tuning guidance:**
- Measure motor current at idle STOP and normal motion with a meter.
- Set `MOTOR_SENSE_THRESHOLD_MA` above STOP current noise but below expected motion current.
- Verify `MOTOR_SENSE_FAULT_TIME_MS` avoids false positives during brief transients.
- If you change the shunt resistor, update `SHUNT_MILLIOHMS` to keep current conversion accurate.

## Testing

### Building Tests

Tests are automatically built as part of the main build process. The CMake configuration includes all test files and links them with Google Test.

To build tests:
```bash
# Full build (includes tests)
cmake -S . -B build
cmake --build build --config Release
```

Or use the VS Code task: **CMake: Build**

### Test Executables

After building, the test executables are located at:
```
build/UnitTests.exe        - Unit tests (5 tests)
build/ComponentTests.exe   - Component tests (12 tests)
build/IntegrationTests.exe - Integration tests (6 tests)
```

### Running Tests

#### Run All Tests

Using CTest (runs all test executables):
```bash
ctest --test-dir build -C Release --output-on-failure
```

Using test executables directly:
```bash
# Run all unit tests
./build/UnitTests.exe

# Run all component tests
./build/ComponentTests.exe

# Run all integration tests
./build/IntegrationTests.exe
```

Or use the VS Code task: **CTest: Run All**

#### Run Specific Tests

Run specific unit tests:
```bash
./build/UnitTests.exe --gtest_filter=MotorController.*
```

Run specific component tests:
```bash
./build/ComponentTests.exe --gtest_filter=TC_SWReq001
```

Run specific integration tests:
```bash
./build/IntegrationTests.exe --gtest_filter=HAL.*
```

Using CTest with regex filter:
```bash
ctest --test-dir build -C Release -R "^ComponentTests$" --output-on-failure
```

#### List Available Tests

```bash
# List unit tests
./build/UnitTests.exe --gtest_list_tests

# List component tests
./build/ComponentTests.exe --gtest_list_tests

# List integration tests
./build/IntegrationTests.exe --gtest_list_tests
```

### Test Types

The project includes three types of tests:

1. **Unit Tests** - Test individual functions and classes in isolation
2. **Component Tests** - Test integrated components (e.g., DeskController with mocked HAL)
3. **Integration Tests** - Test full system integration with hardware abstraction layer

All tests use Google Test framework and are located in the `tests/` directory.

### Test Filtering Options

Google Test supports various filtering patterns:
- `*` - Matches any string
- `?` - Matches any single character
- Use `:` to separate multiple patterns (OR logic)
- Prefix with `-` to exclude tests

Examples:
```bash
# Run all tests in a component test suite except one
./build/ComponentTests.exe --gtest_filter=-TC_SWReq001

# Run multiple specific unit tests
./build/UnitTests.exe --gtest_filter=MotorController.Initialize:MotorController.SetSpeed
```

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
| `motor_controller.cpp/h` | Motor control logic and algorithms |
| `pin_config.h` | Arduino pin assignments and hardware configuration |
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

## CI/CD Pipeline

### Local Development Pipeline

The `tests/run-pipeline.ps1` script provides a convenient way for developers to execute common development tasks locally, including building, testing, and static analysis.

#### Prerequisites

- Windows with MSYS2 installed
- MinGW-w64 toolchain (via MSYS2)
- CMake
- cppcheck (for static analysis): `pacman -S mingw-w64-x86_64-cppcheck`

#### Usage

```powershell
.\tests\run-pipeline.ps1 -Command <command>
```

#### Available Pipeline Commands

| Command | Description |
|---------|-------------|
| `clean-build` | Clean build directory and build the project |
| `rebuild` | Full rebuild (clean + configure + build) |
| `test` | Run all test cases |
| `clean-test` | Clean, build, and run all test cases |
| `static-analysis` | Run static analysis using cppcheck |
| `all` | Run everything (rebuild + test + static analysis) |
| `help` | Show help message |

#### Pipeline Examples

**Clean and Build:**
```powershell
.\tests\run-pipeline.ps1 -Command clean-build
```

**Rebuild Everything:**
```powershell
.\tests\run-pipeline.ps1 -Command rebuild
```

**Run All Tests:**
```powershell
.\tests\run-pipeline.ps1 -Command test
```

**Clean and Run All Tests:**
```powershell
.\tests\run-pipeline.ps1 -Command clean-test
```

**Run Static Analysis:**
```powershell
.\tests\run-pipeline.ps1 -Command static-analysis
```

**Run Complete Pipeline:**
```powershell
.\tests\run-pipeline.ps1 -Command all
```

This will:
1. Clean the build directory
2. Configure CMake
3. Build the project
4. Run all tests
5. Run static analysis

#### Pipeline Output

The script provides color-coded output:
- 🟢 Green: Success messages
- 🔵 Cyan: Info messages
- 🔴 Red: Error messages
- 🟡 Yellow: Section headers

#### Pipeline Results Storage

All pipeline results are stored in the `tests/results/` directory:

| Result File | Description |
|-------------|-------------|
| `cmake_configure.txt` | CMake configuration output with warnings |
| `build_log.txt` | Complete build log with compiler warnings |
| `test_results.txt` | Test execution results with pass/fail summary |
| `static_analysis.txt` | Human-readable static analysis report |
| `static_analysis.xml` | Machine-readable XML for CI integration |

**Note:** Result files are overwritten on each pipeline run. Each file includes a header with:
- Full command executed
- Timestamp of execution
- Brief description of the results
- Relevant configuration details

These logs are useful for:
- Reviewing the latest build/test/analysis results
- Debugging compilation issues
- Quick access to compiler warnings and static analysis findings
- CI integration via XML output

#### Pipeline Exit Codes

- `0`: Success
- `1`: Failure (build, test, or analysis failed)

#### Local vs CI/CD

This script mirrors the CI/CD pipeline configured in `.github/workflows/`, allowing developers to validate their changes locally before pushing to the repository.
