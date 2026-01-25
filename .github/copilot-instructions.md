# Copilot AI Agent Instructions for deskautomation

## Project Overview
- **Purpose:** Automated Mechanical Desk Lift System with safety, compliance (ISO 25119, ASPICE), and robust test automation.
- **Architecture:**
  - Main logic in `source/arduino/DeskController.cpp`/`.h` (application logic)
  - Hardware abstraction in `source/arduino/HAL.cpp`/`.h`
  - Pin configuration in `source/arduino/PinConfig.h`
  - Firmware entry: `source/arduino/arduino.ino`
  - Mocks for host/CI testing: `source/arduino/hal_mock/`
  - Tests: `tests/SoftwareTests.cpp` (unit), `tests/IntegrationTests.cpp` and `tests/Integration_SmokeTests.cpp` (integration)
  - Documentation: `documentation/` (see especially `SoftwareArchitecture.md`, `SoftwareDetailedDesign.md`, `TraceabilityMatrix.md`)

## Build & Test Workflows
- **Build (Windows, MSYS2):**
  - Configure: `cmake -S . -B build`
  - Build: `cmake --build build --config Release`
  - Or use VS Code tasks: `CMake: Clean`, `CMake: Configure`, `CMake: Build`
- **Run All Tests:**
  - `ctest --test-dir build -C Release --output-on-failure`
  - Or run test binary directly: `build\SoftwareTests.exe`
- **Run Single Test:**
  - `build\SoftwareTests.exe --gtest_filter=SmokeTest.BasicTruth`
  - Integration: `build\SoftwareTests.exe --gtest_filter=IntegrationTest.*`
- **CI:**
  - See `.github/workflows/cmake-single-platform.yml` for CI steps, coverage, and artifact upload.

## Coding & Design Conventions
- **Naming:**
  - Classes/structs: `CamelCase`
  - Functions/vars: `snake_case`
  - Constants/macros: `UPPER_SNAKE_CASE`
- **Style:** Allman braces, 2 or 4 spaces, no tabs
- **Safety:** Mark safety-critical code with `// SAFETY-CRITICAL`
- **Testing:**
  - All new code must have unit tests
  - Integration tests use the same binary, filtered by label
- **Documentation:**
  - Update docs in `documentation/` for new features or changes
  - Maintain traceability (see `TraceabilityMatrix.md`)

## Integration & External Dependencies
- **Google Test** is managed via CMake (no manual install needed)
- **Static analysis:** Cppcheck, clang-tidy (see CI and `toolchain.md`)
- **Coverage:** gcov/lcov (see CI and `toolchain.md`)

## Project-Specific Patterns
- **HAL abstraction**: All hardware access goes through `HAL.*` functions
- **Mocks**: Use `hal_mock/` for host/CI testing
- **Test labels**: Use `Unit` and `Integration` labels for test filtering
- **Safety**: Startup and error recovery logic in `DeskApp_task_init()` and `HAL_wait_startup()`

## Key References
- [README.md](../README.md)
- [documentation/codingguidelines.md](../documentation/codingguidelines.md)
- [documentation/TESTING_README.md](../documentation/TESTING_README.md)
- [documentation/SoftwareArchitecture.md](../documentation/SoftwareArchitecture.md)
- [documentation/TraceabilityMatrix.md](../documentation/TraceabilityMatrix.md)

---
**When in doubt, prefer existing patterns and update documentation and tests for all changes.**
