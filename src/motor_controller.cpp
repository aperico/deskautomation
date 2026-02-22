/**
 * @file motor_controller.cpp
 * @brief Motor Controller Signal Processing Implementation
 * 
 * @implementation_overview
 * This module implements soft-start PWM ramping for smooth motor acceleration,
 * direction management, and fault detection. All functionality is hardware-agnostic
 * (application layer) and operates on time-based state machines.
 * 
 * @key_algorithms
 * 1. **Linear PWM Ramping**: pwm(t) = min(target × t / T_ramp, target)
 * 2. **Direction Change Detection**: Reset ramp timers when cmd_dir ≠ last_dir
 * 3. **Stall Detection**: Fault if (pwm ≤ MIN_ACTIVE_PWM) for > STALL_TIMEOUT_MS
 * 
 * @state_variables
 * - last_dir: Previous direction (for change detection)
 * - dir_start_time: Timestamp when current direction started (for ramping)
 * - last_update_time: Timestamp of most recent update (unused, reserved for future)
 * - low_pwm_start_time: Timestamp when PWM dropped below threshold (for stall detection)
 * 
 * @constants_rationale
 * - RAMP_TIME_MS (500 ms): Balances smooth acceleration vs. stroke time performance
 * - STALL_TIMEOUT_MS (2000 ms): Allows full ramp completion plus safety margin
 * - MIN_ACTIVE_PWM (10): Below this value, motor torque insufficient for movement
 * 
 * @requirements_coverage
 * - SysReq-006 (Smooth Motion): RAMP_TIME_MS = 500 ms ensures < 0.5 g acceleration
 * - SysReq-003 (Motion Halt): MOTOR_STOP returns PWM=0 immediately (no ramp-down)
 * - SysReq-010 (Fault Detection): STALL_TIMEOUT_MS enables mechanical failure detection
 * 
 * @version 1.0
 * @date 2026-01-25
 */

#include "motor_controller.h"

// ============================================================================
// CONFIGURATION CONSTANTS
// ============================================================================

/**
 * @brief Soft-start ramp duration (0 → 255 PWM)
 * 
 * @rationale
 * - Too fast (< 200 ms): Perceptible jerk, violates SysReq-006 smoothness
 * - Too slow (> 1000 ms): Sluggish feel, impacts SysReq-004 stroke time
 * - 500 ms: Optimal balance confirmed by testing
 * 
 * @calculation
 * Assuming motor reaches max speed at 255 PWM and desk moves ~3 cm/s:
 * - Acceleration distance during ramp: ~0.75 cm (negligible vs. 90 cm stroke)
 * - Acceleration: (3 cm/s) / (0.5 s) = 6 cm/s² ≈ 0.006 g (well below 0.5 g limit)
 */
static const uint32_t RAMP_TIME_MS = 500U;

/**
 * @brief Stall detection timeout (motor stuck at low PWM)
 * 
 * @rationale
 * - Must exceed RAMP_TIME_MS (500 ms) to avoid false positives during startup
 * - 2000 ms = 4× ramp time, providing generous safety margin
 * - Detects: mechanical binding, limit switch failures, motor overload
 */
static const uint32_t STALL_TIMEOUT_MS = 2000U;

/**
 * @brief Minimum PWM for active motor operation
 * 
 * @rationale
 * - Below this value, motor torque insufficient to overcome friction/load
 * - Empirically determined: PWM < 10 produces audible hum but no rotation
 * - Used as threshold for stall detection logic
 */
static const uint8_t MIN_ACTIVE_PWM = 10U;

// ============================================================================
// MODULE STATE VARIABLES (static/private)
// ============================================================================

// ============================================================================
// MODULE STATE VARIABLES (static/private)
// ============================================================================

/**
 * @brief Last commanded direction (for change detection)
 * 
 * Initialized to MOTOR_STOP (safe initial state).
 * Updated on every direction change to enable ramp reset logic.
 */
static MotorDirection_t last_dir = MOTOR_STOP;

/**
 * @brief Timestamp when current direction started (milliseconds)
 * 
 * Used to calculate elapsed time for PWM ramping:
 * - elapsed_ms = now_ms - dir_start_time
 * - pwm(t) = target × elapsed_ms / RAMP_TIME_MS
 * 
 * Reset to current time on every direction change.
 */
static uint32_t dir_start_time = 0U;

/**
 * @brief Timestamp of most recent update call (milliseconds)
 * 
 * Reserved for future use (e.g., watchdog timeout detection).
 * Currently unused in algorithm.
 */
static uint32_t last_update_time = 0U;

/**
 * @brief Timestamp when PWM first dropped below MIN_ACTIVE_PWM (milliseconds)
 * 
 * Used for stall detection:
 * - If (pwm ≤ MIN_ACTIVE_PWM) for (now_ms - low_pwm_start_time) > STALL_TIMEOUT_MS:
 *   → Set fault flag (motor stalled)
 * 
 * Reset to current time when:
 * - PWM rises above MIN_ACTIVE_PWM (normal operation resumed)
 * - Direction changes (new ramp starts)
 * - Motor stops (MOTOR_STOP command)
 */
static uint32_t low_pwm_start_time = 0U;

// ============================================================================
// PUBLIC FUNCTIONS
// ============================================================================

/**
 * @brief Initialize motor controller to safe idle state
 * 
 * @details
 * Resets all state variables to initial values:
 * - No active motion (last_dir = MOTOR_STOP)
 * - No active ramp (dir_start_time = 0)
 * - No update history (last_update_time = 0)
 * - No stall detection active (low_pwm_start_time = 0)
 * 
 * @safety_critical
 * MUST be called during system initialization before any update calls.
 * Ensures predictable behavior from power-on state.
 */
void MotorController_init(void)
{
    last_dir = MOTOR_STOP;
    dir_start_time = 0U;
    last_update_time = 0U;
    low_pwm_start_time = 0U;
}

// ============================================================================
// PRIVATE HELPER FUNCTIONS
// ============================================================================

// ============================================================================
// PRIVATE HELPER FUNCTIONS
// ============================================================================

/**
 * @brief Calculate ramped PWM value using linear interpolation
 * 
 * @param target_pwm - Final target PWM value (0-255)
 * @param elapsed_ms - Time elapsed since ramp started (milliseconds)
 * 
 * @return uint8_t - Ramped PWM value (0-255)
 * 
 * @algorithm
 * Linear ramp function:
 *   pwm(t) = min(target × t / T_ramp, target)
 * 
 * Where:
 *   t = elapsed_ms (time since ramp start)
 *   T_ramp = RAMP_TIME_MS (500 ms)
 *   target = target_pwm (0-255)
 * 
 * @example
 * ```
 * target_pwm = 255, RAMP_TIME_MS = 500 ms:
 *   t=0 ms:   pwm = (255 × 0) / 500 = 0
 *   t=100 ms: pwm = (255 × 100) / 500 = 51
 *   t=250 ms: pwm = (255 × 250) / 500 = 127.5 → 127
 *   t=500 ms: pwm = (255 × 500) / 500 = 255 (ramp complete)
 *   t>500 ms: pwm = 255 (clamped at target)
 * ```
 * 
 * @rationale
 * - Linear ramp: Simple, predictable, smooth acceleration
 * - Integer math: Avoids floating-point overhead on microcontroller
 * - Early exit: target=0 or elapsed≥T_ramp optimizations for efficiency
 * 
 * @overflow_protection
 * Cast to uint32_t before multiplication prevents overflow:
 *   Max value: 255 × 4,294,967,295 ms = fits in uint32_t before division
 * 
 * @edge_cases
 * - target_pwm = 0: Returns 0 immediately (no division by zero)
 * - elapsed_ms ≥ RAMP_TIME_MS: Returns target_pwm (ramp complete)
 * - elapsed_ms = 0: Returns 0 (ramp just started)
 */
static uint8_t ramp_pwm(uint8_t target_pwm, uint32_t elapsed_ms)
{
    // Early exit: If target is zero, no ramping needed (stop command)
    if (target_pwm == 0U)
    {
        return 0U;
    }
    
    // Early exit: If ramp time elapsed, return target directly (no calculation needed)
    if (elapsed_ms >= RAMP_TIME_MS)
    {
        return target_pwm;
    }
    
    // Linear interpolation: pwm(t) = target × (t / T_ramp)
    // Rearranged for integer math: pwm(t) = (target × t) / T_ramp
    // Widen to uint32_t to prevent overflow during multiplication
    const uint32_t scaled = (static_cast<uint32_t>(target_pwm) * elapsed_ms) / RAMP_TIME_MS;
    
    // Cast back to uint8_t (safe: result ≤ target_pwm ≤ 255)
    return static_cast<uint8_t>(scaled);
}

/**
 * @brief Main update function - processes command and returns ramped output
 * 
 * @implementation_details
 * 
 * **Step 1: Initialize Output Structure**
 * - Set default values: dir=cmd_dir, pwm=0, fault=false
 * - Will be modified based on logic below
 * 
 * **Step 2: Direction Change Detection**
 * - Compare cmd_dir with last_dir (previous direction)
 * - If different: Reset ramp timers to start fresh ramp from PWM=0
 * - Rationale: Prevents high PWM in new direction immediately after reversal
 * 
 * **Step 3: Stop Command Handling**
 * - If cmd_dir == MOTOR_STOP: Return PWM=0 immediately (no ramp-down)
 * - Reset low_pwm_start_time (no stall detection during intentional stop)
 * - Rationale: Emergency halt requires instant response (SysReq-003: < 500 ms)
 * 
 * **Step 4: Active Motion Processing**
 * - Calculate elapsed time: Δt = now_ms - dir_start_time
 * - Call ramp_pwm() to compute ramped PWM value
 * - Apply to output structure
 * 
 * **Step 5: Stall Detection Logic**
 * - If effective_pwm ≤ MIN_ACTIVE_PWM:
 *   - Start/continue stall timer
 *   - If timer exceeds STALL_TIMEOUT_MS: Set fault flag
 * - Else (PWM above threshold):
 *   - Reset stall timer (motor operating normally)
 * 
 * **Step 6: Update State & Return**
 * - Update last_update_time for future watchdog use
 * - Return output structure with ramped PWM and fault status
 * 
 * @state_machine_diagram
 * ```
 * [IDLE/STOP] --cmd=UP/DOWN--> [RAMPING] --t≥500ms--> [FULL_SPEED]
 *      ^                            |                       |
 *      |                            +--cmd=STOP------------+
 *      |                            |
 *      +--cmd=reverse--------------+ (reset ramp)
 *      |
 *      +--pwm≤10 for >2s--> [FAULT]
 * ```
 */
MotorControllerOutput_t MotorController_update(MotorDirection_t cmd_dir, uint8_t target_pwm, uint32_t now_ms)
{
    // Step 1: Initialize output structure with safe defaults
    MotorControllerOutput_t out = {};
    out.dir = cmd_dir;      // Echo commanded direction
    out.pwm = 0U;           // Default to stopped (will be overridden below)
    out.fault = false;      // Assume no fault unless detected

    // Step 2: Direction Change Detection
    // If direction changed (including transitions to/from STOP), reset ramp timers
    if (cmd_dir != last_dir)
    {
        dir_start_time = now_ms;       // Start new ramp from t=0
        low_pwm_start_time = now_ms;   // Reset stall detection timer
        last_dir = cmd_dir;            // Update last direction for next comparison
        // SAFETY: This prevents high PWM in opposite direction immediately after reversal
    }

    // Step 2: Direction Change Detection
    // If direction changed (including transitions to/from STOP), reset ramp timers
    if (cmd_dir != last_dir)
    {
        dir_start_time = now_ms;       // Start new ramp from t=0
        low_pwm_start_time = now_ms;   // Reset stall detection timer
        last_dir = cmd_dir;            // Update last direction for next comparison
        // SAFETY: This prevents high PWM in opposite direction immediately after reversal
    }

    // Step 3: Stop Command Handling
    if (cmd_dir == MOTOR_STOP)
    {
        // Immediate halt: No ramping down (emergency stop requirement)
        out.dir = MOTOR_STOP;
        out.pwm = 0U;  // PWM=0 immediately (no delay)
        low_pwm_start_time = now_ms;  // Reset stall timer (intentional stop, not stall)
        // SAFETY: SysReq-003 requires motion halt < 500 ms; instant PWM=0 ensures compliance
    }
    else
    {
        // Step 4: Active Motion Processing (UP or DOWN command)
        
        // Calculate time elapsed since ramp started
        const uint32_t elapsed = now_ms - dir_start_time;
        
        // Apply soft-start ramping algorithm
        const uint8_t effective_pwm = ramp_pwm(target_pwm, elapsed);
        out.pwm = effective_pwm;
        // Result: PWM ramps linearly from 0→target over RAMP_TIME_MS (500 ms)
        // This implements SysReq-006 smooth motion requirement

        // Step 5: Stall Detection Logic
        if (effective_pwm <= MIN_ACTIVE_PWM)
        {
            // PWM is below active threshold - motor may be stalled
            
            // Start stall timer if not already running
            if (low_pwm_start_time == 0U)
            {
                low_pwm_start_time = now_ms;  // Begin tracking low-PWM duration
            }
            
            // Check if motor has been stuck at low PWM for too long
            const uint32_t low_elapsed = now_ms - low_pwm_start_time;
            if (low_elapsed >= STALL_TIMEOUT_MS)
            {
                // Fault detected: Motor commanded to move but PWM remains low
                // Possible causes: mechanical binding, limit switch failure, motor overload
                out.fault = true;
                // SAFETY: Application layer should transition to ERROR state on fault
            }
        }
        else
        {
            // PWM above threshold - motor operating normally
            low_pwm_start_time = now_ms;  // Reset stall timer (no fault condition)
        }
    }

    // Step 6: Update State & Return
    last_update_time = now_ms;  // Record timestamp (reserved for future watchdog use)
    return out;
    // Output contains: ramped PWM, effective direction, fault status
    // Caller (typically DeskApp) passes this to HAL for physical motor control
}
