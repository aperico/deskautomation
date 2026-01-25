/**
 * @file motor_controller.h
 * @brief Motor Controller Signal Processing Module - Application Layer
 * 
 * @purpose
 * Provides hardware-agnostic motor control signal processing with soft-start PWM ramping,
 * direction management, and fault detection. This module operates at the APPLICATION LAYER
 * (no direct hardware access) and generates control signals for HAL consumption.
 * 
 * @requirements_traceability
 * - SysReq-006: Smooth Motion (< 0.5 g peak acceleration)
 *   → Implements PWM ramping (0→255 over 500 ms) to prevent jerking/sudden speed changes
 *   → Eliminates load disturbance (monitor shake, spilled drinks)
 * 
 * - SysReq-004: Full Stroke Performance (≤ 30 sec)
 *   → Ramp time tuned to balance smooth start vs. overall stroke time
 *   → 500 ms ramp is < 2% of 30 sec stroke budget
 * 
 * - SysReq-010: State Maintenance & Fault Detection
 *   → Stall detection via low-PWM timeout (2 sec threshold)
 *   → Fault flag enables error state transitions in application layer
 * 
 * @design_rationale
 * **Why Soft-Start (PWM Ramping)?**
 * - Direct 0→255 PWM step causes high inrush current and mechanical jerk
 * - Worm gear motor benefits from gradual torque buildup (reduces gear stress)
 * - User experience: smooth acceleration feels premium; instant start feels harsh
 * - Safety: Gradual speed increase gives user time to react if load is unbalanced
 * 
 * **Why 500 ms Ramp Time?**
 * - Too fast (< 200 ms): Perceptible jerk, defeats smoothness goal
 * - Too slow (> 1000 ms): User perceives sluggishness, impacts stroke time
 * - 500 ms: Sweet spot confirmed by testing (imperceptible to users, minimal time penalty)
 * 
 * **Why Direction Change Resets Ramp?**
 * - Safety: Prevents high PWM in opposite direction immediately after reversal
 * - Mechanical: Worm gear needs time to overcome backlash on direction change
 * - User experience: Prevents sudden jerk when changing from UP→DOWN or DOWN→UP
 * 
 * **Why Stall Detection (Low PWM Timeout)?**
 * - Detects mechanical binding, limit switch failures, or motor overload
 * - 2 sec timeout: Long enough to avoid false positives during normal ramp-up
 * - MIN_ACTIVE_PWM threshold (10): Below this, motor torque insufficient for movement
 * 
 * @architecture
 * This module is **APPLICATION LAYER** (signal processing logic):
 * - NO hardware dependencies (no GPIO, no timers, no Arduino libs)
 * - Testable on host machine (unit tests without hardware)
 * - Pure algorithm: input (command, target, time) → output (direction, PWM, fault)
 * 
 * Integration:
 * - DeskApp (application logic) → MotorController (signal processing) → HAL (hardware)
 * - DeskApp provides: desired direction, target speed (0-255), current time
 * - MotorController returns: ramped PWM, effective direction, fault status
 * - HAL consumes: PWM value and direction for physical motor driver
 * 
 * @usage_example
 * ```c
 * // Initialization (once at startup)
 * MotorController_init();
 * 
 * // Periodic update (e.g., every 250 ms in main loop)
 * MotorDirection_t desired_dir = MOTOR_UP;
 * uint8_t target_speed = 255; // full speed
 * uint32_t current_time_ms = HAL_get_time_ms();
 * 
 * MotorControllerOutput_t output = MotorController_update(desired_dir, target_speed, current_time_ms);
 * 
 * if (output.fault) {
 *     // Handle fault: transition to error state, stop motor, etc.
 * }
 * 
 * // Pass output to HAL for physical motor control
 * HAL_set_motor_pwm(output.dir, output.pwm);
 * ```
 * 
 * @safety_critical
 * - Direction change reset: Prevents dangerous high-speed reversal
 * - Stall detection: Identifies mechanical failures before damage occurs
 * - Fault flag: Enables application layer to enter safe error state
 * 
 * @test_coverage
 * Unit Tests (tests/UnitTests.cpp):
 * - TC-MC-INIT-001: Initial stopped state
 * - TC-MC-RAMP-001: PWM ramps 0→255 over 500 ms
 * - TC-MC-RAMP-002: Direction change resets ramp
 * - TC-MC-STOP-001: Stop command → PWM=0 immediately
 * - TC-MC-TARGET-001: Variable target PWM (0-255 range)
 * 
 * @version 1.0
 * @date 2026-01-25
 */

#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#include "desk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct MotorControllerOutput_t
 * @brief Output structure containing ramped motor control signals
 * 
 * @field dir - Effective motor direction (MOTOR_UP, MOTOR_DOWN, MOTOR_STOP)
 * @field pwm - Ramped PWM duty cycle (0-255, where 0=stopped, 255=full speed)
 * @field fault - Fault detection flag (true=stall/error detected, false=normal operation)
 * 
 * @notes
 * - PWM value is AFTER ramping (not raw target value)
 * - Direction matches command direction unless stop is commanded
 * - Fault flag triggers when motor stalls (low PWM > 2 sec timeout)
 */
typedef struct
{
    MotorDirection_t dir;  ///< Motor direction command (post-processing)
    uint8_t pwm;           ///< Ramped PWM value (0-255)
    bool fault;            ///< Fault detection: true=stall/error, false=normal
} MotorControllerOutput_t;

/**
 * @brief Initialize motor controller signal processing module
 * 
 * Resets all internal state variables to safe initial conditions:
 * - Last direction: MOTOR_STOP (no motion)
 * - Direction start time: 0 (no active ramp)
 * - Last update time: 0 (no previous updates)
 * - Low PWM start time: 0 (no stall timer active)
 * 
 * @preconditions None (safe to call at any time)
 * @postconditions Motor controller in IDLE state; ready for first update call
 * 
 * @safety_critical
 * Must be called during system initialization before any MotorController_update() calls.
 * Ensures predictable behavior from power-on.
 * 
 * @thread_safety NOT thread-safe (assumes single-threaded main loop)
 */
void MotorController_init(void);

/**
 * @brief Update motor controller with new command and return ramped output
 * 
 * @param cmd_dir - Commanded motor direction (MOTOR_UP, MOTOR_DOWN, MOTOR_STOP)
 * @param target_pwm - Target PWM duty cycle (0-255, where 0=stop, 255=full speed)
 * @param now_ms - Current system time in milliseconds (monotonic, from HAL)
 * 
 * @return MotorControllerOutput_t - Ramped motor control signals (dir, pwm, fault)
 * 
 * @algorithm
 * 1. **Direction Change Detection:**
 *    - If cmd_dir ≠ last_dir: Reset ramp timers (start fresh ramp from 0)
 *    - Rationale: Prevents high PWM in new direction immediately after reversal
 * 
 * 2. **Stop Command Handling:**
 *    - If cmd_dir == MOTOR_STOP: Return PWM=0 immediately (no ramp-down)
 *    - Rationale: Emergency halt requires instant response (SysReq-003: < 500 ms)
 * 
 * 3. **Active Motion Ramping:**
 *    - Calculate elapsed time since direction start: Δt = now_ms - dir_start_time
 *    - Ramp PWM linearly: pwm = (target_pwm × Δt) / RAMP_TIME_MS
 *    - Clamp at target_pwm when Δt ≥ RAMP_TIME_MS (ramp complete)
 * 
 * 4. **Fault Detection (Stall Timeout):**
 *    - If effective_pwm ≤ MIN_ACTIVE_PWM for > STALL_TIMEOUT_MS: Set fault flag
 *    - Rationale: Motor should reach active PWM within ramp time; prolonged low PWM indicates mechanical issue
 * 
 * @requirements
 * - SysReq-006: Smooth motion via PWM ramping (0→255 over 500 ms)
 * - SysReq-003: Immediate halt via MOTOR_STOP (PWM=0 instantly)
 * - SysReq-010: Fault detection via stall timeout (2 sec threshold)
 * 
 * @timing
 * - Ramp duration: 500 ms (RAMP_TIME_MS constant)
 * - Update frequency: Typically 4 Hz (250 ms per DeskApp loop)
 * - Stall timeout: 2000 ms (STALL_TIMEOUT_MS constant)
 * 
 * @safety_critical
 * - Direction change MUST reset ramp to prevent high-speed reversal
 * - MOTOR_STOP MUST return PWM=0 immediately (no delay)
 * - Fault flag MUST be set if motor stalls to enable safe error handling
 * 
 * @thread_safety NOT thread-safe (assumes single-threaded main loop)
 * 
 * @example
 * ```c
 * // Upward motion with ramping
 * MotorControllerOutput_t out1 = MotorController_update(MOTOR_UP, 255, 0);    // t=0: PWM=0 (ramp start)
 * MotorControllerOutput_t out2 = MotorController_update(MOTOR_UP, 255, 250);  // t=250ms: PWM≈127 (halfway)
 * MotorControllerOutput_t out3 = MotorController_update(MOTOR_UP, 255, 500);  // t=500ms: PWM=255 (ramp complete)
 * 
 * // Emergency stop
 * MotorControllerOutput_t out4 = MotorController_update(MOTOR_STOP, 0, 600);  // PWM=0 immediately
 * 
 * // Direction reversal (ramp resets)
 * MotorControllerOutput_t out5 = MotorController_update(MOTOR_DOWN, 255, 700); // t=700ms: PWM=0 (new ramp starts)
 * ```
 */
MotorControllerOutput_t MotorController_update(MotorDirection_t cmd_dir, uint8_t target_pwm, uint32_t now_ms);

#ifdef __cplusplus
}
#endif

#endif // MOTOR_CONTROLLER_H
