# Copilot AI Agent Instructions for deskautomation

## Project Overview
- **Purpose:** Automated Mechanical Desk Lift System with safety, compliance (ISO 25119, ASPICE), and robust test automation.
- **Architecture:**
  - Main application logic: `src/desk_app.cpp`/`.h`
  - Motor control: `src/motor_controller.cpp`/`.h`
  - Hardware abstraction: `src/hal.cpp`/`.h`
  - Pin configuration: `src/pin_config.h`
  - Type definitions: `src/desk_types.h`
  - Firmware entry: `src/src.ino`
  - Mocks for host/CI testing: `tests/hal_mock/HALMock.*`, `tests/hal_mock/SerialMock.*`
  - Tests: `tests/UnitTests.cpp` (5 unit tests), `tests/ComponentTests.cpp` (12 component tests), `tests/IntegrationTests.cpp` (6 integration tests)
  - Documentation: `documentation/` (see especially `05_SoftwareArchitecture.md`, `06_DetailedDesign.md`)
  - Training materials: `00_training_context/` (system engineering, testing, process assessment guides)

## Build & Test Workflows
- **Build (Windows, MSYS2):**
  - Configure: `cmake -S . -B build`
  - Build: `cmake --build build --config Release`
  - Or use VS Code tasks: `CMake: Clean`, `CMake: Configure`, `CMake: Build`
- **Local Development Pipeline:**
  - Use `tests/run-pipeline.ps1` for local development workflow
  - Commands: `clean-build`, `rebuild`, `test`, `clean-test`, `static-analysis`, `all`, `help`
  - Example: `.\tests\run-pipeline.ps1 -Command all`
  - Results stored in `tests/results/` directory
- **Run All Tests:**
  - `ctest --test-dir build -C Release --output-on-failure`
  - Or: `.\tests\run-pipeline.ps1 -Command test`
  - Test executables: `build\UnitTests.exe`, `build\ComponentTests.exe`, `build\IntegrationTests.exe`
- **Run Specific Tests:**
  - Unit tests: `build\UnitTests.exe --gtest_filter=MotorController.*`
  - Component tests: `build\ComponentTests.exe --gtest_filter=TC_SWReq001`
  - Integration tests: `build\IntegrationTests.exe --gtest_filter=HAL.*`
- **Static Analysis:**
  - Run: `.\tests\run-pipeline.ps1 -Command static-analysis`
  - Tool: cppcheck with comprehensive checks (errors, warnings, style, performance, portability)
- **CI/CD:**
  - GitHub Actions: `.github/workflows/cmake-single-platform.yml`
  - Local pipeline mirrors CI/CD for validation before push

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
- **Static analysis:** cppcheck (via MSYS2: `pacman -S mingw-w64-x86_64-cppcheck`)
- **Coverage:** gcov/lcov (see CI workflow)
- **Build system:** CMake with Ninja generator
- **Toolchain:** MinGW-w64 GCC 15.2.0 (C++17)

## Project-Specific Patterns
- **HAL abstraction**: All hardware access goes through `HAL.*` functions
- **Mocks**: Use `hal_mock/` for host/CI testing
- **Test labels**: Use `Unit` and `Integration` labels for test filtering
- **Safety**: Startup and error recovery logic in `DeskApp_task_init()` and `HAL_wait_startup()`

## Key References
- [README.md](../README.md)
- [documentation/codingguidelines.md](../documentation/codingguidelines.md)
- [documentation/TESTING_README.md](../documentation/TESTING_README.md)
- [documentation/05_SoftwareArchitecture.md](../documentation/05_SoftwareArchitecture.md)
- [documentation/06_DetailedDesign.md](../documentation/06_DetailedDesign.md)
- [documentation/TraceabilityMatrix.md](../documentation/TraceabilityMatrix.md)
- [tests/run-pipeline.ps1](../tests/run-pipeline.ps1)

---
**When in doubt, prefer existing patterns and update documentation and tests for all changes.**
