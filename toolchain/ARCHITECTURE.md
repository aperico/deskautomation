# Pipeline Architecture Documentation

## Overview
The deskautomation project uses a modular Python-based pipeline system for building, testing, and analyzing code. The architecture separates concerns into specialized subscripts that can be used independently or orchestrated together.

All pipeline tooling is located in `tests/toolchain/` for clear organization.

## File Structure
```
tests/
└── toolchain/
    ├── run-pipeline.py           # Main orchestrator
    ├── pipeline_build.py         # Build operations
    ├── pipeline_test.py          # Test operations
    ├── pipeline_analysis.py      # Static analysis
    ├── pipeline_common.py        # Shared utilities
    ├── PIPELINE_ARCHITECTURE.md  # This file
    └── PIPELINE_MIGRATION.md     # Migration notes
```

## Design Principles
1. **Modularity** - Each subscript handles a single responsibility
2. **Reusability** - Subscripts can be called independently or orchestrated
3. **Maintainability** - Smaller, focused modules are easier to understand and modify
4. **Extensibility** - New functionality can be added without modifying existing code
5. **Testability** - Individual components can be tested in isolation

## Architecture Diagram
```
┌─────────────────────────────────────────────────────────────┐
│                    run-pipeline.py                          │
│                  (Main Orchestrator)                        │
│                                                             │
│  Commands: clean-build, rebuild, test, unit, component,    │
│           integration, clean-test, static-analysis, all     │
└───────────────┬─────────────┬─────────────┬────────────────┘
                │             │             │
        ┌───────▼──────┐ ┌───▼─────────┐ ┌─▼──────────────┐
        │              │ │             │ │                │
        │ pipeline_    │ │ pipeline_   │ │ pipeline_      │
        │ build.py     │ │ test.py     │ │ analysis.py    │
        │              │ │             │ │                │
        └──────┬───────┘ └──────┬──────┘ └────────┬───────┘
               │                │                  │
               │                │                  │
        ┌──────▼────────────────▼──────────────────▼───────┐
        │            pipeline_common.py                     │
        │     (Shared: Config, Colors, run_command)        │
        └───────────────────────────────────────────────────┘
```

## Components

### 1. run-pipeline.py (Main Orchestrator)
**Purpose:** Main entry point that orchestrates all pipeline operations

**Key Features:**
- Command-line interface for all pipeline operations
- Delegates to specialized subscripts
- Coordinates complex workflows (e.g., `all` command)
- Provides unified help and error handling

**Commands:**
- `clean-build` - Clean, configure, and build
- `rebuild` - Alias for clean-build
- `test` - Dual motor type testing (MT_BASIC + MT_ROBUST)
- `unit`, `component`, `integration` - Specific test suites
- `clean-test` - Full rebuild + dual motor testing
- `static-analysis` - Run cppcheck
- `all` - Complete pipeline (build + test + analysis)

**Usage:**
```bash
python tests/toolchain/run-pipeline.py <command>
```

### 2. pipeline_build.py (Build Subscript)
**Purpose:** Handles all build-related operations

**Key Features:**
- CMake configuration
- Project building
- Build artifact management
- Compiler diagnostic parsing and display

**Commands:**
- `clean` - Remove build directory
- `configure` - Run CMake configuration
- `build` - Build the project
- `clean-build` / `rebuild` - Full rebuild

**Standalone Usage:**
```bash
python tests/toolchain/pipeline_build.py clean-build
```

**Core Methods:**
- `clean()` - Clean build directory
- `configure()` - Run CMake configuration with logging
- `build()` - Build project and capture diagnostics

### 3. pipeline_test.py (Test Subscript)
**Purpose:** Handles all testing operations

**Key Features:**
- Dual motor type validation (MT_BASIC + MT_ROBUST)
- Individual test suite execution
- Motor configuration management
- Test result parsing and reporting

**Commands:**
- `test` - Dual motor type validation (full test suite)
- `unit` - Run unit tests only
- `component` - Run component tests only
- `integration` - Run integration tests only

**Standalone Usage:**
```bash
python tests/toolchain/pipeline_test.py unit
```

**Core Methods:**
- `test_dual_motor()` - Test both motor configurations
- `run_tests(test_filter)` - Run specific test suite
- `set_motor_type(motor_type)` - Switch motor configuration

### 4. pipeline_analysis.py (Analysis Subscript)
**Purpose:** Handles static code analysis

**Key Features:**
- Comprehensive cppcheck analysis
- XML and text report generation
- Issue categorization by severity
- Detailed diagnostic display

**Commands:**
- `static-analysis` - Run cppcheck with all checks enabled

**Standalone Usage:**
```bash
python tests/toolchain/pipeline_analysis.py static-analysis
```

**Core Methods:**
- `static_analysis()` - Run cppcheck and parse results

### 5. pipeline_common.py (Shared Utilities)
**Purpose:** Provides shared functionality for all subscripts

**Key Features:**
- Configuration management
- Environment setup
- Command execution wrapper
- Color-coded console output
- Path management

**Classes:**
- `Config` - Pipeline configuration (paths, environment)
- `Colors` - ANSI color codes for terminal output

**Functions:**
- `run_command()` - Execute commands with error handling
- `colorize()` - Apply colors to text
- `write_success()`, `write_info()`, `write_error()`, `write_header()` - Formatted output

## Data Flow

### Example: `python run-pipeline.py all`
```
1. run-pipeline.py receives 'all' command
   │
   ├─> Creates Config object (pipeline_common.py)
   │   └─> Sets up paths, environment
   │
   ├─> Instantiates BuildPipeline(config)
   │   ├─> Calls clean()
   │   ├─> Calls configure()
   │   └─> Calls build()
   │
   ├─> Instantiates TestPipeline(config)
   │   ├─> Calls set_motor_type("MT_BASIC")
   │   ├─> Builds and runs tests
   │   ├─> Calls set_motor_type("MT_ROBUST")
   │   ├─> Builds and runs tests
   │   └─> Restores MT_BASIC
   │
   └─> Instantiates AnalysisPipeline(config)
       └─> Calls static_analysis()
```

## Output Management

All pipeline operations save results to `tests/results/`:

| File | Producer | Content |
|------|----------|---------|
| `cmake_configure.txt` | pipeline_build.py | CMake configuration output and warnings |
| `build_log.txt` | pipeline_build.py | Build output with compiler diagnostics |
| `test_results_MT_BASIC.txt` | pipeline_test.py | Test results for L298N motor driver |
| `test_results_MT_ROBUST.txt` | pipeline_test.py | Test results for IBT_2 motor driver |
| `test_results_UnitTests.txt` | pipeline_test.py | Unit test results (when run individually) |
| `test_results_ComponentTests.txt` | pipeline_test.py | Component test results |
| `test_results_IntegrationTests.txt` | pipeline_test.py | Integration test results |
| `static_analysis.txt` | pipeline_analysis.py | Human-readable cppcheck report |
| `static_analysis.xml` | pipeline_analysis.py | Machine-readable XML for CI |

## Error Handling

All subscripts follow consistent error handling patterns:
1. Return boolean success/failure status
2. Write errors using `write_error()`
3. Save detailed logs to `results/` directory
4. Exit with code 1 on failure, 0 on success

## Extension Points

### Adding a New Subscript
1. Create `tests/toolchain/pipeline_<name>.py`
2. Import from `pipeline_common`
3. Implement `<Name>Pipeline` class with operation methods
4. Add `main()` function for standalone execution
5. Update `run-pipeline.py` to use the new subscript
6. Add new commands to help text

### Example Template:
```python
#!/usr/bin/env python3
from pipeline_common import Config, write_success, write_error
from datetime import datetime

class NewPipeline:
    def __init__(self, config=None):
        self.config = config if config else Config()
    
    def operation(self):
        # Implementation
        return True

def main():
    # Standalone execution
    pass

if __name__ == '__main__':
    sys.exit(main())
```

## Benefits of This Architecture

### For Developers
- Quick iteration: Run only needed operations
- Clear separation: Easy to find and modify specific functionality
- Debugging: Test subscripts in isolation
- Flexibility: Combine operations as needed

### For CI/CD
- Granular control: Run specific pipeline stages
- Parallel execution: Multiple subscripts can run in parallel
- Consistent output: Uniform logging and result format
- Easy integration: XML output for test result parsing

### For Maintenance
- Single responsibility: Each file has one clear purpose
- Reduced coupling: Changes to one subscript don't affect others
- Better testability: Unit test individual subscripts
- Clear dependencies: Import structure shows relationships

## Migration Path

The old monolithic `run-pipeline.py` has been refactored into:
- **pipeline_common.py** - Configuration and utilities (lines 1-130 → extracted)
- **pipeline_build.py** - Build operations (lines 131-270 → extracted)
- **pipeline_test.py** - Test operations (lines 271-410 → extracted)
- **pipeline_analysis.py** - Analysis operations (lines 411-550 → extracted)
- **run-pipeline.py** - Orchestration layer (lines 551-627 → simplified to 210 lines)

All functionality is preserved with enhanced modularity.

## Future Enhancements

Potential extensions using this architecture:
1. **Coverage Pipeline** - Code coverage analysis subscript
2. **Documentation Pipeline** - Doxygen/Sphinx generation subscript
3. **Deployment Pipeline** - Firmware packaging and upload subscript
4. **Performance Pipeline** - Benchmarking and profiling subscript
5. **Security Pipeline** - Security scanning and vulnerability checks

Each can be added as a new subscript without modifying existing code.

---

**Last Updated:** February 22, 2026  
**Version:** 2.0 (Modular Architecture)
