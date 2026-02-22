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
- `rules-all` - Run all 32 compliance rules
- `rules-group` - Run a specific rule group
- `rule` - Run a single rule
- `rules-list` - List all available rules

**Standalone Usage:**
```bash
# Comprehensive cppcheck analysis
python toolchain/pipeline_analysis.py static-analysis

# Run all compliance rules and generate summary
python toolchain/pipeline_analysis.py rules-all --src-path src

# Run a specific rule group
python toolchain/pipeline_analysis.py rules-group --group nasa --src-path src

# Run a single rule
python toolchain/pipeline_analysis.py rule --rule-id RULE-001 --src-path src

# List all available rules
python toolchain/pipeline_analysis.py rules-list
```

**What It Does:**
1. Runs cppcheck with all checks enabled:
   - Errors, warnings, style issues, performance, portability
   - Generates text and XML reports
2. Executes 32 automated compliance rules across 6 categories:
   - NASA Power of Ten (17 rules)
   - MISRA C:2012 (13 rules)
   - ISO 25119 (2 rules)
   - Compiler checks (6 rules)
   - Static analysis (12 rules)
   - Custom heuristics (12 rules)
3. Generates per-rule logs and grouped summary
4. Saves results to `toolchain/results/`

---

## Automated Rules and Compliance Checks

The toolchain includes 32 automated compliance rules covering safety-critical coding standards.

### Rule Groups Overview

#### 🛸 **NASA Power of Ten Rules** (17 rules)
**What:** Critical safety rules from NASA/JPL for resource-constrained embedded systems.

**Focus Areas:**
- Simple control flow (no goto, no infinite loops except main)
- Fixed loop bounds and limited recursion
- No dynamic memory allocation
- Short functions and low complexity
- Defensive assertions and NULL checks
- Minimal variable scope
- Return value validation

**Why Important:** Ensures predictability, reliability, and safety-critical code quality through strict control flow discipline and defensive assertions.

**Example Checks:**
- RULE-001: No goto statements
- RULE-007: No while(true) except main
- RULE-009: No malloc/calloc/realloc/free
- RULE-012: Function length ≤ 60 lines
- RULE-026: NULL pointer checks

#### 🏎️ **MISRA C:2012** (13 rules)
**What:** Motor Industry Software Reliability Association guidelines. Used in automotive and safety-critical domains.

**Focus Areas:**
- Prevent undefined behavior and misuse of language features
- Type safety and arithmetic integrity
- Code quality and maintainability
- Control flow safety

**Why Important:** Prevents undefined behavior, improves code safety, portability, and maintainability across automotive and safety-critical systems.

**Example Checks:**
- RULE-051: No dead code (unreachable statements)
- RULE-054: No uninitialized auto variables
- RULE-058: No object pointer casting
- RULE-062: Switch clauses end with break
- RULE-065: No dynamic allocation functions

#### 🔧 **ISO 25119** (2 rules)
**What:** Functional safety requirements for machinery control systems. Specific to safety-critical machinery.

**Focus Areas:**
- Requirement traceability with SWReq and SysReq tags
- Defensive programming practices
- Assert density for safety verification

**Why Important:** Ensures requirement traceability, defensive programming practices, and compliance with functional safety standards for machinery.

**Example Checks:**
- RULE-067: Requirement traceability (SWReq-/SysReq- tags)
- RULE-068: Assert density (minimum 1 per function for safety)

#### ⚙️ **Compiler Checks** (6 rules)
**What:** Compiler flags and build configuration validation.

**Focus Areas:**
- Required compiler warning flags present
- Zero compiler warnings in build
- Aggressive warning levels enabled

**Why Important:** Enforces aggressive warning levels and error detection at compile time to catch potential issues early, before they reach code review or testing.

**Example Checks:**
- RULE-037: Required compiler flags present (-Wall, -Wextra, -Werror, etc.)
- RULE-038: Zero compiler warnings in build log

#### 🔍 **Static Analysis (Cppcheck)** (12 rules)
**What:** Comprehensive static analysis to detect potential runtime errors without execution.

**Focus Areas:**
- Memory management issues
- Undefined behavior detection
- Code quality problems
- Type safety

**Why Important:** Catches potential runtime errors, memory issues, undefined behavior, and code quality problems statically before execution.

**Example Checks:**
- RULE-040: Zero cppcheck errors in analysis
- RULE-049: No undefined behavior
- RULE-051: No dead code
- RULE-054: No uninitialized variables

#### 🧮 **Custom Heuristics** (12 rules)
**What:** Project-specific pattern detection and analysis.

**Focus Areas:**
- Stack usage per function (≤512 bytes heuristic)
- Function complexity metrics
- TRUE NULL pointer and pointer arithmetic detection
- Initialization at declaration

**Why Important:** Provides project-specific safety checks tailored to the embedded system's resource constraints and architectural requirements.

**Example Checks:**
- RULE-011: Stack usage per function ≤ 512 bytes
- RULE-014: Cyclomatic complexity ≤ 10
- RULE-023: Variables initialized at declaration
- RULE-033: No pointer arithmetic

### Running Rules - Tutorial

#### Run All Rules with Summary Report

```bash
# Generate complete compliance report
python toolchain/run-pipeline.py all

# Or run rules-all directly
python toolchain/pipeline_analysis.py rules-all --src-path src
```

**Output:**
- Individual rule logs: `toolchain/results/RULE-*.log`
- Summary report: `toolchain/results/rules_summary.md` (see below)

#### Run Specific Rule Group

```bash
# Run all NASA rules
python toolchain/pipeline_analysis.py rules-group --group nasa --src-path src

# Run all MISRA rules
python toolchain/pipeline_analysis.py rules-group --group misra --src-path src

# Run all ISO rules
python toolchain/pipeline_analysis.py rules-group --group iso --src-path src
```

**Available Groups:**
- `nasa` - NASA Power of Ten rules
- `misra` - MISRA C:2012 rules
- `iso` - ISO 25119 rules
- `compiler` - Compiler checks
- `cppcheck` - Static analysis rules
- `heuristic` - Custom heuristics

#### Run Single Rule

```bash
# Check for no goto statements
python toolchain/pipeline_analysis.py rule --rule-id RULE-001 --src-path src

# Check function length limits
python toolchain/pipeline_analysis.py rule --rule-id RULE-012 --src-path src

# Check requirement traceability
python toolchain/pipeline_analysis.py rule --rule-id RULE-067 --src-path src
```

#### List All Available Rules

```bash
# Show all rules with descriptions
python toolchain/pipeline_analysis.py rules-list
```

### Rule Summary Report

After running `rules-all`, a summary report is generated at `toolchain/results/rules_summary.md`:

**Example Output:**
```markdown
# Rule Summary

**Generated:** 2026-02-22 16:06:18
**Source Path:** `src`

**Guidelines:** [Coding Guidelines and Automated Checks](../coding_guides_and_checks.md)

## Status Summary

✅ **Total PASS:** 32
❌ **Total FAIL:** 0

## Group: NASA
**NASA Power of Ten Rules** - Critical safety rules from NASA/JPL...

| Rule ID | Status | Instances | Log |
|---------|--------|-----------|-----|
| RULE-001 | ✅ PASS | - | [RULE-001.log](./RULE-001.log) |
| RULE-007 | ✅ PASS | - | [RULE-007.log](./RULE-007.log) |
| RULE-012 | ✅ PASS | - | [RULE-012.log](./RULE-012.log) |
...
```

**Report Sections:**
1. **Header** - Timestamp and source path being analyzed
2. **Guidelines Link** - Direct link to comprehensive coding standards
3. **Status Summary** - Total PASS ✅ and FAIL ❌ counts
4. **Group Sections** - For each group:
   - Group name and description
   - Table with:
     - Rule ID
     - Status (✅ PASS or ❌ FAIL with icon)
     - Instances (count of violations, only for FAIL)
     - Log file (clickable link to detailed results)

**Interpreting Results:**
- ✅ PASS + Instances "-" = Rule passed, no violations
- ❌ FAIL + Instances "N" = N violations of this rule found
- Click log file link to see detailed violation information

### Integration with CI/CD

Run rules-all in CI/CD pipelines:

```yaml
# GitHub Actions example
- name: Run Compliance Rules
  run: python toolchain/pipeline_analysis.py rules-all --src-path src

# Check results
- name: Verify No Failures
  run: |
    grep "❌ **Total FAIL:** 0" toolchain/results/rules_summary.md || exit 1
```



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

### Compliance Rules Results
- **`rules_summary.md`** - Comprehensive compliance summary report
  - Status overview (✅ PASS / ❌ FAIL counts)
  - Groups: NASA, MISRA, ISO, Compiler, Cppcheck, Heuristic
  - Each group includes description of checks and why they're important
  - Timestamp of report generation
  - Source path analyzed
  - Link to detailed coding guidelines
  - Per-rule tables with:
    - Rule ID (e.g., RULE-001)
    - Status icon (✅ or ❌)
    - Violation count (for failed rules)
    - Links to detailed rule logs

- **`RULE-*.log`** - Individual rule execution logs (one per rule)
  - Rule ID and description
  - Status (PASS, FAIL, SKIP)
  - Detailed findings with file locations
  - Context on violations found

### Viewing Results

```bash
# View compliance summary
cat toolchain/results/rules_summary.md

# View specific rule details
cat toolchain/results/RULE-001.log
cat toolchain/results/RULE-067.log

# View build log
cat toolchain/results/build_log.txt

# View test results
cat toolchain/results/test_results_MT_BASIC.txt
cat toolchain/results/test_results_MT_ROBUST.txt

# View static analysis
cat toolchain/results/static_analysis.txt

# Open all results in editor
code toolchain/results/
```

**Quick Status Check:**
```bash
# Check if all rules passed
grep "Total PASS: 32" toolchain/results/rules_summary.md && echo "✅ All rules passed!"

# Find failures
grep "❌ FAIL" toolchain/results/rules_summary.md
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
python toolchain/run-pipeline.py all          # Full pipeline (build + test + rules)
python toolchain/run-pipeline.py clean-build  # Fresh build
python toolchain/run-pipeline.py test         # Dual motor test
python toolchain/run-pipeline.py unit         # Quick unit test

# Compliance Rules - Complete Validation
python toolchain/pipeline_analysis.py rules-all --src-path src      # All 32 rules + summary
python toolchain/pipeline_analysis.py rules-group --group nasa --src-path src    # NASA rules only
python toolchain/pipeline_analysis.py rule --rule-id RULE-001 --src-path src     # Single rule
python toolchain/pipeline_analysis.py rules-list                     # List all rules

# Check Results
ls toolchain/results/                                   # List all results
cat toolchain/results/rules_summary.md                  # Compliance summary
cat toolchain/results/build_log.txt                     # Build output
cat toolchain/results/test_results_MT_BASIC.txt         # Test results
grep "❌ FAIL" toolchain/results/rules_summary.md       # Show failures

# Get Help
python toolchain/run-pipeline.py help         # Show all commands
python toolchain/pipeline_build.py --help     # Build subscript help
python toolchain/pipeline_test.py --help      # Test subscript help
python toolchain/pipeline_analysis.py rules-list       # List all rules
```

---

**For more details, see:**
- [ARCHITECTURE.md](ARCHITECTURE.md) - Technical architecture and implementation details
- [coding_guides_and_checks.md](coding_guides_and_checks.md) - Comprehensive coding standards (NASA, MISRA C:2012, ISO 25119) with automation guidance
- [../tests/README.md](../tests/README.md) - Test suite overview  
- [../src/README.md](../src/README.md) - Full project documentation
- [../README.md](../README.md) - Main project documentation

**Last Updated:** February 2026
