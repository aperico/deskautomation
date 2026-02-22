# Coding Guidelines and Automated Checks for Safety-Critical Embedded Software


**Document Revision History:**

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| 1.0 | 2026-02-22 | A. Perico + A.I. | Initial version based on NASA Power of Ten |
| 1.1 | 2026-02-22 | A. Perico + A.I. | Added MISRA C:2012 and ISO 25119 rules with automation guidance |

**Approval:**

- [ ] Software Lead
- [ ] Safety Engineer  
- [ ] Quality Manager
--

**Project:** Automated Mechanical Desk Lift System  
**Compliance Standards:**
- ISO 25119 (Functional Safety for Agricultural and Forestry Machinery)
- ASPICE Level 2 (Automotive SPICE)
- MISRA C:2012 (Selected Rules)
- NASA/JPL "Power of Ten" Rules for Safety-Critical Code

**Reference Documents:**
- [NASA Power of Ten Rules](resources/nasa10rules.pdf)
- MISRA C:2012 Guidelines for the use of the C language in critical systems
- ISO 25119-3:2018 Software design and development requirements

**Last Updated:** February 22, 2026

---

## Table of Contents

- [Coding Guidelines and Automated Checks for Safety-Critical Embedded Software](#coding-guidelines-and-automated-checks-for-safety-critical-embedded-software)
  - [Table of Contents](#table-of-contents)
  - [Rule Index (Unified IDs)](#rule-index-unified-ids)
  - [Introduction](#introduction)
    - [Why These Rules?](#why-these-rules)
    - [Scope](#scope)
    - [Enforcement](#enforcement)
  - [Rule 1: Simple Control Flow](#rule-1-simple-control-flow)
    - [Requirements](#requirements)
  - [Rule 2: Fixed Loop Bounds](#rule-2-fixed-loop-bounds)
    - [Requirements](#requirements-1)
  - [Rule 3: No Dynamic Memory Allocation](#rule-3-no-dynamic-memory-allocation)
    - [Requirements](#requirements-2)
  - [Rule 4: Short Functions](#rule-4-short-functions)
    - [Requirements](#requirements-3)
  - [Rule 5: Defensive Assertions](#rule-5-defensive-assertions)
    - [Requirements](#requirements-4)
  - [Rule 6: Minimal Variable Scope](#rule-6-minimal-variable-scope)
    - [Requirements](#requirements-5)
  - [Rule 7: Check Return Values](#rule-7-check-return-values)
    - [Requirements](#requirements-6)
  - [Rule 8: Limited Preprocessor Use](#rule-8-limited-preprocessor-use)
    - [Requirements](#requirements-7)
  - [Rule 9: Restricted Pointer Use](#rule-9-restricted-pointer-use)
    - [Requirements](#requirements-8)
  - [Rule 10: Aggressive Compiler Warnings and Static Analysis](#rule-10-aggressive-compiler-warnings-and-static-analysis)
    - [Requirements](#requirements-9)
  - [Project-Specific Guidelines](#project-specific-guidelines)
    - [Naming Conventions](#naming-conventions)
    - [Safety-Critical Code](#safety-critical-code)
    - [Testing Requirements](#testing-requirements)
  - [Rule 14: Code Quality Metrics](#rule-14-code-quality-metrics)
    - [Requirements](#requirements-10)
  - [MISRA C:2012 Compliance Rules](#misra-c2012-compliance-rules)
    - [Overview](#overview)
    - [Core MISRA C Rules for Safety-Critical Systems](#core-misra-c-rules-for-safety-critical-systems)
    - [MISRA Rule Categories](#misra-rule-categories)
  - [ISO 25119 Coding Requirements](#iso-25119-coding-requirements)
    - [ISO 25119-3 Software Design Rules](#iso-25119-3-software-design-rules)
    - [Defensive Programming (ISO 25119)](#defensive-programming-iso-25119)
  - [Automated Checking](#automated-checking)
    - [Comprehensive Automated Checks Table](#comprehensive-automated-checks-table)
    - [Automation Scripts Reference](#automation-scripts-reference)
    - [Manual Review Rules (Require human judgment)](#manual-review-rules-require-human-judgment)
  - [Appendix: Quick Reference](#appendix-quick-reference)
    - [Critical Rules (Must Never Violate)](#critical-rules-must-never-violate)
    - [Recommended Practices](#recommended-practices)
    - [Automated Checking Command](#automated-checking-command)
    - [Tool Configuration](#tool-configuration)
  - [Conclusion](#conclusion)

---

## Rule Index (Unified IDs)

This index assigns a unique, portable identifier to every rule in this document.

**Legend:**
- 🤖 **Automated Script:** Full automation available
- 🔍 **Code Review:** Manual review required  
- 📊 **Tool-Based:** Use external tools (compiler, cppcheck, gcov, etc.)
- 📝 **Pattern Match:** Simple grep/regex pattern matching available

| Unified ID | Original ID | Rule Title | Automation | Method |
|------------|-------------|------------|-----------|--------|
| RULE-001 | R1.1 | No goto statements | 🤖 | [rule_RULE-001_R1_1_no_goto.py](../toolchain/rules/rule_RULE-001_R1_1_no_goto.py) |
| RULE-002 | R1.2 | No setjmp/longjmp | 🤖 | [rule_RULE-002_R1_2_no_setjmp_longjmp.py](../toolchain/rules/rule_RULE-002_R1_2_no_setjmp_longjmp.py) |
| RULE-003 | R1.3 | No recursion | 🤖 | [rule_RULE-003_R1_3_no_recursion.py](../toolchain/rules/rule_RULE-003_R1_3_no_recursion.py) |
| RULE-004 | R1.4 | Single entry/exit | 🔍 | Code review - verify each function has one entry point and exit sequence |
| RULE-005 | R2.1 | Bounded loops | 🔍 | Loop termination proof needed; examine loop counters and bounds |
| RULE-006 | R2.2 | Loop counter limits | 🔍 | Code review - verify loop variables have explicit bounds |
| RULE-007 | R2.3 | No while(true) except main | 🤖 | [rule_RULE-007_R2_3_no_while_true.py](../toolchain/rules/rule_RULE-007_R2_3_no_while_true.py) |
| RULE-008 | R2.4 | Loop invariants documented | 🔍 | Code review - check comments document loop invariants for complex loops |
| RULE-009 | R3.1 | No malloc/calloc/realloc/free | 🤖 | [rule_RULE-009_R3_1_no_malloc_free.py](../toolchain/rules/rule_RULE-009_R3_1_no_malloc_free.py) |
| RULE-010 | R3.2 | Avoid hidden dynamic allocation | 🔍 | Library audit + code review; check for implicit allocation in dependencies |
| RULE-011 | R3.4 | Stack size limits | 🤖 | [rule_RULE-011_R3_4_stack_limits.py](../toolchain/rules/rule_RULE-011_R3_4_stack_limits.py) |
| RULE-012 | R4.1 | Function length limit | 🤖 | [rule_RULE-012_R4_1_function_length.py](../toolchain/rules/rule_RULE-012_R4_1_function_length.py) |
| RULE-013 | R4.2 | Single responsibility | 🔍 | Design review - verify each function has one clear purpose |
| RULE-014 | R4.3 | Cyclomatic complexity <= 10 | 🤖 | [rule_RULE-014_R4_3_complexity.py](../toolchain/rules/rule_RULE-014_R4_3_complexity.py) |
| RULE-015 | R5.1 | Assertion density | 🤖 | [rule_RULE-053_R14_5_assertion_density.py](../toolchain/rules/rule_RULE-053_R14_5_assertion_density.py) (see R14.5) |
| RULE-016 | R5.2 | Precondition checks | 🔍 | Code review - verify functions validate input parameters at entry |
| RULE-017 | R5.3 | Invariant checks | 🔍 | Code review - verify critical invariants guarded by assertions |
| RULE-018 | R5.4 | Assertions in release builds | 🤖 | [rule_RULE-018_R5_4_ndebug.py](../toolchain/rules/rule_RULE-018_R5_4_ndebug.py) |
| RULE-019 | R5.5 | No side effects in assertions | 🤖 | [rule_RULE-019_R5_5_assert_side_effects.py](../toolchain/rules/rule_RULE-019_R5_5_assert_side_effects.py) |
| RULE-020 | R6.1 | Local scope preferred | 🔍 | Code review - verify variables declared in innermost scope possible |
| RULE-021 | R6.2 | Minimize global variables | 📝 | 📊 Pattern: `grep -r "^[^/]*\b\w\+\s\+\*\?[a-z_]\+\s*[=;]" src/*.c` (heuristic) |
| RULE-022 | R6.3 | Static variables documented | 🤖 | [rule_RULE-022_R6_3_static_documented.py](../toolchain/rules/rule_RULE-022_R6_3_static_documented.py) |
| RULE-023 | R6.4 | Initialize at declaration | 🤖 | [rule_RULE-023_R6_4_init_at_decl.py](../toolchain/rules/rule_RULE-023_R6_4_init_at_decl.py) |
| RULE-024 | R7.1 | Check all return values | 🤖 | [rule_RULE-024_R7_1_unused_return.py](../toolchain/rules/rule_RULE-024_R7_1_unused_return.py) |
| RULE-025 | R7.2 | Parameter validation | 🔍 | Code review - verify public functions check parameters at entry |
| RULE-026 | R7.3 | Pointer parameter checks | 🤖 | [rule_RULE-026_R7_3_null_checks.py](../toolchain/rules/rule_RULE-026_R7_3_null_checks.py) |
| RULE-027 | R7.4 | Error propagation | 🔍 | Code review - verify error codes propagate up call chain correctly |
| RULE-028 | R8.1 | Include guards only | 🤖 | [rule_RULE-028_R8_1_include_guards.py](../toolchain/rules/rule_RULE-028_R8_1_include_guards.py) |
| RULE-029 | R8.2 | No function-like macros | 🤖 | [rule_RULE-029_R8_2_no_function_macros.py](../toolchain/rules/rule_RULE-029_R8_2_no_function_macros.py) |
| RULE-030 | R8.3 | Named constants only | 📝 | `grep -r "define.*[0-9]" src/ ` (find numeric defines, manual review) |
| RULE-031 | R8.4 | Conditional compilation for platform only | 🤖 | [rule_RULE-031_R8_4_conditional_compile.py](../toolchain/rules/rule_RULE-031_R8_4_conditional_compile.py) |
| RULE-032 | R9.1 | Prefer non-NULL interfaces | 🔍 | Code review - document assumptions about NULL parameters |
| RULE-033 | R9.2 | No pointer arithmetic | 🤖 | [rule_RULE-033_R9_2_no_pointer_arith.py](../toolchain/rules/rule_RULE-033_R9_2_no_pointer_arith.py) |
| RULE-034 | R9.3 | No function pointers | 🤖 | [rule_RULE-034_R9_3_no_function_pointers.py](../toolchain/rules/rule_RULE-034_R9_3_no_function_pointers.py) |
| RULE-035 | R9.4 | No multi-level pointers | 📝 | `grep -r "\*\*" src/ ` (find `**` patterns, verify exceptions like argv[]) |
| RULE-036 | R9.5 | No opaque ownership transfers | 🔍 | Design review - verify pointer ownership clearly documented in contracts |
| RULE-037 | R10.1 | Required compiler flags | 🤖 | [rule_RULE-037_R10_1_compiler_flags.py](../toolchain/rules/rule_RULE-037_R10_1_compiler_flags.py) |
| RULE-038 | R10.2 | Zero warnings policy | 🤖 | [rule_RULE-038_R10_2_zero_warnings.py](../toolchain/rules/rule_RULE-038_R10_2_zero_warnings.py) |
| RULE-039 | R10.3 | Run static analysis | 🤖 | [rule_RULE-039_R10_3_cppcheck_run.py](../toolchain/rules/rule_RULE-039_R10_3_cppcheck_run.py) |
| RULE-040 | R10.4 | Zero static analysis errors | 🤖 | [rule_RULE-040_R10_4_cppcheck_clean.py](../toolchain/rules/rule_RULE-040_R10_4_cppcheck_clean.py) |
| RULE-041 | R10.5 | MISRA C:2012 compliance (optional) | 📊 | Commercial MISRA tools (PC-lint Plus, Cppcheck Premium) |
| RULE-042 | R11.1 | Function naming | 🤖 | [rule_RULE-042_R11_1_function_naming.py](../toolchain/rules/rule_RULE-042_R11_1_function_naming.py) |
| RULE-043 | R11.2 | Variable naming | 🤖 | [rule_RULE-043_R11_2_variable_naming.py](../toolchain/rules/rule_RULE-043_R11_2_variable_naming.py) |
| RULE-044 | R11.3 | Type naming | 🤖 | [rule_RULE-044_R11_3_type_naming.py](../toolchain/rules/rule_RULE-044_R11_3_type_naming.py) |
| RULE-045 | R12.1 | Safety markers | 🤖 | [rule_RULE-045_R12_1_safety_markers.py](../toolchain/rules/rule_RULE-045_R12_1_safety_markers.py) |
| RULE-046 | R12.2 | Fail-safe defaults | 🔍 | Code review - verify safe state initialization in all structures |
| RULE-047 | R13.1 | Unit test coverage | 📊 | gcov/lcov coverage analysis - target 100% statement coverage |
| RULE-048 | R13.2 | Test naming | 🤖 | [rule_RULE-048_R13_2_test_naming.py](../toolchain/rules/rule_RULE-048_R13_2_test_naming.py) |
| RULE-049 | R14.1 | Function length limit (60 lines) | 🤖 | [rule_RULE-049_R14_1_function_length.py](../toolchain/rules/rule_RULE-049_R14_1_function_length.py) |
| RULE-050 | R14.2 | Function parameters limit (4 max) | 🤖 | [rule_RULE-050_R14_2_parameter_count.py](../toolchain/rules/rule_RULE-050_R14_2_parameter_count.py) |
| RULE-051 | R14.3 | Nesting depth limit (4 levels) | 🤖 | [rule_RULE-051_R14_3_nesting_depth.py](../toolchain/rules/rule_RULE-051_R14_3_nesting_depth.py) |
| RULE-052 | R14.4 | File length limit (800 lines) | 🤖 | [rule_RULE-052_R14_4_file_length.py](../toolchain/rules/rule_RULE-052_R14_4_file_length.py) |
| RULE-053 | R14.5 | Assertion density minimum (2 per function) | 🤖 | [rule_RULE-053_R14_5_assertion_density.py](../toolchain/rules/rule_RULE-053_R14_5_assertion_density.py) |
| RULE-054 | MISRA-1.3 | No undefined/critical unspecified behavior | 🤖 | [rule_RULE-054_MISRA_1_3_cppcheck.py](../toolchain/rules/rule_RULE-054_MISRA_1_3_cppcheck.py) |
| RULE-055 | MISRA-2.1 | No unreachable code | 🤖 | [rule_RULE-055_MISRA_2_1_unreachable.py](../toolchain/rules/rule_RULE-055_MISRA_2_1_unreachable.py) |
| RULE-056 | MISRA-2.2 | No dead code | 🤖 | [rule_RULE-056_MISRA_2_2_dead_code.py](../toolchain/rules/rule_RULE-056_MISRA_2_2_dead_code.py) |
| RULE-057 | MISRA-8.1 | Types explicitly specified | 🤖 | [rule_RULE-057_MISRA_8_1_compiler_flags.py](../toolchain/rules/rule_RULE-057_MISRA_8_1_compiler_flags.py) |
| RULE-058 | MISRA-8.3 | Consistent function declarations | 🤖 | [rule_RULE-058_MISRA_8_3_function_consistency.py](../toolchain/rules/rule_RULE-058_MISRA_8_3_function_consistency.py) |
| RULE-059 | MISRA-9.1 | Initialize automatic variables | 🤖 | [rule_RULE-059_MISRA_9_1_uninit.py](../toolchain/rules/rule_RULE-059_MISRA_9_1_uninit.py) |
| RULE-060 | MISRA-10.1 | Appropriate essential types | 🤖 | [rule_RULE-060_MISRA_10_1_types.py](../toolchain/rules/rule_RULE-060_MISRA_10_1_types.py) |
| RULE-061 | MISRA-10.3 | No narrowing assignments | 🤖 | [rule_RULE-061_MISRA_10_3_narrowing.py](../toolchain/rules/rule_RULE-061_MISRA_10_3_narrowing.py) |
| RULE-062 | MISRA-10.4 | Same essential type category | 🤖 | [rule_RULE-062_MISRA_10_4_conversion_warnings.py](../toolchain/rules/rule_RULE-062_MISRA_10_4_conversion_warnings.py) |
| RULE-063 | MISRA-11.3 | No object pointer casting | 🤖 | [rule_RULE-063_MISRA_11_3_pointer_cast.py](../toolchain/rules/rule_RULE-063_MISRA_11_3_pointer_cast.py) |
| RULE-064 | MISRA-12.1 | Explicit operator precedence | 🤖 | [rule_RULE-064_MISRA_12_1_operator_precedence.py](../toolchain/rules/rule_RULE-064_MISRA_12_1_operator_precedence.py) |
| RULE-065 | MISRA-13.2 | Single side effect per expression | 🤖 | [rule_RULE-065_MISRA_13_2_side_effects.py](../toolchain/rules/rule_RULE-065_MISRA_13_2_side_effects.py) |
| RULE-066 | MISRA-14.3 | No invariant control expressions | 📊 | cppcheck detects `if(1)`, `while(true)` patterns |
| RULE-067 | MISRA-16.3 | Switch clauses end with break | 🤖 | [rule_RULE-067_MISRA_16_3_switch_break.py](../toolchain/rules/rule_RULE-067_MISRA_16_3_switch_break.py) |
| RULE-068 | MISRA-17.7 | Use return values | 🤖 | [rule_RULE-068_MISRA_17_7_return_used.py](../toolchain/rules/rule_RULE-068_MISRA_17_7_return_used.py) |
| RULE-069 | MISRA-18.1 | Pointer arithmetic within bounds | 🤖 | [rule_RULE-069_MISRA_18_1_pointer_bounds.py](../toolchain/rules/rule_RULE-069_MISRA_18_1_pointer_bounds.py) |
| RULE-070 | MISRA-21.3 | No dynamic allocation | 🤖 | [rule_RULE-070_MISRA_21_3_no_malloc.py](../toolchain/rules/rule_RULE-070_MISRA_21_3_no_malloc.py) |
| RULE-071 | MISRA-21.6 | No standard I/O functions | 🤖 | [rule_RULE-071_MISRA_21_6_no_stdio.py](../toolchain/rules/rule_RULE-071_MISRA_21_6_no_stdio.py) |
| RULE-072 | ISO-001 | Requirement traceability | 🤖 | [rule_RULE-072_ISO_001_traceability.py](../toolchain/rules/rule_RULE-072_ISO_001_traceability.py) |
| RULE-073 | ISO-002 | Defensive programming | 🤖 | [rule_RULE-073_ISO_002_assert_density.py](../toolchain/rules/rule_RULE-073_ISO_002_assert_density.py) |
| RULE-074 | ISO-003 | Software diversity | 🔍 | Architectural review - verify independent safety checks in critical functions |
| RULE-075 | ISO-004 | Fail-safe behavior | 🔍 | Error handling review - verify all errors lead to safe state |
| RULE-076 | ISO-005 | Watchdog monitoring | 🔍 | Code review - verify watchdog refresh calls in task loop |
| RULE-077 | ISO-006 | Memory integrity checks | 🔍 | Code review - verify CRC/parity on safety-critical data structures |
| RULE-078 | ISO-007 | Plausibility checks | 🔍 | Code review - verify sensor outputs have range/rate validation |
| RULE-079 | ISO-008 | Range checking | 🔍 | Code review - verify function parameters validated at entry |
| RULE-080 | ISO-009 | State transition validity | 🔍 | State machine review - verify only legal transitions allowed |
| RULE-081 | ISO-010 | Overflow prevention | 🔍 | Code review + compiler: `-Wconversion` helps identify overflow risks

## Introduction

This document defines mandatory coding standards for the Automated Mechanical Desk Lift System. These rules are derived from NASA/JPL's "Power of Ten" rules for developing safety-critical code, adapted for our embedded C/C++ control system.

**Portability Note:** These guidelines are written to be portable and usable in both C and C++ projects. The rules focus primarily on C, and any C++-specific guidance has been removed when it does not apply cleanly to C.

### Why These Rules?

Safety-critical embedded systems require:
- **Predictability:** Code behavior must be deterministic and analyzable
- **Verifiability:** All code paths must be testable and provable
- **Reliability:** No undefined behavior or runtime failures
- **Maintainability:** Code must be understandable by all team members

### Scope

These rules apply to all C and C++ source code in:
- `src/*.c` and `src/*.cpp` - Application logic
- `src/*.h` - Public interfaces
- Motor control algorithms
- Safety-critical functions (marked with `// SAFETY-CRITICAL`)

### Enforcement

Rules are enforced through:
1. **Automated Checks:** Static analysis (cppcheck), compiler warnings (-Wall -Wextra -Werror)
2. **Code Reviews:** All code must pass peer review
3. **CI/CD Pipeline:** Automated checks on every commit
4. **Manual Verification:** Complex rules verified during code review

---

## Rule 1: Simple Control Flow

**Principle:** Restrict all code to very simple control flow constructs to ensure code is easy to analyze and verify.

### Requirements

**R1.1 - No goto Statements**
- **Rule:** Never use `goto` statements
- **Rationale:** `goto` creates unpredictable control flow and makes analysis difficult
- **Check Method:** Automated (grep, cppcheck)
- **Severity:** CRITICAL

```c
// ❌ PROHIBITED
void bad_function() {
    if (error) goto error_handler;
    // ...
    error_handler:
        cleanup();
}

// ✅ CORRECT
void good_function() {
    if (error) {
        cleanup();
        return;
    }
    // ...
}
```

**R1.2 - No setjmp/longjmp**
- **Rule:** Never use `setjmp()` or `longjmp()`
- **Rationale:** Non-local jumps break stack unwinding and RAII
- **Check Method:** Automated (grep)
- **Severity:** CRITICAL

**R1.3 - Limited Recursion**
- **Rule:** Recursion is prohibited in production code
- **Rationale:** Unbounded stack usage in embedded systems
- **Exception:** Test code may use recursion if stack depth is proven bounded
- **Check Method:** Manual review, static analysis
- **Severity:** CRITICAL

```c
// ❌ PROHIBITED - Recursion
int fibonacci_recursive(int n) {
    if (n <= 1) return n;
    return fibonacci_recursive(n-1) + fibonacci_recursive(n-2);
}

// ✅ CORRECT - Iterative
int fibonacci_iterative(int n) {
    if (n <= 1) return n;
    int prev = 0, curr = 1;
    for (int i = 2; i <= n; i++) {
        int next = prev + curr;
        prev = curr;
        curr = next;
    }
    return curr;
}
```

**R1.4 - Single Entry/Exit**
- **Rule:** Functions should have single entry and single exit point where practical
- **Rationale:** Easier to reason about function behavior and resource cleanup
- **Exception:** Early `return` for error conditions is acceptable
- **Check Method:** Code review
- **Severity:** RECOMMENDED

```c
// ✅ ACCEPTABLE - Early return for errors
ErrorCode process_command(Command cmd) {
    if (!cmd.is_valid()) {
        return ERR_INVALID_COMMAND;  // Early exit OK
    }
    
    // Main processing
    ErrorCode result = do_work(cmd);
    
    return result;  // Single exit for success path
}
```

---

## Rule 2: Fixed Loop Bounds

**Principle:** All loops must have a fixed upper bound that can be statically verified.

### Requirements

**R2.1 - Bounded Loops**
- **Rule:** Every loop must have a demonstrably bounded iteration count
- **Rationale:** Prevent infinite loops and ensure worst-case execution time (WCET) is calculable
- **Check Method:** Code review, static analysis
- **Severity:** CRITICAL

```c
// ❌ PROHIBITED - Unbounded loop
while (sensor_active()) {  // Could run forever
    read_sensor();
}

// ✅ CORRECT - Fixed iterations
const uint8_t MAX_RETRIES = 3;
for (uint8_t i = 0; i < MAX_RETRIES && sensor_active(); i++) {
    read_sensor();
}

// ✅ CORRECT - Timeout bound
const uint32_t TIMEOUT_MS = 100;
uint32_t start_time = HAL_get_time_ms();
while ((HAL_get_time_ms() - start_time) < TIMEOUT_MS && sensor_active()) {
    read_sensor();
}
```

**R2.2 - Loop Counter Limits**
- **Rule:** Loop counters must use fixed-size types with explicit upper bounds
- **Rationale:** Prevent overflow and ensure bounds are clear
- **Check Method:** Code review
- **Severity:** MANDATORY

```c
// ❌ POOR - Implicit bound
for (int i = 0; i < array_size; i++) {  // 'int' size is platform-dependent
    process(array[i]);
}

// ✅ CORRECT - Explicit bound
const uint8_t ARRAY_SIZE = 10;
for (uint8_t i = 0; i < ARRAY_SIZE; i++) {
    process(array[i]);
}
```

**R2.3 - No while(true)**
- **Rule:** `while(true)` or `for(;;)` loops prohibited except in main task loop
- **Rationale:** Infinite loops acceptable only at top-level task structure
- **Exception:** Main control loop in `DeskApp_task()` or interrupt handlers
- **Check Method:** Automated (grep)
- **Severity:** CRITICAL

```c
// ✅ ACCEPTABLE - Main task loop only
void DeskApp_task() {
    while (true) {  // Top-level control loop OK
        read_inputs();
        update_state();
        control_outputs();
        delay(TASK_PERIOD_MS);
    }
}

// ❌ PROHIBITED - Infinite loop in function
void wait_for_sensor() {
    while (true) {  // Never do this
        if (sensor_ready()) break;
    }
}

// ✅ CORRECT - Bounded wait
bool wait_for_sensor_timeout(uint32_t timeout_ms) {
    uint32_t start = HAL_get_time_ms();
    while ((HAL_get_time_ms() - start) < timeout_ms) {
        if (sensor_ready()) return true;
    }
    return false;  // Timeout
}
```

**R2.4 - Loop Invariants**
- **Rule:** Complex loops must document their invariants
- **Rationale:** Helps verify correctness and aids maintenance
- **Check Method:** Code review
- **Severity:** RECOMMENDED

```c
// ✅ GOOD - Documented invariant
// Loop invariant: 0 <= i < BUFFER_SIZE and sum contains sum of buffer[0..i-1]
uint32_t sum = 0;
for (uint8_t i = 0; i < BUFFER_SIZE; i++) {
    sum += buffer[i];
}
```

---

## Rule 3: No Dynamic Memory Allocation

**Principle:** Do not use dynamic memory allocation (heap) after system initialization.

### Requirements

**R3.1 - No malloc/free**
- **Rule:** Prohibited: `malloc()`, `calloc()`, `realloc()`, `free()`
- **Rationale:** Fragmentation, non-deterministic timing, allocation failures
- **Check Method:** Automated (grep, static analysis)
- **Severity:** CRITICAL

**R3.2 - Avoid Hidden Dynamic Allocation**
- **Rule:** Do not use libraries or APIs that allocate memory implicitly at runtime
- **Rationale:** Hidden allocation undermines predictability and timing
- **Check Method:** Code review, static analysis
- **Severity:** MANDATORY

**R3.4 - Stack Size Limits**
- **Rule:** Stack usage per function must not exceed 512 bytes
- **Rationale:** Limited stack space in embedded systems (typically 2-8 KB total)
- **Check Method:** Static analysis, manual review
- **Severity:** MANDATORY

```c
// ❌ PROHIBITED - Excessive stack usage
void bad_function() {
    uint8_t large_buffer[2048];  // 2KB on stack - too large
    process(large_buffer);
}

// ✅ CORRECT - Static storage for large buffers
static uint8_t large_buffer[2048];  // Stored in .bss, not stack
void good_function() {
    process(large_buffer);
}
```

---

## Rule 4: Short Functions

**Principle:** Limit function length to a single printed page (60 lines of code).

### Requirements

**R4.1 - Function Length Limit**
- **Rule:** Functions must not exceed 60 lines of code (excluding comments and blank lines)
- **Rationale:** Short functions are easier to understand, test, and verify
- **Check Method:** Automated (line counting script)
- **Severity:** MANDATORY

```c
// ❌ POOR - Function too long (>60 lines)
// Split into smaller functions

// ✅ CORRECT - Functions under 60 lines
void update_motor_state() {  // ~20 lines
    read_sensors();
    calculate_control();
    apply_outputs();
}

void read_sensors() {  // ~15 lines
    button_state = read_buttons();
    limit_state = read_limits();
    current = read_motor_current();
}
```

**R4.2 - Single Responsibility**
- **Rule:** Each function should do one thing and do it well
- **Rationale:** Supports modularity and testability
- **Check Method:** Code review
- **Severity:** RECOMMENDED

**R4.3 - Function Complexity**
- **Rule:** Cyclomatic complexity must not exceed 10
- **Rationale:** High complexity indicates need for decomposition
- **Check Method:** Static analysis (cppcheck --enable=all)
- **Severity:** MANDATORY

---

## Rule 5: Defensive Assertions

**Principle:** Use assertions liberally to check assumptions and preconditions.

### Requirements

**R5.1 - Assertion Density**
- **Rule:** Use minimum 2 assertions per function on average
- **Rationale:** Defensive programming catches errors early
- **Check Method:** Manual review, automated counting
- **Severity:** RECOMMENDED

**R5.2 - Precondition Checks**
- **Rule:** All public functions must validate input parameters
- **Rationale:** Protect against invalid inputs
- **Check Method:** Code review
- **Severity:** MANDATORY

```c
// ✅ CORRECT - Parameter validation
void MotorController_set_direction(MotorDirection dir) {
    // Precondition: direction must be valid
    assert(dir == DIR_FORWARD || dir == DIR_REVERSE || dir == DIR_STOP);
    
    // Precondition: motor must be initialized
    assert(motor_initialized);
    
    // Function implementation
    current_direction = dir;
}
```

**R5.3 - Invariant Checks**
- **Rule:** Safety-critical functions must assert invariants
- **Rationale:** Detect corruption and violated assumptions
- **Check Method:** Code review
- **Severity:** CRITICAL (for safety-critical code)

```c
// SAFETY-CRITICAL
void DeskApp_update_state(DeskState* state) {
    // Invariant: state pointer must be valid
    assert(state != NULL);
    
    // Invariant: state must be in valid range
    assert(state->current >= STATE_IDLE && state->current <= STATE_ERROR);
    
    // Function implementation
    // ...
    
    // Post-condition: new state must be valid
    assert(state->current >= STATE_IDLE && state->current <= STATE_ERROR);
}
```

**R5.4 - Assertions in Release Builds**
- **Rule:** Safety-critical assertions must remain active in release builds
- **Rationale:** Safety checks should never be disabled
- **Implementation:** Use custom `SAFETY_ASSERT()` macro
- **Check Method:** Code review
- **Severity:** CRITICAL

```c
// ✅ CORRECT - Safety assertions always active
#define SAFETY_ASSERT(condition) \
    do { \
        if (!(condition)) { \
            HAL_enter_safe_state(); \
            while(1); /* Halt system */ \
        } \
    } while(0)

// SAFETY-CRITICAL
void apply_motor_power(uint8_t pwm) {
    SAFETY_ASSERT(pwm <= MAX_PWM_VALUE);  // Never disabled
    HAL_set_motor_pwm(pwm);
}
```

**R5.5 - No Side Effects in Assertions**
- **Rule:** Assertion conditions must not have side effects
- **Rationale:** If assertions are disabled, behavior must not change
- **Check Method:** Code review
- **Severity:** MANDATORY

```c
// ❌ PROHIBITED - Side effect in assertion
assert(read_sensor() > 0);  // Reads sensor as side effect

// ✅ CORRECT - No side effect
int sensor_value = read_sensor();
assert(sensor_value > 0);
```

---

## Rule 6: Minimal Variable Scope

**Principle:** Declare data objects at the smallest possible scope.

### Requirements

**R6.1 - Local Scope Preferred**
- **Rule:** Declare variables in the innermost scope where they are needed
- **Rationale:** Reduces coupling, prevents unintended access
- **Check Method:** Code review
- **Severity:** MANDATORY

```c
// ❌ POOR - Unnecessarily broad scope
void process_data() {
    int i, j, k;  // Declared at top
    uint8_t temp;
    
    for (i = 0; i < 10; i++) {
        // ...
    }
    
    for (j = 0; j < 5; j++) {  // j only used here
        // ...
    }
}

// ✅ CORRECT - Minimal scope
void process_data() {
    for (int i = 0; i < 10; i++) {  // i scoped to loop
        // ...
    }
    
    for (int j = 0; j < 5; j++) {  // j scoped to loop
        // ...
    }
}
```

**R6.2 - Minimize Global Variables**
- **Rule:** Global variables must be justified and documented
- **Rationale:** Globals create hidden coupling and complicate testing
- **Check Method:** Manual review
- **Severity:** MANDATORY

```c
// ❌ AVOID - Global state
int motor_pwm;  // Global, accessed from anywhere

// ✅ BETTER - Encapsulated in a module
static uint8_t motor_pwm = 0;

uint8_t MotorController_get_pwm(void) { return motor_pwm; }
void MotorController_set_pwm(uint8_t pwm) { motor_pwm = pwm; }
```

**R6.3 - Static Variables Documentation**
- **Rule:** Static variables must have comment explaining why static
- **Rationale:** Persistent state should be explicit and justified
- **Check Method:** Code review
- **Severity:** RECOMMENDED

```c
// ✅ CORRECT - Documented static
void count_calls() {
    // Static used to maintain call count across invocations
    // for diagnostic purposes only - not safety-critical
    static uint32_t call_count = 0;
    call_count++;
}
```

**R6.4 - Initialization at Declaration**
- **Rule:** Variables should be initialized at point of declaration
- **Rationale:** Prevents use of uninitialized variables
- **Check Method:** Compiler warnings (-Wuninitialized)
- **Severity:** MANDATORY

```c
// ❌ POOR - Uninitialized
uint8_t status;
// ... many lines of code
status = read_status();  // Could forget to initialize

// ✅ CORRECT - Initialize at declaration
uint8_t status = read_status();
```

---

## Rule 7: Check Return Values

**Principle:** Check the return value of all non-void functions, and check validity of parameters.

### Requirements

**R7.1 - Check All Return Values**
- **Rule:** Return value of every non-void function must be checked or explicitly ignored
- **Rationale:** Errors must not be silently ignored
- **Check Method:** Compiler warnings (-Wunused-result), code review
- **Severity:** CRITICAL

```c
// ❌ PROHIBITED - Ignored return value
read_sensor();  // Returns error code, but ignored

// ✅ CORRECT - Check return value
ErrorCode err = read_sensor();
if (err != ERR_OK) {
    handle_error(err);
}

// ✅ ACCEPTABLE - Explicitly void cast if truly don't care
(void)diagnostic_function();  // Explicitly ignored
```

**R7.2 - Parameter Validation**
- **Rule:** All function parameters must be validated
- **Rationale:** Protect against invalid inputs
- **Check Method:** Code review
- **Severity:** MANDATORY

```c
// ✅ CORRECT - Validate all parameters
ErrorCode set_motor_speed(uint8_t speed_percent) {
    // Validate parameter range
    if (speed_percent > 100) {
        return ERR_INVALID_PARAMETER;
    }
    
    // Parameter is valid, proceed
    motor_pwm = (speed_percent * MAX_PWM) / 100;
    return ERR_OK;
}
```

**R7.3 - Pointer Parameter Checks**
- **Rule:** All pointer parameters must be checked for NULL
- **Rationale:** Dereferencing NULL causes crash
- **Check Method:** Code review, static analysis
- **Severity:** CRITICAL

```c
// ✅ CORRECT - NULL pointer check
void process_buffer(const uint8_t* buffer, uint8_t length) {
    if (buffer == NULL) {
        return;  // Or handle error appropriately
    }
    
    if (length == 0) {
        return;  // Validate all parameters
    }
    
    // Safe to use buffer
    for (uint8_t i = 0; i < length; i++) {
        process_byte(buffer[i]);
    }
}
```

**R7.4 - Error Propagation**
- **Rule:** Errors must be propagated to caller or handled locally
- **Rationale:** No error should be lost
- **Check Method:** Code review
- **Severity:** MANDATORY

```c
// ✅ CORRECT - Error propagation
ErrorCode read_and_process() {
    ErrorCode err = read_sensor();
    if (err != ERR_OK) {
        return err;  // Propagate error to caller
    }
    
    err = process_data();
    if (err != ERR_OK) {
        return err;  // Propagate error
    }
    
    return ERR_OK;
}
```

---

## Rule 8: Limited Preprocessor Use

**Principle:** Limit preprocessor use to file inclusion and simple conditional compilation.

### Requirements

**R8.1 - Include Guards Only**
- **Rule:** Prefer `#pragma once` or include guards over complex preprocessor logic
- **Rationale:** Simplicity and portability
- **Check Method:** Code review
- **Severity:** MANDATORY

```c
// ✅ CORRECT - Include guard
#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

// Header content

#endif  // MOTOR_CONTROLLER_H

// ✅ ALSO CORRECT - Pragma once (simpler)
#pragma once

// Header content
```

**R8.2 - No Function-Like Macros**
- **Rule:** Prohibited: Function-like macros (except assertions)
- **Rationale:** Macros don't respect scope, type safety, or debugger
- **Exception:** Assertions and compile-time constants
- **Check Method:** Code review
- **Severity:** MANDATORY

```c
// ❌ PROHIBITED - Function-like macro
#define MAX(a, b) ((a) > (b) ? (a) : (b))  // Type-unsafe, evaluates args twice

// ✅ CORRECT - Inline function (C99+)
static inline int max_int(int a, int b) {
    return (a > b) ? a : b;
}
```

**R8.3 - Named Constants Only**
- **Rule:** Macros for constants only, with clear names
- **Rationale:** Constants are acceptable use of preprocessor
- **Check Method:** Code review
- **Severity:** RECOMMENDED

```c
// ✅ ACCEPTABLE - Named constants
#define MAX_BUFFER_SIZE 256
#define TASK_PERIOD_MS  10

// ✅ BETTER - Const variables
static const uint8_t MAX_BUFFER_SIZE = 256;
static const uint8_t TASK_PERIOD_MS = 10;
```

**R8.4 - Conditional Compilation**
- **Rule:** Conditional compilation only for platform/configuration
- **Rationale:** Minimize #ifdef complexity
- **Check Method:** Code review
- **Severity:** RECOMMENDED

```c
// ✅ ACCEPTABLE - Platform-specific
#ifdef TESTENVIRONMENT
    #include "HALMock.h"
#else
    #include <Arduino.h>
#endif

// ❌ AVOID - Logic in preprocessor
#ifdef FEATURE_X
    void do_x() { /* ... */ }
#else
    void do_x() { /* different impl */ }
#endif

// ✅ BETTER - Runtime logic
void do_x() {
    if (feature_x_enabled) {
        // Feature X implementation
    } else {
        // Alternative implementation
    }
}
```

---

## Rule 9: Restricted Pointer Use

**Principle:** Limit pointer use to minimize risk of errors.

### Requirements

**R9.1 - Prefer Non-NULL Interfaces Where Available**
- **Rule:** Prefer APIs that cannot accept NULL when the language supports it
- **Rationale:** Reduces null-dereference risk
- **Check Method:** Code review
- **Severity:** RECOMMENDED
- **Applicability:** C and C++

**R9.2 - No Pointer Arithmetic**
- **Rule:** Pointer arithmetic prohibited except for array indexing
- **Rationale:** Easy to make off-by-one errors
- **Exception:** Array traversal with bounds checking
- **Check Method:** Code review
- **Severity:** MANDATORY

```c
// ❌ PROHIBITED - Pointer arithmetic
uint8_t* p = buffer;
for (int i = 0; i < size; i++) {
    *(p + i) = 0;  // Pointer arithmetic
}

// ✅ CORRECT - Array indexing
for (int i = 0; i < size; i++) {
    buffer[i] = 0;  // Clear and safe
}
```

**R9.3 - No Function Pointers**
- **Rule:** Function pointers prohibited in production code
- **Rationale:** Makes static analysis difficult
- **Exception:** Test mocking frameworks may use function pointers
- **Check Method:** Code review, grep
- **Severity:** MANDATORY

```c
// ❌ PROHIBITED - Function pointer
typedef void (*callback_t)(int);
void register_callback(callback_t cb);

// ✅ CORRECT - Direct function call
void handle_event(int value) {
    // Handle event directly
    (void)value;
}
```

**R9.4 - No Multi-Level Pointers**
- **Rule:** Pointer-to-pointer prohibited
- **Rationale:** Complexity and error-prone
- **Exception:** C string arrays (`char* argv[]`)
- **Check Method:** Code review
- **Severity:** MANDATORY

```c
// ❌ PROHIBITED - Multi-level pointer
void modify_pointer(int** p) {
    *p = NULL;  // Multi-level complexity and confusion
}

int value = 10;
int* ptr = &value;
modify_pointer(&ptr);  // Hard to reason about

// ✅ CORRECT - Use return value
int* create_pointer(int* initial_value) {
    if (initial_value == NULL) {
        return NULL;
    }
    return initial_value;
}

int value = 10;
int* ptr = create_pointer(&value);  // Clear and simple

// ✅ ALSO CORRECT - Use struct wrapper
typedef struct {
    int* ptr;
} PointerHandle;

PointerHandle create_handle(int* initial_value) {
    PointerHandle handle;
    handle.ptr = initial_value;
    return handle;
}

int value = 10;
PointerHandle handle = create_handle(&value);  // Encapsulated
```

**R9.5 - No Opaque Ownership Transfers**
- **Rule:** Avoid ownership transfer patterns that obscure lifetime
- **Rationale:** Hidden ownership increases risk of leaks and use-after-free
- **Check Method:** Code review
- **Severity:** RECOMMENDED

---

## Rule 10: Aggressive Compiler Warnings and Static Analysis

**Principle:** Compile with all warnings enabled and use static analysis tools.

### Requirements

**R10.1 - Compiler Warning Flags**
- **Rule:** Must compile with `-Wall -Wextra -Werror -pedantic`
- **Rationale:** Compiler catches many common errors
- **Check Method:** Build system configuration
- **Severity:** CRITICAL

```cmake
# CMakeLists.txt - MANDATORY compiler flags
add_compile_options(
    -Wall          # Enable all warnings
    -Wextra        # Enable extra warnings
    -Werror        # Treat warnings as errors
    -pedantic      # Strict ISO C/C++ compliance
    -Wconversion   # Warn on implicit conversions
    -Wshadow       # Warn on variable shadowing
)
```

**R10.2 - Zero Warnings Policy**
- **Rule:** Code must compile with zero warnings
- **Rationale:** Warnings indicate potential bugs
- **Exception:** None - fix or suppress with justification
- **Check Method:** CI/CD pipeline
- **Severity:** CRITICAL

```c
// ❌ PROHIBITED - Unused variable warning
void process_value(int value) {
    int result = calculate(value);  // Assigned but never used
}

// ✅ CORRECT - Use the value or explicitly discard
void process_value(int value) {
    int result = calculate(value);
    apply_result(result);
}

// ✅ ALSO CORRECT - Explicitly discard if unused by design
void process_value(int value) {
    (void)value;  // Intentionally unused
}

// ❌ PROHIBITED - Uninitialized variable
void calculate() {
    int total;
    total = total + 5;  // Used before initialization
}

// ✅ CORRECT - Initialize all variables
void calculate() {
    int total = 0;
    total = total + 5;  // Safe
}

// ❌ PROHIBITED - Implicit narrowing conversion
void set_speed(float percent) {
    uint8_t pwm = percent * 255;  // Implicit double to int conversion
}

// ✅ CORRECT - Explicit cast documents intent
void set_speed(float percent) {
    uint8_t pwm = (uint8_t)(percent * 255);  // Explicit cast
}

// ❌ PROHIBITED - Shadowing
void outer() {
    int counter = 0;
    {
        int counter = 1;  // Shadows outer counter - confusing!
    }
}

// ✅ CORRECT - Use different names
void outer() {
    int total_counter = 0;
    {
        int loop_counter = 1;  // Clear and distinct name
    }
}
```

**R10.3 - Static Analysis**
- **Rule:** Run cppcheck on every commit
- **Rationale:** Catches errors beyond compiler capabilities
- **Check Method:** Automated in pipeline
- **Severity:** MANDATORY

```bash
# Required static analysis command
# Use --std=c++17 for C++ projects
cppcheck --enable=all \
         --error-exitcode=1 \
         --suppress=missingIncludeSystem \
         --std=c11 \
         src/
```

```c
// ❌ CAUGHT BY STATIC ANALYSIS - Memory leak
void load_config(void) {
    char* buffer = (char*)malloc(1024);
    if (buffer == NULL) {
        return;  // Leak! Allocated memory never freed
    }
    // ...
    process_buffer(buffer);
    // Returns without free()
}

// ✅ CORRECT - Proper resource cleanup
void load_config(void) {
    char* buffer = (char*)malloc(1024);
    if (buffer == NULL) {
        return ERR_NO_MEMORY;
    }
    
    ErrorCode result = process_buffer(buffer);
    free(buffer);  // Always cleanup
    return result;
}

// ❌ CAUGHT BY STATIC ANALYSIS - Dead code
int validate_range(int value) {
    if (value < 0) {
        return ERR_NEGATIVE;
    }
    return ERR_OK;
    printf("Validation complete\n");  // Unreachable code
}

// ✅ CORRECT - No code after return
int validate_range(int value) {
    if (value < 0) {
        return ERR_NEGATIVE;
    }
    // All reachable code before final return
    return ERR_OK;
}

// ❌ CAUGHT BY STATIC ANALYSIS - Unchecked function result
void read_sensor(void) {
    int value;
    HAL_read_adc(&value);  // Non-zero return means error!
    process_value(value);  // Processing with possibly invalid value
}

// ✅ CORRECT - Always check return values
void read_sensor(void) {
    int value;
    ErrorCode err = HAL_read_adc(&value);
    if (err != ERR_OK) {
        return err;  // Handle error
    }
    process_value(value);  // Safe to process
}
```

**R10.4 - Static Analysis Results**
- **Rule:** Zero errors, zero warnings from static analysis
- **Rationale:** Each issue is a potential defect
- **Exception:** False positives must be documented and suppressed inline
- **Check Method:** CI/CD pipeline
- **Severity:** MANDATORY

```c
// ✅ CORRECT - Inline suppression with justification
void example() {
    // cppcheck-suppress unusedVariable
    // Variable used in assertion, may appear unused in release build
    int debug_counter = get_counter();
    assert(debug_counter >= 0);
}
```

**R10.5 - MISRA C:2012 Compliance (Optional)**
- **Rule:** Strive for MISRA C:2012 compliance where practical
- **Rationale:** Industry standard for safety-critical C
- **Exception:** Some MISRA rules may not map cleanly to all C++ usage
- **Check Method:** MISRA checker tools
- **Severity:** RECOMMENDED

---

## Project-Specific Guidelines

### Naming Conventions

**R11.1 - Function Names**
- **Rule:** `Module_functionName()` - PascalCase module, camelCase function
- **Example:** `MotorController_set_direction()`, `HAL_read_button()`
- **Rationale:** Consistent with project codebase
- **Severity:** MANDATORY

```c
// ❌ PROHIBITED - Inconsistent naming
void setMotorPwm() { }           // No module prefix
void motor_controller_update() { } // Module not PascalCase
void MC_SetPWM() { }             // Function not camelCase

// ✅ CORRECT - Consistent naming
void MotorController_set_pwm() { }
void HAL_read_sensor() { }
void DeskApp_update_state() { }
```

**R11.2 - Variable Names**
- **Rule:** `snake_case` for local variables, `SCREAMING_SNAKE_CASE` for constants
- **Example:** `button_state`, `MAX_PWM_VALUE`
- **Severity:** MANDATORY

```c
// ❌ PROHIBITED - Inconsistent naming
int ButtonState;          // PascalCase for local variable
const int max_pwm = 255; // lowercase for constant

// ✅ CORRECT - Consistent naming
int button_state;           // snake_case for variables
const int MAX_PWM = 255;    // SCREAMING_SNAKE_CASE for constants
static int motor_pwm = 0;   // snake_case for static
```

**R11.3 - Type Names**
- **Rule:** `CamelCase` for classes/structs, `snake_case_t` for typedefs
- **Example:** `MotorController`, `error_code_t`
- **Severity:** MANDATORY

```c
// ❌ PROHIBITED - Inconsistent type naming
struct motor_controller { };  // Should be CamelCase
typedef int ErrorCode;        // Should have _t suffix

// ✅ CORRECT - Consistent type naming
struct MotorController {
    uint8_t pwm;
    MotorDirection direction;
};

typedef enum {
    ERR_OK = 0,
    ERR_INVALID_PARAM,
    ERR_TIMEOUT
} error_code_t;
```

### Safety-Critical Code

**R12.1 - Safety Markers**
- **Rule:** Mark safety-critical sections with `// SAFETY-CRITICAL` comment
- **Example:** See motor control, limit checking, error handling
- **Rationale:** Easy identification for review and analysis
- **Severity:** MANDATORY

**R12.2 - Fail-Safe Defaults**
- **Rule:** All safety states must default to safe condition
- **Example:** Motor stopped, power off, brakes engaged
- **Severity:** CRITICAL

```c
// ✅ CORRECT - Fail-safe initialization
typedef struct {
    MotorDirection direction;
    uint8_t pwm;
    bool enabled;
} MotorState;

static MotorState MotorState_init(void) {
    MotorState state;
    state.direction = DIR_STOP;  // Safe default
    state.pwm = 0;               // Safe default (off)
    state.enabled = false;       // Safe default (disabled)
    return state;
}
```

### Testing Requirements

**R13.1 - Unit Test Coverage**
- **Rule:** All functions must have unit tests
- **Target:** 100% statement coverage
- **Check Method:** Coverage tools (gcov/lcov)
- **Severity:** MANDATORY

**R13.2 - Test Naming**
- **Rule:** Tests named `TC_<Module>_<Requirement>_<Description>`
- **Example:** `TC_SWReq001_001_MotorStopsOnButtonRelease`
- **Severity:** MANDATORY

---

## Rule 14: Code Quality Metrics

**Principle:** Enforce measurable code quality thresholds to prevent complexity creep and technical debt.

### Requirements

**R14.1 - Limited Function Length**
- **Rule:** Functions must not exceed 60 lines of executable code (excluding comments/whitespace)
- **Threshold:** ⚠️ Yellow flag 40+ lines, 🔴 Red flag 60+ lines
- **Rationale:** 
  - Functions >60 lines often violate single responsibility principle
  - NASA Power of Ten baseline; proven for safety-critical systems
  - Fits on single screen → easier code review and testing
  - Correlates with cyclomatic complexity ~10
- **Check Method:** Automated line count analysis per function
- **Severity:** MANDATORY
- **Automation:** `rule_RULE-069_R14_1_function_length.py`

```c
// ❌ PROHIBITED - Function too long (>60 lines)
void process_sensor_data_complex(uint16_t* data, uint16_t size) {
    // Line 1: Validation
    if (data == NULL) return;
    if (size == 0) return;
    
    // Lines 5-25: Complex processing block
    for (int i = 0; i < size; i++) {
        if (data[i] > 1000) {
            // ... 15+ lines of nested logic ...
        }
    }
    
    // Lines 26-60: More processing
    for (int i = 0; i < size; i++) {
        if (data[i] < 100) {
            // ... 15+ lines more ...
        }
    }
    // Total: 60+ lines! This should be split.
}

// ✅ CORRECT - Refactored into smaller functions
void process_sensor_data(uint16_t* data, uint16_t size) {
    if (!validate_sensor_data(data, size)) {
        return;
    }
    
    filter_high_values(data, size);
    filter_low_values(data, size);
}

void filter_high_values(uint16_t* data, uint16_t size) {
    // Single responsibility: handle values > 1000
    for (int i = 0; i < size; i++) {
        if (data[i] > 1000) {
            data[i] = 1000;
        }
    }
}

void filter_low_values(uint16_t* data, uint16_t size) {
    // Single responsibility: handle values < 100
    for (int i = 0; i < size; i++) {
        if (data[i] < 100) {
            data[i] = 0;
        }
    }
}
```

**Benefits:**
- ✅ Forces single responsibility principle adherence
- ✅ Easier unit testing (fewer code paths to cover)
- ✅ Reduces bug density (proven linear correlation)
- ✅ Improves code review effectiveness
- ✅ Better maintainability (context stays in memory)

**Industry Precedent:** NASA JPL Power of Ten (Rule 4), Google C++ Style Guide, Linux kernel standards

---

**R14.2 - Limited Function Parameters**
- **Rule:** Functions must not exceed 4 parameters
- **Threshold:** ⚠️ Yellow at 4 params, 🔴 Red at 5+ params
- **Rationale:**
  - 4 parameters is cognitive limit for most developers
  - More parameters = harder to test and understand
  - Forces better data encapsulation
  - Improves API clarity
- **Check Method:** Parameter count analysis per function
- **Severity:** MANDATORY
- **Automation:** `rule_RULE-070_R14_2_parameter_count.py`

```c
// ❌ PROHIBITED - Too many parameters
void set_motor_config(uint8_t pwm, enum Direction dir, bool enabled, 
                      uint16_t timeout, int acceleration, float curve) {
    // 6 parameters! Unclear caller intent, hard to test
    // ...
}

// ✅ CORRECT - Encapsulate in struct
typedef struct {
    uint8_t pwm;
    enum Direction direction;
    bool enabled;
    uint16_t timeout;
    int acceleration;
    float curve;
} MotorConfig;

void set_motor_config(const MotorConfig* config) {
    // Single parameter! Clear intent, testable
    assert(config != NULL);
    // ...
}

// Caller side is CLEARER now:
MotorConfig cfg = {
    .pwm = 128,
    .direction = DIR_UP,
    .enabled = true,
    .timeout = 1000,
    .acceleration = 50,
    .curve = 1.5f
};
set_motor_config(&cfg);  // Much clearer than 6 args!
```

**Benefits:**
- ✅ Simpler unit test setup (fewer mock parameters)
- ✅ Clearer function contracts
- ✅ Easier to extend without breaking API
- ✅ Forces better data structure design

---

**R14.3 - Limited Nesting Depth**
- **Rule:** Block nesting must not exceed 4 levels
- **Threshold:** ⚠️ Yellow at 4 levels, 🔴 Red at 5+ levels
- **Rationale:**
  - Each nesting level increases cognitive load exponentially
  - Most developers reliably understand only 3-4 levels
  - Correlates strongly with error-prone code
  - Improves readability dramatically
- **Check Method:** Nesting depth analysis per function
- **Severity:** MANDATORY
- **Automation:** `rule_RULE-071_R14_3_nesting_depth.py`

```c
// ❌ PROHIBITED - 5+ levels of nesting
void process_depth_problem(uint8_t* data, uint16_t size) {
    if (data != NULL) {                       // Level 1
        for (int i = 0; i < size; i++) {    // Level 2
            if (data[i] > THRESHOLD) {      // Level 3
                while (data[i] < MAX) {     // Level 4
                    if (check_valid()) {    // Level 5 ❌ TOO DEEP!
                        process();
                    }
                }
            }
        }
    }
}

// ✅ CORRECT - Refactored for clarity (4 levels max)
void process_depth_correct(uint8_t* data, uint16_t size) {
    if (data == NULL) {
        return;  // Guard clause - early exit
    }
    
    for (int i = 0; i < size; i++) {        // Level 1
        if (data[i] > THRESHOLD) {          // Level 2
            process_high_value(&data[i]);   // Extracted function
        }
    }
}

// Helper function keeps nesting low
static void process_high_value(uint8_t* value) {
    if (value == NULL) return;
    
    while (*value < MAX) {                  // Level 1 (in this context)
        if (check_valid()) {                // Level 2
            process();
        }
    }
}

// Better yet: use guard clauses to eliminate nesting
static void process_high_value_simple(uint8_t* value) {
    if (value == NULL) return;
    if (*value >= MAX) return;
    if (!check_valid()) return;
    
    process();  // No nesting! Crystal clear.
}
```

**Benefits:**
- ✅ Dramatically improves code readability
- ✅ Reduces cognitive load on reviewers
- ✅ Decreases error probability
- ✅ Encourages extraction of helper functions

---

**R14.4 - Limited File Length**
- **Rule:** Source files must not exceed 800 lines of code
- **Threshold:** ⚠️ Yellow at 600 lines, 🔴 Red at 800+ lines
- **Rationale:**
  - Large files often contain multiple responsibilities
  - 800+ lines indicates need for module splitting
  - Mirrors function length rule but at module level
  - Improves compilation time and coupling
- **Check Method:** Line count analysis per file
- **Severity:** MANDATORY
- **Automation:** `rule_RULE-072_R14_4_file_length.py`

```c
// ❌ PROHIBITED - Motor controller file too long (800+ lines)
// file: motor_controller.cpp
// Contains: initialization, control, monitoring, diagnostics, testing
// Total: 950 lines - should be split

// ✅ CORRECT - Split by responsibility
// File 1: motor_controller.cpp (280 lines)
// - Core motor control API only
// - set_speed(), get_status(), stop()

// File 2: motor_diagnostics.cpp (320 lines)
// - Fault detection and reporting
// - check_current_limit(), get_temperature()

// File 3: motor_calibration.cpp (240 lines)
// - Startup calibration and safety checks
// - calibrate_min(), calibrate_max(), verify_safety()
```

**Benefits:**
- ✅ Enforces single responsibility at module level
- ✅ Easier to navigate large codebases
- ✅ Reduces recompilation impact
- ✅ Increases code reusability and modularity
- ✅ Improves test isolation

---

**R14.5 - Minimum Assertion Density**
- **Rule:** Functions must contain minimum 2 assertions on average
- **Threshold:** 🟡 Warning if < 2 assertions per function (average across module)
- **Rationale:**
  - Defensive programming requires invariant checking
  - Critical for ISO 25119 safety requirements
  - Catches assumptions that may be invalid
  - Low assertion density = low defensive rigor
- **Check Method:** Assertion count per function analysis
- **Severity:** MANDATORY (for safety functions), RECOMMENDED (general)
- **Automation:** `rule_RULE-073_R14_5_assertion_density.py`

```c
// ❌ PROHIBITED - Insufficient assertions (only 1 per 10-line function)
uint8_t calculate_duty_cycle_unsafe(uint16_t target_rpm) {
    assert(target_rpm <= MAX_RPM);  // Only 1 assertion!
    
    // What if MAX_RPM is 0? What if multiplication overflows?
    // What if final value >255?
    uint16_t scaled = (target_rpm * 255) / MAX_RPM;
    return (uint8_t)scaled;  // Could truncate or overflow!
}

// ✅ CORRECT - Comprehensive assertions document assumptions
uint8_t calculate_duty_cycle_safe(uint16_t target_rpm) {
    // Input validation
    assert(target_rpm <= MAX_RPM);
    assert(MAX_RPM > 0);
    
    // Calculation with sanity checks
    uint16_t scaled = (target_rpm * 255) / MAX_RPM;
    
    // Output validation
    assert(scaled <= 255);
    
    return (uint8_t)scaled;
}

// Safety-critical function example
bool is_desk_position_safe(int16_t position_mm) {
    // Preconditions
    assert(position_mm >= MIN_DESK_HEIGHT_MM);
    assert(position_mm <= MAX_DESK_HEIGHT_MM);
    assert(motor_initialized);
    
    // Check limit switches (independent verification)
    bool limit_safe = !limit_switch_pressed();
    assert(!limit_safe || limit_switch_pressed());  // Assumption check
    
    // Plausibility check
    bool position_plausible = (position_mm < MAX_LIMIT_POSITION);
    assert(position_plausible);
    
    // Postcondition
    assert(limit_safe == !limit_switch_pressed());
    
    return limit_safe && position_plausible;
}
```

**Assertion Best Practices:**
- ✅ Place assertions at function entry (preconditions)
- ✅ Place assertions before critical operations (invariants)
- ✅ Place assertions at function exit (postconditions)
- ✅ Use `assert()` for invariants that MUST be true
- ✅ Use runtime `if` checks for potentially invalid user input
- ✅ Document why each assertion matters

**Benefits:**
- ✅ Documents implicit assumptions explicitly
- ✅ Catches edge cases during testing and in field
- ✅ Improves debugging (assertion identifies exact failure)
- ✅ Critical for ISO 25119 compliance
- ✅ Aligns with NASA R5 (defensive assertions)

**Formula:** Ideal Density = 2-3 assertions per function

Example:
```
10-line function → 2+ assertions ideal
30-line function → 5-7 assertions ideal  
60-line function → 8-10 assertions (but should be shorter!)
```

---

## MISRA C:2012 Compliance Rules

### Overview

MISRA C:2012 "Guidelines for the use of the C language in critical systems" defines 143 rules across 22 categories. This section covers the most critical rules for embedded safety applications, extracted from MISRA C:2012.

**Rule Categories:** Required (SHALL), Mandatory (SHALL), Advisory (SHOULD)

### Core MISRA C Rules for Safety-Critical Systems

**MISRA C Rule 1.3: No Undefined or Critical Unspecified Behaviour (Required)**
- **Category:** Required (SHALL)
- **Rule:** There shall be no occurrence of undefined or critical unspecified behaviour
- **Rationale:** Undefined behavior makes programs non-portable and unpredictable
- **Standard Refs:** C90 Undefined behaviors, C99 Undefined behaviors
- **Check:** Static analysis (cppcheck), compiler warnings
- **Severity:** CRITICAL
- **Automation:** `cppcheck --enable=all`, GCC `-Wundef`

```c
// ❌ PROHIBITED - Undefined behavior
int overflow = INT_MAX + 1;  // Signed overflow - undefined!
int shift = 1 << 32;  // Shift >= width - undefined!

// ✅ CORRECT - Well-defined behavior
if (value < INT_MAX) {
    value++;
}
```

**MISRA C Rule 2.1: No Unreachable Code (Required)**
- **Category:** Required (SHALL)
- **Rule:** A project shall not contain unreachable code
- **Rationale:** Unreachable code indicates logic errors and wastes resources
- **Standards:** IEC 61508-7 C.5.9, DO-178C 6.4.4.3.c
- **Check:** Static analysis, coverage analysis
- **Severity:** MANDATORY
- **Automation:** `cppcheck --enable=unusedFunction`, gcov

```c
// ❌ PROHIBITED - Unreachable code
void bad_function() {
    return;
    int x = 5;  // Unreachable!
}

// ✅ CORRECT - All code reachable
void good_function() {
    int x = 5;
    return;
}
```

**MISRA C Rule 2.2: No Dead Code (Required)**
- **Category:** Required (SHALL)
- **Rule:** There shall be no dead code
- **Rationale:** Operations with no effect indicate errors
- **Standards:** IEC 61508-7 C.5.10, ISO 26262-6 9.4.5, DO-178C 6.4.4.3.c
- **Check:** Static analysis, code review
- **Severity:** MANDATORY
- **Automation:** `cppcheck --enable=style`

```c
// ❌ PROHIBITED - Dead code
void dead_assignment() {
    int x = 5;
    x = 10;  // Dead - never used
}

// ✅ CORRECT - All code has effect
void live_code() {
    int x = 5;
    use_value(x);
}
```

**MISRA C Rule 8.1: Types Shall Be Explicitly Specified (Required)**
- **Category:** Required (SHALL)
- **Rule:** Types shall be explicitly specified (no implicit int)
- **Rationale:** Prevents confusion from C90 implicit int assumption
- **Check:** Compiler warnings
- **Severity:** MANDATORY
- **Automation:** GCC `-Wimplicit-int`

```c
// ❌ PROHIBITED - Implicit type (C90)
extern func();  // Assumes int return type

// ✅ CORRECT - Explicit type
extern int func(void);
```

**MISRA C Rule 8.3: Consistent Function Declarations (Required)**
- **Category:** Required (SHALL)
- **Rule:** All declarations of an object or function shall use the same names and type qualifiers
- **Rationale:** Encourages strong typing and interface consistency
- **Check:** Static analysis
- **Severity:** MANDATORY
- **Automation:** `cppcheck --enable=all`

```c
// ❌ PROHIBITED - Inconsistent declarations
extern void func(signed int);
void func(int x) { }

// ✅ CORRECT - Consistent
extern void func(int x);
void func(int x) { }
```

**MISRA C Rule 9.1: Automatic Variables Must Be Initialized Before Read (Mandatory)**
- **Category:** Mandatory (SHALL)
- **Rule:** The value of an object with automatic storage duration shall not be read before it has been set
- **Rationale:** Reading uninitialized variables is undefined behavior
- **Check:** Static analysis
- **Severity:** CRITICAL
- **Automation:** GCC `-Wuninitialized`, `cppcheck --enable=all`

```c
// ❌ PROHIBITED - Uninitialized read
void bad_init(void) {
    int x;
    if (x > 0) { }
}

// ✅ CORRECT - Initialize before use
void good_init(void) {
    int x = 0;
    if (x > 0) { }
}
```

**MISRA C Rule 10.1: Operands Must Have Appropriate Essential Types (Required)**
- **Category:** Required (SHALL)
- **Rule:** Operands shall not be of an inappropriate essential type
- **Rationale:** Prevents unintended implicit promotions and conversions
- **Check:** Static analysis, compiler warnings
- **Severity:** MANDATORY
- **Automation:** GCC `-Wconversion`, `cppcheck --enable=all`

```c
// ❌ PROHIBITED - Mixed essential types
uint8_t u8 = 10U;
int16_t s16 = -1;
int result = u8 + s16;  // Mixed signed/unsigned

// ✅ CORRECT - Explicit, validated conversion
int16_t s16_safe = s16;
int result = (int)u8 + (int)s16_safe;
```

**MISRA C Rule 10.3: No Narrowing or Cross-Category Assignments (Required)**
- **Category:** Required (SHALL)
- **Rule:** The value of an expression shall not be assigned to an object with a narrower essential type or of a different essential type category
- **Rationale:** Prevents data loss and type confusion
- **Check:** Compiler warnings
- **Severity:** MANDATORY
- **Automation:** GCC `-Wconversion`, `-Wsign-conversion`

```c
// ❌ PROHIBITED - Narrowing without cast
uint8_t val8 = 300U;

// ✅ CORRECT - Explicit cast with validation
int raw = 300;
if (raw <= 255) {
    uint8_t val8 = (uint8_t)raw;
}
```

**MISRA C Rule 10.4: Operands Must Share Essential Type Category (Required)**
- **Category:** Required (SHALL)
- **Rule:** Both operands of an operator subject to the usual arithmetic conversions shall have the same essential type category
- **Rationale:** Prevents unintended conversions and loss of information
- **Check:** Static analysis
- **Severity:** MANDATORY
- **Automation:** `cppcheck --enable=all`

```c
// ❌ PROHIBITED - Mixed essential categories (implicit promotion)
uint16_t u16 = 100U;
int16_t s16 = -2;
int32_t sum = u16 + s16;  // Implicit conversion - ambiguous!

// ❌ ALSO PROHIBITED - Comparison with mixed types
uint8_t byte = 255U;
int16_t value = -1;
if (byte > value) { }  // Are we comparing unsigned or signed?

// ✅ CORRECT - Explicit conversion to common type
uint16_t u16 = 100U;
int16_t s16 = -2;
int32_t sum = (int32_t)u16 + (int32_t)s16;  // Explicit and clear

// ✅ CORRECT - Ensure both operands are same category
int8_t signed_val = -1;
int8_t unsigned_val = 255;
int16_t result = (int16_t)signed_val + (int16_t)unsigned_val;  // Cast both to same type
```

**MISRA C Rule 11.3: No Casting Between Different Object Pointer Types (Required)**
- **Category:** Required (SHALL)
- **Rule:** A cast shall not be performed between a pointer to object type and a pointer to a different object type
- **Rationale:** May cause alignment errors and undefined behavior
- **Check:** Static analysis
- **Severity:** CRITICAL
- **Automation:** `cppcheck --enable=all`

```c
// ❌ PROHIBITED - Pointer type punning
uint16_t* p16 = (uint16_t*)&value32;

// ✅ CORRECT - Use memcpy or unions only with approved deviation
uint16_t out;
memcpy(&out, &value32, sizeof(out));
```

**MISRA C Rule 12.1: Explicit Operator Precedence (Advisory)**
- **Category:** Advisory (SHOULD)
- **Rule:** The precedence of operators within expressions should be made explicit
- **Rationale:** Reduces misunderstanding and bugs
- **Check:** Code review
- **Severity:** RECOMMENDED
- **Automation:** Manual review

```c
// ⚠️ DISCOURAGED - Implicit precedence
if (a & b == c) { }

// ✅ CORRECT - Explicit parentheses
if ((a & b) == c) { }
```

**MISRA C Rule 13.2: Expression Side Effects Used Only Once (Required)**
- **Category:** Required (SHALL)
- **Rule:** The value of an expression and its persistent side effects shall be the same under all permitted evaluation orders
- **Rationale:** Order of evaluation is often unspecified
- **Check:** Static analysis
- **Severity:** CRITICAL
- **Automation:** `cppcheck --enable=style`

```c
// ❌ PROHIBITED - Multiple side effects
array[i++] = i++;

// ✅ CORRECT - Single side effect
array[i] = value;
i++;
```

**MISRA C Rule 14.3: Controlling Expressions Shall Not Be Invariant (Required)**
- **Category:** Required (SHALL)
- **Rule:** Controlling expressions shall not be invariant
- **Rationale:** Invariant conditions often indicate errors
- **Check:** Static analysis
- **Severity:** MANDATORY
- **Automation:** `cppcheck --enable=style`

```c
// ❌ PROHIBITED - Invariant condition
if (1) {
    do_something();
}

// ✅ CORRECT - Meaningful condition
if (sensor_value > THRESHOLD) {
    do_something();
}
```

**MISRA C Rule 16.3: Every Switch Clause Shall Have Unconditional Break (Required)**
- **Category:** Required (SHALL)
- **Rule:** An unconditional break statement shall terminate every switch-clause
- **Rationale:** Prevents accidental switch fallthrough bugs
- **Check:** Compiler warnings
- **Severity:** MANDATORY
- **Automation:** GCC `-Wimplicit-fallthrough`

```c
// ❌ PROHIBITED - No break
switch (state) {
    case STATE_INIT:
        init();
    case STATE_RUN:
        run();
        break;
}

// ✅ CORRECT - explicit break
switch (state) {
    case STATE_INIT:
        init();
        break;
    case STATE_RUN:
        run();
        break;
    default:
        break;
}
```

**MISRA C Rule 17.7: Return Value Shall Be Used (Required)**
- **Category:** Required (SHALL)
- **Rule:** The value returned by a function having non-void return type shall be used
- **Rationale:** Ignoring return values hides errors
- **Check:** Compiler warnings
- **Severity:** CRITICAL
- **Automation:** GCC `-Wunused-result`

```c
// ❌ PROHIBITED - Ignored return value
read_sensor();

// ✅ CORRECT - Check return value
int status = read_sensor();
if (status != OK) {
    handle_error(status);
}
```

**MISRA C Rule 18.1: Pointer Arithmetic Within Array Bounds (Required)**
- **Category:** Required (SHALL)
- **Rule:** A pointer resulting from arithmetic on a pointer operand shall address an element of the same array as that pointer operand
- **Rationale:** Out-of-bounds pointer arithmetic is undefined
- **Check:** Static analysis
- **Severity:** CRITICAL
- **Automation:** `cppcheck --enable=all`

```c
// ❌ PROHIBITED - Out of bounds arithmetic
int array[10];
int* p = &array[0];
p += 15;

// ✅ CORRECT - Bounded arithmetic
int* p = &array[0];
if (index < 10) {
    p += index;
}
```

**MISRA C Rule 21.3: No Dynamic Memory Allocation (Required)**
- **Category:** Required (SHALL)
- **Rule:** The memory allocation and deallocation functions of <stdlib.h> shall not be used
- **Rationale:** Dynamic allocation risks fragmentation and undefined behavior
- **Check:** Grep search
- **Severity:** CRITICAL
- **Automation:** `grep -r '\bmalloc\|calloc\|realloc\|free\b' src/`

```c
// ❌ PROHIBITED - Dynamic allocation
int* ptr = (int*)malloc(100U * sizeof(int));
free(ptr);

// ✅ CORRECT - Static or stack allocation
int array[100];
static int global_array[100];
```

**MISRA C Rule 21.6: No Standard I/O Functions (Required)**
- **Category:** Required (SHALL)
- **Rule:** The Standard Library input/output functions shall not be used
- **Rationale:** Standard I/O may be unavailable or non-deterministic on embedded targets
- **Check:** Grep search
- **Severity:** MANDATORY
- **Automation:** `grep -r '\bprintf\|scanf\|fopen\|fclose\b' src/`

```c
// ❌ PROHIBITED - Standard I/O
printf("Debug: %d\n", value);

// ✅ CORRECT - Target-specific logging
debug_log(value);
```

### MISRA Rule Categories

| Category | Rules | Automation Level | Tool |
|----------|-------|------------------|------|
| **Required** | Must comply | High | cppcheck, compiler |
| **Advisory** | Should comply | Medium | Code review, cppcheck |
| **Document** | Deviations must be documented | Manual | Review process |

---

## ISO 25119 Coding Requirements

### ISO 25119-3 Software Design Rules

ISO 25119-3:2018 specifies software design requirements for safety functions in agricultural machinery. Key coding requirements:

**ISO-001: Traceable Requirements**
- **Rule:** All code must trace to a software requirement
- **Rationale:** ISO 25119 mandates full traceability
- **Check:** Traceability matrix validation
- **Severity:** MANDATORY
- **Automation:** grep for requirement IDs in comments, traceability script

```c
// ✅ CORRECT - Requirement traceability
// Implements: SWReq-001 - Motor shall stop when button released
void check_button_release() {
    if (!button_pressed && motor_running) {
        stop_motor();  // SWReq-001
    }
}
```

**ISO-002: Defensive Programming Mandatory**
- **Rule:** Validate all inputs, check all outputs, assert invariants
- **Rationale:** ISO 25119 requires defensive programming techniques
- **Check:** Code review, assertion count
- **Severity:** MANDATORY
- **Automation:** Count assertions per function

**ISO-003: Software Diversity for Safety Functions**
- **Rule:** Critical safety decisions must use independent checks
- **Rationale:** Single point of failure prevention
- **Check:** Architecture review
- **Severity:** CRITICAL (for AgPL c or d)
- **Automation:** Manual architectural review

```c
// ✅ CORRECT - Dual-channel safety check
bool is_safe_to_move_up() {
    // Primary check: limit sensor
    bool primary_safe = !limit_switch_top;
    
    // Secondary check: position estimation
    bool secondary_safe = (estimated_position < MAX_POSITION);
    
    // Both must agree
    return primary_safe && secondary_safe;
}
```

**ISO-004: Fail-Safe Behavior**
- **Rule:** All error conditions must result in safe state
- **Rationale:** ISO 25119 core principle
- **Check:** Error handling review
- **Severity:** CRITICAL
- **Automation:** grep for error returns, verify safe state calls

**ISO-005: Watchdog Monitoring**
- **Rule:** Software must implement watchdog/alive monitoring
- **Rationale:** Detect CPU crashes and lockups
- **Check:** Architecture review
- **Severity:** MANDATORY (for AgPL c or d)
- **Automation:** Verify watchdog refresh calls

**ISO-006: Memory Integrity Checks**
- **Rule:** Critical data must use integrity checks (CRC, parity)
- **Rationale:** Detect RAM corruption
- **Check:** Code review
- **Severity:** MANDATORY (for AgPL c or d)
- **Automation:** Verify CRC calculation for critical data

**ISO-007: Plausibility Checks**
- **Rule:** Sensor values must be plausibility checked
- **Rationale:** Detect sensor failures
- **Check:** Code review
- **Severity:** MANDATORY
- **Automation:** Verify range checks on all sensor inputs

```c
// ❌ PROHIBITED - No plausibility checks
int16_t read_motor_current_unchecked() {
    return HAL_read_motor_current_ma();  // Assumes sensor is good
}

// ✅ CORRECT - Comprehensive plausibility checking
int16_t read_motor_current_checked(void) {
    int16_t current_ma = HAL_read_motor_current_ma();
    
    // Plausibility: motor current has physical limits
    const int16_t MAX_PLAUSIBLE_CURRENT = 5000;  // 5A max for this motor
    if (current_ma > MAX_PLAUSIBLE_CURRENT) {
        // Sensor failure or short circuit detected!
        trigger_safety_shutdown();
        return -1;  // Invalid reading
    }
    
    // Plausibility: negative current impossible (unidirectional)
    if (current_ma < 0) {
        // Sensor inversion or failure
        trigger_safety_shutdown();
        return -1;  // Invalid reading
    }
    
    // Plausibility: rate of change check
    static int16_t last_current = 0;
    const int16_t MAX_RATE_CHANGE = 500;  // Max 500mA/sample change
    if (abs(current_ma - last_current) > MAX_RATE_CHANGE) {
        // Unrealistic jump indicates sensor glitch
        return last_current;  // Return last known good value
    }
    
    last_current = current_ma;
    return current_ma;
}
```

### Defensive Programming (ISO 25119)

**ISO-008: Range Checking**
- **Rule:** All numeric parameters must be range-checked
- **Check:** Parameter validation at function entry
- **Severity:** MANDATORY
- **Automation:** Check for validation code in all public functions

```c
// ❌ PROHIBITED - No range checking
void set_motor_speed(uint8_t speed_percent) {
    motor_pwm = (speed_percent * 255) / 100;  // Assumes 0-100
}

// ✅ CORRECT - Comprehensive range checking
ErrorCode set_motor_speed(uint8_t speed_percent) {
    // Validate input range
    if (speed_percent > 100) {
        return ERR_OUT_OF_RANGE;
    }
    
    // Ensure motor is in safe state
    if (!motor_initialized) {
        return ERR_NOT_INITIALIZED;
    }
    
    // Convert and apply
    motor_pwm = (speed_percent * 255) / 100;
    return ERR_OK;
}

// ✅ ALSO GOOD - With assertions for documentation
void set_position_absolute(int16_t position_mm) {
    // Precondition: position must be within valid range for desk
    assert(position_mm >= MIN_DESK_HEIGHT_MM && 
           position_mm <= MAX_DESK_HEIGHT_MM);
    
    assert(motor_initialized);  // Safety check
    
    // Perform movement
    move_to_position(position_mm);
}
```

**ISO-009: State Machine Validity**
- **Rule:** State machines must validate state transitions
- **Check:** State transition matrix verification
- **Severity:** MANDATORY
- **Automation:** Verify default case in state switches

```c
// ❌ PROHIBITED - No transition validation
void change_state_unsafe(State new_state) {
    current_state = new_state;  // Any transition allowed!
}

// ❌ ALSO PROHIBITED - Default case missing
void handle_state(State s) {
    switch(s) {
        case STATE_IDLE:
            idle_handler();
            break;
        case STATE_MOVING:
            moving_handler();
            break;
        // What about unexpected states? No default!
    }
}

// ✅ CORRECT - Valid state transition checking
ErrorCode change_state(State new_state) {
    // Validate new state is in valid range
    if (new_state < STATE_MIN || new_state > STATE_MAX) {
        return ERR_INVALID_STATE;
    }
    
    // Check transition is legal per state machine diagram
    if (!is_valid_transition(current_state, new_state)) {
        return ERR_ILLEGAL_TRANSITION;
    }
    
    current_state = new_state;
    return ERR_OK;
}

// ✅ CORRECT - Default case always present
void handle_state(State s) {
    switch(s) {
        case STATE_IDLE:
            idle_handler();
            break;
        case STATE_MOVING:
            moving_handler();
            break;
        case STATE_ERROR:
            error_handler();
            break;
        default:
            // Unexpected state - safety shutdown!
            SAFETY_ASSERT(false);  // This should never happen
            trigger_fault();
            break;
    }
}
```

**ISO-010: Overflow Prevention**
- **Rule:** Arithmetic operations must check for overflow
- **Check:** Static analysis, runtime checks
- **Severity:** MANDATORY
- **Automation:** Compiler warnings, static analysis

```c
// ❌ PROHIBITED - No overflow detection
void accumulate_unsafe(uint16_t new_value) {
    total_sum += new_value;  // Could overflow without detection!
}

// ❌ ALSO PROHIBITED - Silent wrap-around
uint8_t counter = 255;
counter++;  // Now 0 - silent overflow!

// ✅ CORRECT - Overflow with saturating arithmetic
ErrorCode accumulate_safe(uint16_t new_value) {
    // Check if addition would overflow
    if (total_sum > (UINT16_MAX - new_value)) {
        total_sum = UINT16_MAX;  // Saturate at maximum
        return ERR_OVERFLOW;  // Notify caller
    }
    
    total_sum += new_value;
    return ERR_OK;
}

// ✅ CORRECT - Overflow detection with wider type
uint16_t safe_add_u16(uint16_t a, uint16_t b, bool* overflow) {
    // Use wider type to detect overflow
    uint32_t result = (uint32_t)a + (uint32_t)b;
    
    if (result > UINT16_MAX) {
        *overflow = true;
        return UINT16_MAX;  // Saturate
    }
    
    *overflow = false;
    return (uint16_t)result;
}

// ✅ ALSO CORRECT - Check before operation
uint8_t safe_increment(uint8_t value) {
    if (value == UINT8_MAX) {
        return UINT8_MAX;  // Already at max
    }
    return value + 1;  // Safe to increment
}
```

---

## Automated Checking

### Comprehensive Automated Checks Table

This table provides complete automation guidance for all rules. Use this for implementing automated checking scripts.

**Rule Scripts:** Each automated rule listed in the Rule Index has a dedicated script under `toolchain/rules/`.
Run a rule script by passing the source path:

```bash
python toolchain/rules/rule_RULE-001_R1_1_no_goto.py src/
```

All rule scripts use the shared helper library `toolchain/rules/common.py` to iterate and scan the provided path.

| Rule ID | Standard | Check Method | Tool/Command | Exit on Fail | Severity |
|---------|----------|--------------|--------------|--------------|----------|
| **NASA Rules** |
| R1.1 | NASA-01 | Grep pattern | `grep -r '\bgoto\b' src/` | Yes | CRITICAL |
| R1.2 | NASA-01 | Grep pattern | `grep -r 'setjmp\|longjmp' src/` | Yes | CRITICAL |
| R1.3 | NASA-01 | Call graph analysis | Manual review | Yes | CRITICAL |
| R2.1 | NASA-02 | Loop bound analysis | Manual review | Yes | CRITICAL |
| R2.3 | NASA-02 | Grep pattern | `grep -r 'while\s*\(\s*true\s*\)' src/` | Review | CRITICAL |
| R3.1 | NASA-03 | Grep pattern | `grep -r '\bmalloc\|calloc\|realloc\|free\b' src/` | Yes | CRITICAL |
| R3.2 | NASA-03 | Hidden allocation review | Code review | Review | MANDATORY |
| R3.4 | NASA-03 | Stack usage analysis | Static analysis | Yes | MANDATORY |
| R4.1 | NASA-04 | Line count per function | `wc -l` or custom script | Yes | MANDATORY |
| R4.3 | NASA-04 | Cyclomatic complexity | `cppcheck --enable=all` | Yes | MANDATORY |
| R5.2 | NASA-05 | Parameter validation | Code review | No | MANDATORY |
| R5.3 | NASA-05 | Invariant assertions | Code review | No | CRITICAL |
| R6.4 | NASA-06 | Uninitialized variables | `gcc -Wuninitialized` | Yes | MANDATORY |
| R7.1 | NASA-07 | Unused result | `gcc -Wunused-result` | Yes | CRITICAL |
| R7.3 | NASA-07 | NULL dereference | `cppcheck --enable=all` | Yes | CRITICAL |
| R8.2 | NASA-08 | Function-like macros | `grep -r '#define.*(' src/` | Review | MANDATORY |
| R9.2 | NASA-09 | Pointer arithmetic | `cppcheck --enable=portability` | Review | MANDATORY |
| R9.3 | NASA-09 | Function pointers | `grep -r '\(\*.*\)\s*\(' src/` | Review | MANDATORY |
| R10.1 | NASA-10 | Compiler flags | Check CMakeLists.txt | Yes | CRITICAL |
| R10.2 | NASA-10 | Warning count | GCC/Clang output | Yes | CRITICAL |
| R10.3 | NASA-10 | Static analysis run | `cppcheck --enable=all src/` | Yes | MANDATORY |
| R10.4 | NASA-10 | Analysis error count | cppcheck XML output | Yes | MANDATORY |
| **MISRA C Rules** |
| MISRA-1.3 | MISRA C | Undefined/unspecified behavior | `cppcheck --enable=all` | Yes | CRITICAL |
| MISRA-2.1 | MISRA C | Unreachable code | `cppcheck --enable=unusedFunction` | Yes | MANDATORY |
| MISRA-2.2 | MISRA C | Dead code | `cppcheck --enable=style` | Yes | MANDATORY |
| MISRA-9.1 | MISRA C | Uninitialized auto vars | `gcc -Wuninitialized` | Yes | CRITICAL |
| MISRA-10.1 | MISRA C | Essential type misuse | `gcc -Wconversion` | Yes | MANDATORY |
| MISRA-10.3 | MISRA C | Narrowing assignment | `gcc -Wconversion` | Yes | MANDATORY |
| MISRA-11.3 | MISRA C | Pointer cast mismatch | `cppcheck --enable=all` | Yes | CRITICAL |
| MISRA-13.2 | MISRA C | Multiple side effects | `cppcheck --enable=style` | Review | CRITICAL |
| MISRA-16.3 | MISRA C | Switch fallthrough | `gcc -Wimplicit-fallthrough` | Yes | MANDATORY |
| MISRA-17.7 | MISRA C | Unused return value | `gcc -Wunused-result` | Yes | CRITICAL |
| MISRA-18.1 | MISRA C | Pointer arithmetic bounds | `cppcheck --enable=all` | Review | CRITICAL |
| MISRA-21.3 | MISRA C | Dynamic allocation | `grep -r '\bmalloc\|calloc\|realloc\|free\b' src/` | Yes | CRITICAL |
| MISRA-21.6 | MISRA C | Standard I/O usage | `grep -r '\bprintf\|scanf\|fopen\|fclose\b' src/` | Yes | MANDATORY |
| **ISO 25119 Requirements** |
| ISO-001 | ISO 25119 | Requirement trace | `grep -r 'SWReq-' src/` + validation | Review | MANDATORY |
| ISO-002 | ISO 25119 | Defensive prog | Assertion count script | Review | MANDATORY |
| ISO-004 | ISO 25119 | Fail-safe behavior | Error handling review | No | CRITICAL |
| ISO-007 | ISO 25119 | Plausibility checks | Range check validation | Review | MANDATORY |
| ISO-008 | ISO 25119 | Range checking | Parameter validation grep | Review | MANDATORY |
| ISO-009 | ISO 25119 | State validation | Default case in switches | Review | MANDATORY |
| ISO-010 | ISO 25119 | Overflow checks | Arithmetic analysis | Review | MANDATORY |
| R4.1 | Line count per function | Custom script | Yes |
| R4.3 | Cyclomatic complexity | cppcheck | Yes (if >10) |
| R7.1 | Unused result warnings | GCC `-Wunused-result` | Yes |
| R7.3 | NULL dereference | cppcheck | Yes |
| R9.2 | Pointer arithmetic detection | cppcheck | Review |
| R9.3 | grep function pointers | grep | Review |
| R10.1 | Compiler flags present | CMake check | Yes |
| R10.2 | Compilation warnings | GCC/Clang | Yes |
| R10.3 | cppcheck execution | cppcheck | Yes |

### Automation Scripts Reference

These commands can be integrated into automated checking scripts:

**Basic Prohibition Checks:**
```bash
#!/bin/bash
# Check for prohibited constructs

ERRORS=0

# NASA-01: No goto
if grep -rn '\bgoto\b' src/; then
    echo "ERROR: goto statement found"
    ERRORS=$((ERRORS+1))
fi

# NASA-03: No dynamic allocation
if grep -rn '\bmalloc\|calloc\|realloc\|free\b' src/; then
    echo "ERROR: Dynamic memory allocation found"
    ERRORS=$((ERRORS+1))
fi

exit $ERRORS
```

**Function Length Check:**
```python
#!/usr/bin/env python3
# Check function length (NASA-04)

import re
import sys
from pathlib import Path

MAX_FUNCTION_LINES = 60
errors = 0

for src_file in list(Path('src').rglob('*.c')) + list(Path('src').rglob('*.cpp')):
    with open(src_file) as f:
        content = f.read()
    
    # Simple function detection for C/C++ (improve as needed)
    functions = re.findall(
        r'^[a-zA-Z_][\w\s\*]*\s+[a-zA-Z_][\w]*\s*\([^;]*\)\s*\{([^}]*)\}',
        content,
        re.MULTILINE | re.DOTALL
    )
    
    for func_body in functions:
        lines = [l for l in func_body.split('\n') 
                if l.strip() and not l.strip().startswith('//')]
        if len(lines) > MAX_FUNCTION_LINES:
            print(f"ERROR: Function in {src_file} exceeds {MAX_FUNCTION_LINES} lines")
            errors += 1

sys.exit(1 if errors > 0 else 0)
```

**Assertion Density Check:**
```python
#!/usr/bin/env python3
# Check assertion density (NASA-05, ISO-002)

import re
import sys
from pathlib import Path

MIN_ASSERTIONS_PER_FUNCTION = 2
warnings = 0

for src_file in list(Path('src').rglob('*.c')) + list(Path('src').rglob('*.cpp')):
    with open(src_file) as f:
        content = f.read()
    
    # Count functions (C/C++)
    functions = re.findall(r'^[a-zA-Z_][\w\s\*]*\s+[a-zA-Z_][\w]*\s*\([^;]*\)', content, re.MULTILINE)
    
    # Count assertions
    assertions = len(re.findall(r'\bassert\(', content))
    
    if len(functions) > 0:
        density = assertions / len(functions)
        if density < MIN_ASSERTIONS_PER_FUNCTION:
            print(f"WARNING: {src_file} has low assertion density: {density:.1f}")
            warnings += 1

print(f"Assertion density check: {warnings} warnings")
sys.exit(0)  # Warning only, don't fail build
```

**Requirement Traceability Check:**
```python
#!/usr/bin/env python3
# Check requirement traceability (ISO-001)

import re
import sys
from pathlib import Path

requirements = []  # Load from requirements document
traced_requirements = set()

for src_file in list(Path('src').rglob('*.c')) + list(Path('src').rglob('*.cpp')):
    with open(src_file) as f:
        content = f.read()
    
    # Find requirement IDs in comments
    req_ids = re.findall(r'SWReq-\d+', content)
    traced_requirements.update(req_ids)

print(f"Traced {len(traced_requirements)} requirements in code")
# Compare with requirement database for gaps
```

### Manual Review Rules (Require human judgment)

| Rule ID | Standard | Description | Review Focus |
|---------|----------|-------------|--------------|
| R1.3 | NASA-01 | Recursion prohibited | Function call graph analysis |
| R1.4 | NASA-01 | Single entry/exit preferred | Code review for clarity |
| R2.1 | NASA-02 | Loop bounds provable | Loop termination analysis |
| R2.4 | NASA-02 | Loop invariants documented | Documentation review |
| R4.2 | NASA-04 | Single responsibility | Design review |
| R5.1 | NASA-05 | Assertion density (2+/function) | Count assertions per function |
| R5.2 | NASA-05 | Precondition checks present | Parameter validation review |
| R6.1 | NASA-06 | Variable scope minimized | Code structure review |
| R6.2 | NASA-06 | Global variable justification | Architecture review |
| MISRA-13.2 | MISRA C | Side effects in expressions | Expression complexity |
| ISO-003 | ISO 25119 | Software diversity | Architectural analysis |
| ISO-005 | ISO 25119 | Watchdog implementation | Verify refresh calls |
| ISO-006 | ISO 25119 | Memory integrity checks | CRC implementation review |
| ISO-009 | ISO 25119 | State transition validation | State machine analysis |

---

## Appendix: Quick Reference

### Critical Rules (Must Never Violate)

1. ❌ **NO goto** (R1.1)
2. ❌ **NO setjmp/longjmp** (R1.2)
3. ❌ **NO recursion** (R1.3)
4. ❌ **NO unbounded loops** (R2.1, R2.3)
5. ❌ **NO malloc/free** (R3.1)
6. ❌ **NO hidden dynamic allocation** (R3.2)
7. ✅ **CHECK all return values** (R7.1)
8. ✅ **VALIDATE all pointers** (R7.3)
9. ✅ **COMPILE with -Wall -Wextra -Werror** (R10.1)
10. ✅ **ZERO warnings, ZERO static analysis errors** (R10.2, R10.4)

### Recommended Practices

- ✅ Keep functions under 60 lines (R4.1)
- ✅ Use 2+ assertions per function (R5.1)
- ✅ Minimize variable scope (R6.1)
- ✅ Prefer non-NULL interfaces where available (R9.1)
- ✅ Initialize variables at declaration (R6.4)
- ✅ Document safety-critical code (R12.1)
- ✅ Write unit tests for all functions (R13.1)

### Automated Checking Command

```bash
# Run from workspace root
python toolchain/run-pipeline.py all

# Includes:
# - Build with all warnings
# - Static analysis (cppcheck)
# - Unit/component/integration tests
# - Dual motor type validation
```

### Tool Configuration

**Compiler Flags (GCC/Clang) - Full Set:**
```bash
# Mandatory flags for compliance
-Wall                    # All warnings
-Wextra                  # Extra warnings
-Werror                  # Treat warnings as errors
-pedantic                # Strict ISO C/C++ compliance
-std=c11                 # C standard (use -std=c++17 for C++)
-Wconversion             # Implicit conversions (MISRA-003)
-Wshadow                 # Variable shadowing
-Wunused-result          # Unchecked return values (R7.1)
-Wimplicit-fallthrough   # Switch fallthrough (MISRA-005)
-Wfloat-equal            # Floating point comparison
-Wundef                  # Undefined macros
-Wcast-align             # Alignment cast issues
```

**Cppcheck Configuration - Comprehensive:**
```bash
# Run cppcheck with full analysis
# Use --std=c++17 for C++ projects
cppcheck \
    --enable=all \
    --std=c11 \
    --inconclusive \
    --force \
    --verbose \
    --error-exitcode=1 \
    --suppress=missingIncludeSystem \
    --suppress=missingInclude \
    --inline-suppr \
    --template=gcc \
    --xml \
    --xml-version=2 \
  src/
```

**MISRA Checking Tools:**
```bash
# Commercial tools (recommended for full MISRA compliance):
# - PC-lint Plus (Gimpel Software)
# - Cppcheck Premium (with MISRA addon)
# - LDRA Testbed
# - Polyspace Bug Finder

# Open-source approximation:
cppcheck --addon=misra --enable=all src/
# Note: Basic MISRA checking, not complete certification
```

**Integration with Pipeline:**
```bash
# All checks are integrated in:
python toolchain/run-pipeline.py all

# Custom compliance check:
python toolchain/check_compliance.py  # To be implemented
```

---

## Conclusion

These guidelines ensure our code is:
- ✅ **Safe:** No undefined behavior or runtime failures
- ✅ **Predictable:** Deterministic and analyzable
- ✅ **Verifiable:** All paths testable and provable
- ✅ **Maintainable:** Clear, consistent, and documented

**Remember:** When in doubt, choose safety over convenience.

**Questions?** Contact the software lead or refer to:

**Project Resources:**
- [NASA Power of Ten Rules (PDF)](resources/nasa10rules.pdf) - Local reference
- [NASA JPL Power of Ten Rules (Online)](https://en.wikipedia.org/wiki/The_Power_of_Ten:_Rules_for_Developing_Safety-Critical_Code)

**Standards:**
- [MISRA C:2012](https://www.misra.org.uk/) - Guidelines for the use of the C language in critical systems
- [ISO 25119 Functional Safety](https://www.iso.org/standard/72578.html) - Safety of machinery – Agricultural and forestry machinery
- [ASPICE](https://www.automotivespice.com/) - Automotive SPICE Process Assessment Model

**Tools:**
- [Cppcheck](https://cppcheck.sourceforge.io/) - Static analysis tool
- [GCC Compiler](https://gcc.gnu.org/) - Warning flags documentation
- [Clang Static Analyzer](https://clang-analyzer.llvm.org/) - Advanced static analysis


