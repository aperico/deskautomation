# Development Pipeline Toolchain

**Quick Start Guide for Building, Testing, and Analyzing the deskautomation Project**

---

## Table of Contents
- [Prerequisites](#prerequisites)
- [Quick Start](#quick-start)
- [Common Workflows](#common-workflows)
- [Available Commands](#available-commands)
- [Detailed Usage](#detailed-usage)
- [Output and Results](#output-and-results)
- [Advanced Usage](#advanced-usage)
- [Troubleshooting](#troubleshooting)

---

## Prerequisites

Before running the pipeline, ensure you have:

- **Python 3.7+** - Pipeline scripts are written in Python
- **MSYS2 with MinGW-w64** - C++ toolchain for building
- **CMake** - Build system generator
- **cppcheck** (optional) - For static analysis
  ```bash
  # Install via MSYS2
  pacman -S mingw-w64-x86_64-cppcheck
  ```

---

## Quick Start

### Run Complete Pipeline
Build, test (both motor types), and analyze in one command:
```bash
python toolchain/run-pipeline.py all
```

### Build Project
```bash
python toolchain/run-pipeline.py clean-build
```

### Run Tests
```bash
python toolchain/run-pipeline.py test
```

### Get Help
```bash
python toolchain/run-pipeline.py help
```

---

## Common Workflows

### 1. Fresh Build After Changes
```bash
# Clean, configure, and build
python toolchain/run-pipeline.py rebuild
```

### 2. Test Before Commit
```bash
# Build and test both motor configurations
python toolchain/run-pipeline.py clean-test
```

### 3. Quick Unit Test During Development
```bash
# Run only unit tests on current configuration
python toolchain/run-pipeline.py unit
```

### 4. Pre-Push Validation
```bash
# Full pipeline: build + dual motor test + static analysis
python toolchain/run-pipeline.py all
```

### 5. Static Analysis Only
```bash
# Run cppcheck without rebuilding
python toolchain/run-pipeline.py static-analysis
```

---

## Available Commands

| Command | Description | When to Use |
|---------|-------------|-------------|
| `clean-build` | Clean, configure, and build | After changing build configuration |
| `rebuild` | Same as clean-build | Alias for clean build workflow |
| `test` | Run all tests (MT_BASIC + MT_ROBUST) | Validate motor-agnostic behavior |
| `unit` | Run unit tests only | Quick feedback during development |
| `component` | Run component tests only | Test module interactions |
| `integration` | Run integration tests only | Test system-level behavior |
| `clean-test` | Full rebuild + dual motor tests | Clean slate testing |
| `static-analysis` | Run cppcheck analysis | Find potential code issues |
| `all` | Build + test + analyze | Complete validation pipeline |
| `help` | Show help message | Get command reference |

---

## Detailed Usage

### Main Pipeline Script

**Location:** `toolchain/run-pipeline.py`

**Syntax:**
```bash
python toolchain/run-pipeline.py <command>
```

**Examples:**
```bash
# Clean build
python toolchain/run-pipeline.py clean-build

# Run all tests with dual motor validation
python toolchain/run-pipeline.py test

# Run unit tests only
python toolchain/run-pipeline.py unit

# Complete pipeline
python toolchain/run-pipeline.py all
```

### Build Operations

**Script:** `toolchain/pipeline_build.py`

**Commands:**
- `clean` - Remove build directory
- `configure` - Run CMake configuration
- `build` - Build the project
- `clean-build` - Clean + configure + build
- `rebuild` - Alias for clean-build

**Standalone Usage:**
```bash
# Full rebuild
python toolchain/pipeline_build.py rebuild

# Just configure
python toolchain/pipeline_build.py configure

# Just build (assumes already configured)
python toolchain/pipeline_build.py build
```

**What It Does:**
1. Removes existing build artifacts (if clean)
2. Runs CMake to generate build files
3. Compiles all source code
4. Links executables
5. Saves build logs to `toolchain/results/build_log.txt`

### Test Operations

**Script:** `toolchain/pipeline_test.py`

**Commands:**
- `test` - Dual motor type validation (MT_BASIC + MT_ROBUST)
- `unit` - Run unit tests only
- `component` - Run component tests only
- `integration` - Run integration tests only

**Standalone Usage:**
```bash
# Dual motor type testing
python toolchain/pipeline_test.py test

# Unit tests only
python toolchain/pipeline_test.py unit

# Component tests only
python toolchain/pipeline_test.py component

# Integration tests only
python toolchain/pipeline_test.py integration
```

**Dual Motor Type Testing:**
The `test` command automatically:
1. Sets motor type to **MT_BASIC** (L298N driver)
2. Rebuilds the project
3. Runs all 37 tests
4. Saves results to `test_results_MT_BASIC.txt`
5. Sets motor type to **MT_ROBUST** (IBT_2 driver)
6. Rebuilds the project
7. Runs all 37 tests
8. Saves results to `test_results_MT_ROBUST.txt`
9. Restores default configuration (MT_BASIC)

This ensures the application logic is truly motor-type-agnostic.

### Static Analysis

**Script:** `toolchain/pipeline_analysis.py`

**Commands:**
- `static-analysis` - Run comprehensive cppcheck analysis

**Standalone Usage:**
```bash
python toolchain/pipeline_analysis.py static-analysis
```

**What It Does:**
1. Runs cppcheck with all checks enabled:
   - Errors
   - Warnings
   - Style issues
   - Performance problems
   - Portability concerns
2. Generates text report with issue details
3. Generates XML report for CI integration
4. Categorizes issues by severity
5. Saves results to `toolchain/results/`

---

## Output and Results

All pipeline operations save detailed logs to `toolchain/results/`:

### Build Results
- **`cmake_configure.txt`** - CMake configuration output
  - Compiler detection
  - Dependency resolution
  - Build system generation
  - Configuration warnings

- **`build_log.txt`** - Complete build output
  - Compilation messages
  - Compiler warnings and errors
  - Linking information
  - Build diagnostics

### Test Results
- **`test_results_MT_BASIC.txt`** - Test results for L298N driver
  - All test executions
  - Pass/fail status
  - Failed test details
  - Test summary

- **`test_results_MT_ROBUST.txt`** - Test results for IBT_2 driver
  - Same structure as MT_BASIC
  - Validates motor-agnostic behavior

- **`test_results_UnitTests.txt`** - Unit tests only (when run individually)
- **`test_results_ComponentTests.txt`** - Component tests only
- **`test_results_IntegrationTests.txt`** - Integration tests only

### Analysis Results
- **`static_analysis.txt`** - Human-readable cppcheck report
  - Issue categorization by severity
  - Detailed issue descriptions
  - File locations and line numbers
  - Summary statistics

- **`static_analysis.xml`** - Machine-readable XML
  - CI/CD integration
  - Automated parsing
  - Tool chain integration

### Viewing Results
```bash
# View latest build log
cat toolchain/results/build_log.txt

# View test results
cat toolchain/results/test_results_MT_BASIC.txt
cat toolchain/results/test_results_MT_ROBUST.txt

# View static analysis
cat toolchain/results/static_analysis.txt

# Open in editor
code toolchain/results/
```

---

## Advanced Usage

### Running Subscripts Independently

Each pipeline component can be run standalone for focused operations:

```bash
# Build operations only
python toolchain/pipeline_build.py clean-build

# Test operations only
python toolchain/pipeline_test.py unit

# Static analysis only
python toolchain/pipeline_analysis.py static-analysis
```

### Custom Workflows

Combine individual operations for custom workflows:

```bash
# Configure and build without cleaning
python toolchain/pipeline_build.py configure
python toolchain/pipeline_build.py build

# Run specific test suites
python toolchain/pipeline_test.py unit
python toolchain/pipeline_test.py component
```

### CI/CD Integration

Use in CI/CD pipelines:

```yaml
# GitHub Actions example
- name: Build
  run: python toolchain/run-pipeline.py clean-build

- name: Test
  run: python toolchain/run-pipeline.py test

- name: Analyze
  run: python toolchain/run-pipeline.py static-analysis
```

---

## Troubleshooting

### Build Failures

**Problem:** CMake configuration fails
```
Solution:
1. Check CMakeLists.txt for syntax errors
2. Verify MinGW-w64 is in PATH
3. Review toolchain/results/cmake_configure.txt for details
```

**Problem:** Compilation errors
```
Solution:
1. Check toolchain/results/build_log.txt for error messages
2. Verify source code syntax
3. Clean build and retry: python toolchain/run-pipeline.py rebuild
```

### Test Failures

**Problem:** Tests fail on one motor type
```
Solution:
1. Check motor_config.cpp configuration
2. Review test_results_MT_BASIC.txt or test_results_MT_ROBUST.txt
3. Verify motor-specific logic is properly abstracted
```

**Problem:** Motor configuration not restored
```
Solution:
Run manually: Edit src/motor_config.cpp and set to MT_BASIC
Or use: python toolchain/pipeline_test.py test (will restore)
```

### Static Analysis Issues

**Problem:** cppcheck not found
```
Solution:
Install via MSYS2: pacman -S mingw-w64-x86_64-cppcheck
Add to PATH: export PATH="/c/msys64/mingw64/bin:$PATH"
```

**Problem:** Too many warnings
```
Solution:
Review toolchain/results/static_analysis.txt
Priority: Fix errors first, then warnings, then style
Suppress false positives in code with: // cppcheck-suppress [rule]
```

### General Issues

**Problem:** Python import errors
```
Solution:
Ensure you're in workspace root: cd /path/to/deskautomation
Use correct Python version: python --version (should be 3.7+)
Check imports are from same directory: ls toolchain/
```

**Problem:** Permission denied
```
Solution:
Windows: Run as Administrator or check antivirus
Linux/Mac: Check file permissions: chmod +x toolchain/*.py
```

---

## Pipeline Architecture

The toolchain uses a modular design:

```
toolchain/                 # Located at workspace root (parallel to src/, tests/)
├── run-pipeline.py        # Main orchestrator (use this for most tasks)
├── pipeline_build.py      # Build operations
├── pipeline_test.py       # Test operations
├── pipeline_analysis.py   # Static analysis
├── pipeline_common.py     # Shared utilities
├── results/               # Pipeline output files (auto-created)
├── README.md              # This file - user guide
└── ARCHITECTURE.md        # Technical architecture details
```

**Design Principles:**
- **Modularity** - Each script has single responsibility
- **Reusability** - Subscripts work independently or orchestrated
- **Maintainability** - Small, focused files are easier to modify
- **Extensibility** - Add new subscripts without changing existing ones

**Exit Codes:**
- `0` - Success
- `1` - Failure (build, test, or analysis failed)

---

## Quick Reference Card

```bash
# Most Common Commands
python toolchain/run-pipeline.py all          # Full pipeline
python toolchain/run-pipeline.py clean-build  # Fresh build
python toolchain/run-pipeline.py test         # Dual motor test
python toolchain/run-pipeline.py unit         # Quick unit test

# Check Results
ls toolchain/results/                                   # List all results
cat toolchain/results/build_log.txt                     # Build output
cat toolchain/results/test_results_MT_BASIC.txt         # Test results

# Get Help
python toolchain/run-pipeline.py help         # Show all commands
python toolchain/pipeline_build.py --help     # Build subscript help
python toolchain/pipeline_test.py --help      # Test subscript help
```

---

**For more details, see:**
- [ARCHITECTURE.md](ARCHITECTURE.md) - Technical architecture and implementation details
- [coding_guides_and_checks.md](coding_guides_and_checks.md) - Comprehensive coding standards (NASA, MISRA C:2012, ISO 25119) with automation guidance
- [../tests/README.md](../tests/README.md) - Test suite overview  
- [../src/README.md](../src/README.md) - Full project documentation
- [../README.md](../README.md) - Main project documentation

**Last Updated:** February 2026
