# Pipeline Scripts - Quick Reference

## 🚀 Most Common Commands

**Start here for typical workflows:**

```bash
# Complete pipeline (recommended before commits)
python toolchain/run-pipeline.py all

# Build project
python toolchain/run-pipeline.py clean-build

# Run all tests with dual motor validation
python toolchain/run-pipeline.py test

# Quick unit tests during development
python toolchain/run-pipeline.py unit

# Get help
python toolchain/run-pipeline.py help
```

**📖 Complete Guide:** [../toolchain/README.md](../toolchain/README.md) - Full pipeline manual with troubleshooting

---

## Architecture

The pipeline system is modular with specialized subscripts for different operations:

| Script | Purpose | Commands |
|--------|---------|----------|
| **../toolchain/run-pipeline.py** | Main orchestrator | `clean-build`, `rebuild`, `test`, `unit`, `component`, `integration`, `clean-test`, `static-analysis`, `all`, `help` |
| **../toolchain/pipeline_build.py** | Build operations | `clean`, `configure`, `build`, `clean-build`, `rebuild` |
| **../toolchain/pipeline_test.py** | Test operations | `test`, `unit`, `component`, `integration` |
| **../toolchain/pipeline_analysis.py** | Static analysis | `static-analysis` |
| **../toolchain/pipeline_common.py** | Shared utilities | Configuration, colors, command execution |

## Primary Script (Recommended)
**Main Pipeline:** `toolchain/run-pipeline.py`
- **Language:** Python 3.7+
- **Cross-platform:** Windows, Linux, macOS
- **Usage:** `python toolchain/run-pipeline.py <command>`
- **Purpose:** Orchestrates all pipeline operations

## Subscripts (Advanced Usage)
Can be run independently for focused operations:

```bash
# Build operations only
python toolchain/pipeline_build.py clean-build

# Test operations only
python toolchain/pipeline_test.py unit

# Static analysis only
python toolchain/pipeline_analysis.py static-analysis
```

---

## Available Commands (Main Pipeline)
- `clean-build` - Clean build directory and build the project
- `rebuild` - Full rebuild (clean + configure + build)
- `test` - Run all test cases with dual motor type validation (MT_BASIC + MT_ROBUST)
- `unit` - Run unit tests only (current motor configuration)
- `component` - Run component tests only (current motor configuration)
- `integration` - Run integration tests only (current motor configuration)
- `clean-test` - Clean, build, and run all test cases with dual motor validation
- `static-analysis` - Run cppcheck static analysis
- `all` - Run complete pipeline (rebuild + dual motor test + static analysis)
- `help` - Show help message

## Quick Examples

### Main Pipeline (Full Integration)
```bash
# Full build
python toolchain/run-pipeline.py clean-build

# Dual motor type testing (MT_BASIC + MT_ROBUST)
python toolchain/run-pipeline.py test

# Run specific test suites (current motor type)
python toolchain/run-pipeline.py unit
python toolchain/run-pipeline.py component
python toolchain/run-pipeline.py integration

# Complete pipeline
python toolchain/run-pipeline.py all
```

### Subscripts (Direct Usage)
```bash
# Build operations only
python toolchain/pipeline_build.py rebuild

# Test operations only
python toolchain/pipeline_test.py unit

# Static analysis only
python toolchain/pipeline_analysis.py static-analysis
```

## Output Location
All scripts save results to: `toolchain/results/`
- `cmake_configure.txt` - CMake configuration log
- `build_log.txt` - Build output with compiler diagnostics
- `test_results_MT_BASIC.txt` - Test results for L298N driver
- `test_results_MT_ROBUST.txt` - Test results for IBT_2 driver
- `test_results_UnitTests.txt` - Unit test results (when run individually)
- `test_results_ComponentTests.txt` - Component test results (when run individually)
- `test_results_IntegrationTests.txt` - Integration test results (when run individually)
- `static_analysis.txt` - Human-readable analysis report
- `static_analysis.xml` - Machine-readable XML for CI

## Dual Motor Type Testing
The `test` and `clean-test` commands automatically:
1. Set motor type to MT_BASIC (L298N)
2. Build and run all 37 tests
3. Set motor type to MT_ROBUST (IBT_2)
4. Rebuild and run all 37 tests
5. Restore default configuration (MT_BASIC)

This ensures the application logic is motor-type-agnostic.

## Prerequisites
- Python 3.7+ (for pipeline scripts)
- MSYS2 with MinGW-w64 toolchain
- CMake
- cppcheck (optional, for static analysis): `pacman -S mingw-w64-x86_64-cppcheck`

## Benefits of Modular Architecture
1. **Separation of Concerns** - Each subscript handles one responsibility
2. **Reusability** - Subscripts can be used independently or orchestrated
3. **Easier Testing** - Individual components can be tested in isolation
4. **Better Maintainability** - Smaller, focused files are easier to understand and modify
5. **Extensibility** - New subscripts can be added without modifying existing ones

## Documentation
- [PIPELINE_ARCHITECTURE.md](toolchain/PIPELINE_ARCHITECTURE.md) - Detailed architecture documentation
- [PIPELINE_MIGRATION.md](toolchain/PIPELINE_MIGRATION.md) - PowerShell → Python migration notes
