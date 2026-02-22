# Source Code Build & Test Guide

This directory contains the source code for the Automated Mechanical Desk Lift System. For high-level project documentation and overview, see the [main README](../README.md).

## 🚀 Recommended: Automated Pipeline

**Use the automated development pipeline for the fastest and most reliable builds:**

```bash
# Complete pipeline: build + dual motor validation + static analysis
python toolchain/run-pipeline.py all

# Build only (clean + configure + build)
python toolchain/run-pipeline.py clean-build

# Run all tests (validates both MT_BASIC and MT_ROBUST motor types)
python toolchain/run-pipeline.py test

# Quick development workflow
python toolchain/run-pipeline.py unit        # Unit tests only
python toolchain/run-pipeline.py component   # Component tests only

# Get help
python toolchain/run-pipeline.py help
```

**Why use the pipeline?**
- ✅ Automated dual motor type validation (MT_BASIC + MT_ROBUST)
- ✅ Consistent build environment
- ✅ Comprehensive test coverage (37 tests)
- ✅ Static analysis integration
- ✅ Detailed results saved to `toolchain/results/`

**📖 Complete Pipeline Documentation:** [toolchain/README.md](../toolchain/README.md)

---

## Manual Build Instructions

For manual control or understanding the build process, use these commands:

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

**With Motor Driver Selection:**
```bash
# Build for L298N (default)
cmake -S . -B build -DMOTOR_TYPE=MT_BASIC

# Build for IBT_2 (high-capacity)
cmake -S . -B build -DMOTOR_TYPE=MT_ROBUST
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
- `build/UnitTests.exe` - Unit test executable (6 tests)
- `build/ComponentTests.exe` - Component test executable (17 tests)
- `build/IntegrationTests.exe` - Integration test executable (14 tests)
- Build artifacts in `build/` directory

## Motor Driver Configuration

The system supports two motor driver configurations selected at compile-time:

| Driver | Model | Capacity | Use Case |
|--------|-------|----------|----------|
| **MT_BASIC** (default) | L298N Dual H-Bridge | Standard (~50kg) | Cost-optimized office desks |
| **MT_ROBUST** | IBT_2 Intelligent Driver | High (~100kg+) | Heavy-duty commercial desks |

Both drivers share the same software architecture and provide identical safety functionality through a unified Hardware Abstraction Layer (HAL).

### Selecting a Motor Driver

**Option 1: CMake Configuration (Recommended)**

When configuring the build, specify the motor type:

```bash
# Build for L298N (default) - cost-optimized
cmake -S . -B build -DMOTOR_TYPE=MT_BASIC
cmake --build build --config Release

# Build for IBT_2 - high-capacity
cmake -S . -B build -DMOTOR_TYPE=MT_ROBUST
cmake --build build --config Release
```

**Option 2: Edit motor_config.h**

Edit [motor_config.h](motor_config.h) line 47:

```cpp
// For L298N:
#define MOTOR_TYPE MT_BASIC      // Default

// For IBT_2:
#define MOTOR_TYPE MT_ROBUST
```

Then clean and rebuild:
```bash
Remove-Item -Path build -Recurse -Force
cmake -S . -B build
cmake --build build --config Release
```

### Verifying Motor Driver Type

After building, the active motor driver type is embedded in the binary. Pin assignments are determined by the selection:

**L298N (MT_BASIC):**
- PIN_MOTOR_EN1 = 6  (Enable UP direction)
- PIN_MOTOR_EN2 = 7  (Enable DOWN direction)
- PIN_MOTOR_PWM = 9  (Speed control)

**IBT_2 (MT_ROBUST):**
- PIN_MOTOR_LPWM = 9   (Left PWM - UP direction)
- PIN_MOTOR_RPWM = 10  (Right PWM - DOWN direction)
- PIN_MOTOR_CIN = A1   (Diagnostic input, optional)

See [Motor Driver Configuration Guide](../documentation/07_MotorDriverConfiguration.md) for detailed hardware setup instructions.

## Configuration

Safety-related thresholds and ADC conversion constants are defined in [safety_config.h](safety_config.h).

```cpp
static const uint16_t MOTOR_SENSE_THRESHOLD_MA = 150U;
static const uint16_t MOTOR_SENSE_OBSTRUCTION_THRESHOLD_MA = 200U;
static const uint32_t MOTOR_SENSE_FAULT_TIME_MS = 100U;
static const uint16_t ADC_REF_MV = 5000U;
static const uint16_t SHUNT_MILLIOHMS = 500U;
```

**Tuning guidance:**
- Measure motor current at idle STOP and normal motion with a meter.
- Set `MOTOR_SENSE_THRESHOLD_MA` above STOP current noise but below expected motion current.
- Set `MOTOR_SENSE_OBSTRUCTION_THRESHOLD_MA` to detect jam conditions during motion (typically 200 mA).
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
build/UnitTests.exe        - Unit tests (6 tests)
build/ComponentTests.exe   - Component tests (17 tests)
build/IntegrationTests.exe - Integration tests (14 tests)
Total: 37 tests
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

## CI/CD Pipeline

### Local Development Pipeline

The `tests/toolchain/run-pipeline.py` script provides a convenient way for developers to execute common development tasks locally, including building, testing, and static analysis.

#### Prerequisites

- Python 3.7+
- MSYS2 with MinGW-w64 toolchain
- CMake
- cppcheck (for static analysis): `pacman -S mingw-w64-x86_64-cppcheck`

#### Usage

```bash
python tests/toolchain/run-pipeline.py <command>
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
```bash
python tests/toolchain/run-pipeline.py clean-build
```

**Rebuild Everything:**
```bash
python tests/toolchain/run-pipeline.py rebuild
```

**Run All Tests:**
```bash
python tests/toolchain/run-pipeline.py test
```

**Clean and Run All Tests:**
```bash
python tests/toolchain/run-pipeline.py clean-test
```

**Run Static Analysis:**
```bash
python tests/toolchain/run-pipeline.py static-analysis
```

**Run Complete Pipeline:**
```bash
python tests/toolchain/run-pipeline.py all
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

## Source Files

| File | Description |
|------|-------------|
| `desk_app.cpp/h` | Main application logic and state machine |
| `desk_types.h` | Type definitions and data structures |
| `hal.cpp/h` | Hardware Abstraction Layer (HAL) interface |
| `motor_controller.cpp/h` | Motor control logic and algorithms |
| `pin_config.h` | Arduino pin assignments and hardware configuration |
| `safety_config.h` | Safety thresholds and configuration constants |
| `src.ino` | Arduino firmware entry point |
