# Testing README

Purpose
- Quick instructions to build and run unit and integration tests locally and in CI.

Prerequisites
- CMake, a C++ toolchain (MSYS/mingw or Visual Studio toolchain on Windows), and Python (optional for helpers).
- For host tests: `gtest` is fetched via CMake (project configuration handles dependencies).

Build (out-of-source)

```powershell
# Configure
cmake -S . -B build -D CMAKE_BUILD_TYPE=Release
# Build
cmake --build build --config Release -j 2
```

Run unit tests (host)

```powershell
# Run all tests
build\SoftwareTests.exe
# Run a single GoogleTest by filter (example: smoke test)
build\SoftwareTests.exe --gtest_filter=SmokeTest.BasicTruth
# Run a single named test suite
build\SoftwareTests.exe --gtest_filter=DeskAppTest.*
```

Run integration tests (host)

```powershell
# Integration tests are implemented in the same test binary (IT-001..IT-010)
build\SoftwareTests.exe --gtest_filter=IntegrationTest.*
```

CI and test artifacts
- CI pipeline: `.github/workflows/cmake-single-platform.yml` builds and runs tests.
- Look for test artifacts (JUnit XML, coverage) attached in CI run artifacts or in the `build/` output if enabled.
- To produce JUnit XML locally, set the Google Test flag to generate output (if supported):

```powershell
build\SoftwareTests.exe --gtest_output=xml:build/test-results/results.xml
```

Coverage (optional)
- If coverage is enabled in CI, the pipeline may run `lcov`/`gcov` to produce HTML or `coverage.info` files. Consult the CI job steps.

HAL mocks and host tests
- Host tests use `source/arduino/hal_mock/` which provides `millis()`, `digitalWrite()`, `digitalRead()`, and `SerialMock`.
- Tests run deterministically using `std::chrono::steady_clock` for timing.

Logging during tests
- HAL exposes `HAL_set_logger(HAL_Logger_t)` to register a simple C-style logger callback for capturing HAL diagnostic messages in host tests.
- In Arduino builds, debug logging uses `Serial` when `DEBUG` is defined.

Running on target (Arduino)
- Flash `arduino.ino` via the Arduino IDE or `arduino-cli` using your board configuration.
- Note: `HAL_wait_startup()` performs a short startup wait; on-target you will see behavior identical to host tests, but with real hardware I/O.

Notes for assessors / reviewers
- Baseline commit used during recent checks: `d784c71` (HEAD as of 2026-01-07). Consider tagging a release (e.g., `v1.0.0`) and archiving CI artifacts to produce formal Level 2 evidence.
- Test-to-requirement traceability is maintained in `documentation/TraceabilityMatrix.md`.

Contact / contributions
- Open issues or PRs in the repository for test improvements or to add platform-specific CI jobs.
