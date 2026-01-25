# Session 6: Workflow Pipelines

**Focus:** Local and Online (Github action) build chains

**Time:** ~15 minutes

---

## Part 1: Local Pipeline

Create a pipeline that enables the team to build and test the system on their respective machines.

**Objective:** Automated local build, test, and lint pipeline for developer machines (Windows 10+, CMake 3.20+)

**Output Format:** PowerShell script (`build_and_test.ps1`) executable in terminal

Prompt:
```
You are a Build Systems Engineer for an embedded C++ project (Standing Desk Automation System).

CONTEXT:
- Project: Standing Desk Automation (Arduino-based motorized desk controller)
- Language: C++ (Arduino compatible)
- Build System: CMake 3.20+
- Test Framework: Google Test (GTest)
- Target: Windows 10+ development machines
- Codebase Structure:
  * 02_src/ → Source code (hal.h, hal.cpp, motor_controller.h/cpp, desk_app.h/cpp, desk_automation.ino, pin_config.h)
  * 03_tests/ → Test code (test_component_software_requirements.cpp)
  * build/ → Build output directory
  * CMakeLists.txt → CMake configuration

REQUIREMENTS:
Generate a PowerShell script (build_and_test.ps1) that automates the local development pipeline on Windows machines.

Pipeline Stages:
1. **Build Stage:**
   - Clean build directory (rm -r build if exists)
   - Create build directory
   - Run CMake configuration: cmake .. (from build directory)
   - Compile: cmake --build . (Release mode recommended)
   - Report: Print "BUILD SUCCESS" or detailed error message on failure
   - Stop execution on build failure (exit with code 1)

2. **Test Stage:**
   - Run all component tests: ctest --verbose (from build directory)
   - Parse test output: Count passed/failed tests
   - Report: Print summary (e.g., "10 tests passed, 0 failed")
   - Stop execution on test failure (exit with code 1)

3. **Lint Stage:**
   - Run C++ linter on source files (02_src/*.h, 02_src/*.cpp)
   - Tool recommendation: clang-tidy (if available) or cppcheck
   - Check for: Code style violations, warnings, security issues
   - Report: List all violations with file/line numbers
   - Warning: Print lint warnings but do NOT fail pipeline (exit code 0)

4. **Summary Report:**
   - Print final status: "✅ All checks passed" or "⚠️ Warnings detected" or "❌ Build/tests failed"
   - Elapsed time
   - Next steps (e.g., "Ready to commit" or "Fix errors and re-run")

SPECIFICATIONS:
- Language: PowerShell 5.1 (Windows native)
- Error Handling: Try-catch blocks, meaningful error messages
- Logging: Print each stage status to console with timestamps
- Colors: Green for success, Yellow for warnings, Red for errors
- Configuration: Allow optional flags (e.g., -SkipLint, -SkipTests, -Verbose)
- Artifacts: Build output in build/ directory, test results in build/Testing/

OUTPUT:
Provide ONLY the complete PowerShell script, no explanation. Script must be copy-paste ready and executable immediately.

Include shebang, comments for each stage, and inline documentation.
```

## Part 2: GitHub Actions Pipeline

Create a CI/CD pipeline that automatically builds and tests the system on every push/pull request.

**Objective:** Automated GitHub Actions workflow for cloud-based build, test, and lint validation

**Output Format:** YAML workflow file (`.github/workflows/build-test-lint.yml`)

Prompt:
```
You are a CI/CD Engineer designing a GitHub Actions workflow for an embedded C++ project.

CONTEXT:
- Repository: Standing Desk Automation System (GitHub)
- Language: C++ (Arduino-compatible)
- Build System: CMake 3.20+
- Test Framework: Google Test (GTest)
- Target Platforms: Ubuntu 20.04 LTS (Linux), Windows Server 2022
- Triggers: On push to main/develop, on pull request to main
- Codebase: CMakeLists.txt, 02_src/**, 03_tests/**

REQUIREMENTS:
Generate a GitHub Actions workflow YAML file (.github/workflows/build-test-lint.yml) that:

1. **Trigger Configuration:**
   - Run on push to branches: main, develop
   - Run on pull requests targeting: main
   - Run on manual trigger (workflow_dispatch)

2. **Build Job (Ubuntu Linux):**
   - OS: ubuntu-latest
   - Steps:
     * Checkout code (github.com/actions/checkout@v3)
     * Install dependencies: cmake, g++, build-essential
     * Fetch Google Test (already in CMakeLists.txt)
     * Create build directory
     * CMake configure: cmake .. (Release mode)
     * Build: cmake --build . --parallel 4
     * Artifact: Upload build/ to GitHub Artifacts (optional, for debugging)
   - Fail if build fails (automatic on non-zero exit code)

3. **Test Job (Ubuntu Linux, depends on Build Job):**
   - OS: ubuntu-latest
   - Runs after Build Job succeeds
   - Steps:
     * Checkout code
     * Install dependencies (same as Build Job)
     * CMake configure & build (same as Build Job)
     * Run tests: ctest --verbose --output-on-failure
     * Upload test results (JUnit format for GitHub UI)
   - Fail if any test fails

4. **Lint Job (Ubuntu Linux, parallel with Build):**
   - OS: ubuntu-latest
   - Steps:
     * Checkout code
     * Install clang-tidy or cppcheck
     * Run linter on 02_src/ files
     * Generate lint report (sarif format for GitHub UI)
     * Upload lint results to GitHub Security tab
   - Do NOT fail pipeline on lint warnings (continue-on-error: true)

5. **Build Job (Windows Server):**
   - OS: windows-latest
   - Same build steps as Ubuntu
   - CMake command same, compiler: MSVC (native to Windows runner)
   - Artifact: Upload build/ (optional)

6. **Test Job (Windows Server):**
   - OS: windows-latest
   - Run after Windows Build succeeds
   - Same test steps as Ubuntu version

7. **Summary Report:**
   - Create summary job (runs-on: ubuntu-latest) that depends on all jobs
   - Print matrix results: "✅ Ubuntu Build/Test/Lint passed" or "❌ Failed"
   - Print matrix results: "✅ Windows Build/Test passed" or "❌ Failed"
   - Set workflow conclusion (success if all jobs pass)

SPECIFICATIONS:
- Format: GitHub Actions YAML syntax (v2+)
- Jobs: build-linux, test-linux, lint, build-windows, test-windows, summary
- Matrix: OS (ubuntu-latest, windows-latest) for build/test
- Timeout: 15 minutes per job
- Artifacts: test-results (optional), build logs (optional)
- Notifications: GitHub Actions UI (built-in), no email/Slack required for this MVP
- Secret handling: None required (public repo)

DELIVERABLE:
Provide ONLY the complete .github/workflows/build-test-lint.yml file. File must be:
- Valid YAML syntax
- Copy-paste ready into repository
- Executable immediately after push
- Includes inline comments explaining each section

Include error handling, dependency ordering, and success/failure conditions.
```
---

**Next Step:** Proceed to [process maturiy assessment](07_process_assessment.md).