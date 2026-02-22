# Comprehensive Rule Analysis Report

**Generated:** 2026-02-22 17:44:29
**Directory:** `src`

[📋 Back to Coding Guidelines](../coding_guides_and_checks.md)

## Execution Summary

- **Total Rules Scanned:** 41
- **✅ Passed:** 40
- **❌ Failed:** 1
- **⚠️  Skipped:** 0
- **Success Rate:** 97.6%

## Detailed Results

| Rule ID | Script | Status | Log |
|---------|--------|--------|-----|
| RULE-001 | `rule_RULE-001_R1_1_no_goto.py` | ✅ PASS | [RULE-001.log](./RULE-001.log) |
| RULE-002 | `rule_RULE-002_R1_2_no_setjmp_longjmp.py` | ✅ PASS | [RULE-002.log](./RULE-002.log) |
| RULE-007 | `rule_RULE-007_R2_3_no_while_true.py` | ✅ PASS | [RULE-007.log](./RULE-007.log) |
| RULE-009 | `rule_RULE-009_R3_1_no_malloc_free.py` | ✅ PASS | [RULE-009.log](./RULE-009.log) |
| RULE-011 | `rule_RULE-011_R3_4_stack_limits.py` | ✅ PASS | [RULE-011.log](./RULE-011.log) |
| RULE-012 | `rule_RULE-012_R4_1_function_length.py` | ✅ PASS | [RULE-012.log](./RULE-012.log) |
| RULE-014 | `rule_RULE-014_R4_3_complexity.py` | ✅ PASS | [RULE-014.log](./RULE-014.log) |
| RULE-021 | `rule_RULE-021_R6_2_minimize_globals.py` | ❌ FAIL | [RULE-021.log](./RULE-021.log) |
| RULE-023 | `rule_RULE-023_R6_4_init_at_decl.py` | ✅ PASS | [RULE-023.log](./RULE-023.log) |
| RULE-024 | `rule_RULE-024_R7_1_unused_return.py` | ✅ PASS | [RULE-024.log](./RULE-024.log) |
| RULE-026 | `rule_RULE-026_R7_3_null_checks.py` | ✅ PASS | [RULE-026.log](./RULE-026.log) |
| RULE-029 | `rule_RULE-029_R8_2_no_function_macros.py` | ✅ PASS | [RULE-029.log](./RULE-029.log) |
| RULE-030 | `rule_RULE-030_R8_3_named_constants.py` | ✅ PASS | [RULE-030.log](./RULE-030.log) |
| RULE-033 | `rule_RULE-033_R9_2_no_pointer_arith.py` | ✅ PASS | [RULE-033.log](./RULE-033.log) |
| RULE-034 | `rule_RULE-034_R9_3_no_function_pointers.py` | ✅ PASS | [RULE-034.log](./RULE-034.log) |
| RULE-035 | `rule_RULE-035_R9_4_no_multilevel_pointers.py` | ✅ PASS | [RULE-035.log](./RULE-035.log) |
| RULE-037 | `rule_RULE-037_R10_1_compiler_flags.py` | ✅ PASS | [RULE-037.log](./RULE-037.log) |
| RULE-038 | `rule_RULE-038_R10_2_zero_warnings.py` | ✅ PASS | [RULE-038.log](./RULE-038.log) |
| RULE-039 | `rule_RULE-039_R10_3_cppcheck_run.py` | ✅ PASS | [RULE-039.log](./RULE-039.log) |
| RULE-040 | `rule_RULE-040_R10_4_cppcheck_clean.py` | ✅ PASS | [RULE-040.log](./RULE-040.log) |
| RULE-041 | `rule_RULE-041_R10_5_misra_compliance.py` | ✅ PASS | [RULE-041.log](./RULE-041.log) |
| RULE-049 | `rule_RULE-049_R14_1_function_length.py` | ✅ PASS | [RULE-049.log](./RULE-049.log) |
| RULE-049 | `rule_RULE-049_R14_1_function_length.py` | ✅ PASS | [RULE-049.log](./RULE-049.log) |
| RULE-050 | `rule_RULE-050_R14_2_parameter_count.py` | ✅ PASS | [RULE-050.log](./RULE-050.log) |
| RULE-050 | `rule_RULE-050_R14_2_parameter_count.py` | ✅ PASS | [RULE-050.log](./RULE-050.log) |
| RULE-051 | `rule_RULE-051_R14_3_nesting_depth.py` | ✅ PASS | [RULE-051.log](./RULE-051.log) |
| RULE-051 | `rule_RULE-051_R14_3_nesting_depth.py` | ✅ PASS | [RULE-051.log](./RULE-051.log) |
| RULE-052 | `rule_RULE-052_R14_4_file_length.py` | ✅ PASS | [RULE-052.log](./RULE-052.log) |
| RULE-053 | `rule_RULE-053_R14_5_assertion_density.py` | ✅ PASS | [RULE-053.log](./RULE-053.log) |
| RULE-054 | `rule_RULE-054_MISRA_9_1_uninit.py` | ✅ PASS | [RULE-054.log](./RULE-054.log) |
| RULE-055 | `rule_RULE-055_MISRA_10_1_types.py` | ✅ PASS | [RULE-055.log](./RULE-055.log) |
| RULE-056 | `rule_RULE-056_MISRA_10_3_narrowing.py` | ✅ PASS | [RULE-056.log](./RULE-056.log) |
| RULE-058 | `rule_RULE-058_MISRA_8_3_function_consistency.py` | ✅ PASS | [RULE-058.log](./RULE-058.log) |
| RULE-058 | `rule_RULE-058_MISRA_8_3_function_consistency.py` | ✅ PASS | [RULE-058.log](./RULE-058.log) |
| RULE-060 | `rule_RULE-060_MISRA_13_2_side_effects.py` | ✅ PASS | [RULE-060.log](./RULE-060.log) |
| RULE-063 | `rule_RULE-063_MISRA_17_7_return_used.py` | ✅ PASS | [RULE-063.log](./RULE-063.log) |
| RULE-065 | `rule_RULE-065_MISRA_21_3_no_malloc.py` | ✅ PASS | [RULE-065.log](./RULE-065.log) |
| RULE-066 | `rule_RULE-066_MISRA_21_6_no_stdio.py` | ✅ PASS | [RULE-066.log](./RULE-066.log) |
| RULE-066 | `rule_RULE-066_MISRA_21_6_no_stdio.py` | ✅ PASS | [RULE-066.log](./RULE-066.log) |
| RULE-067 | `rule_RULE-067_ISO_001_traceability.py` | ✅ PASS | [RULE-067.log](./RULE-067.log) |
| RULE-068 | `rule_RULE-068_ISO_002_assert_density.py` | ✅ PASS | [RULE-068.log](./RULE-068.log) |

## Failed Rules Details

### RULE-021

```
RULE_ID: RULE-021
SCRIPT: rule_RULE-021_R6_2_minimize_globals.py
SRC_PATH: src
STATUS: FAIL
EXIT_CODE: 1
TIMESTAMP: 2026-02-22 17:44:20
================================================================================
[FAIL] desk_app.cpp:5 Global variable 'current_state' should be minimized
       static AppState_t current_state = APP_STATE_IDLE;
[FAIL] desk_app.cpp:6 Global variable 'state_entry_time' should be minimized
       static uint32_t state_entry_time = 0U;
[FAIL] desk_app.cpp:9 Global variable 'button_fault_latched' should be minimized
       static bool button_fault_latched = false;      // Dual button press fault
[FAIL] desk_app.cpp:11 Global variable 'current_fault_latched' should be minimized
       static bool current_fault_latched = false;     // Stuck-on or obstruction fault
[FAIL] desk_app.cpp:14 Global variable 'stuck_on_timer_start_ms' should be minimized
       static uint32_t stuck_on_timer_start_ms = UINT32_MAX;    // Stuck-on detection when motor should be stopped
[FAIL] desk_app.cpp:15 Global variable 'obstruction_timer_start_ms' should be minimized
       static uint32_t obstruction_timer_start_ms = UINT32_MAX; // Obstruction detection during motion
[FAIL] hal.cpp:12 Global variable 'g_motor_type' should be minimized
       static MotorType_t g_motor_type = MT_BASIC;  // Default to MT_BASIC if not explicitly set
[FAIL] motor_controller.cpp:90 Global variable 'last_dir' should be minimized
       static MotorDirection_t last_dir = MOTOR_STOP;
[FAIL] motor_controller.cpp:101 Global variable 'dir_start_time' should be minimized
       static uint32_t dir_start_time = 0U;
[FAIL] motor_controller.cpp:109 Global variable 'last_update_time' should be minimized
       static uint32_t last_update_time = 0U;
[FAIL] motor_controller.cpp:123 Global variable 'low_pwm_start_time' should be minimized
       static uint32_t low_pwm_start_time = 0U;

[FAIL] RULE-021 VIOLATION: Found 11 potential global variables

```

