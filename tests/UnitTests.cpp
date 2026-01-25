#include <gtest/gtest.h>
#include "motor_controller.h"
#include "hal.h"
#include "desk_types.h"

// ============================================================================
// TEST CASE SPECIFICATION: Motor Controller Unit Tests
// ============================================================================
// PURPOSE: Verify motor controller signal processing (hardware-agnostic)
//
// SCOPE: Tests the motor_controller component which implements:
//   - PWM ramping (soft-start/soft-stop)
//   - Direction control (UP/DOWN/STOP)
//   - State management (stall detection, fault handling)
//
// CLASSIFICATION: Unit Tests (Application Layer)
//   - Tests pure signal processing logic
//   - No hardware (GPIO, timers) exercised
//   - Timing is simulated via mock timer values
//   - Mocked HAL_getTime() provides deterministic timing
// ============================================================================

class MotorControllerUnitTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        MotorController_init();
    }
};

// ============================================================================
// TEST CASE: TC-MC-INIT-001 - Initial State is STOPPED
// ============================================================================
// Requirement: Motor controller must initialize to safe stopped state
//
// Test Objective:
//   Verify that motor controller initializes with motor stopped (PWM=0),
//   direction=STOP, and no fault condition.
//
// Preconditions:
//   - MotorController_init() has been called
//
// Test Steps:
//   1. Call MotorController_update(MOTOR_STOP, 0, current_time)
//   2. Verify output state
//
// Expected Results:
//   - dir: MOTOR_STOP
//   - pwm: 0
//   - fault: false
//
// Rationale:
//   - Ensures safe initial state prevents unintended motor activation
//   - Critical for safety on startup
// ============================================================================
TEST_F(MotorControllerUnitTest, TC_MC_INIT_001_InitialStateIsStopped)
{
    const uint32_t now = HAL_getTime();
    MotorControllerOutput_t out = MotorController_update(MOTOR_STOP, 0U, now);
    
    EXPECT_EQ(out.dir, MOTOR_STOP) 
        << "Initial direction must be STOP";
    EXPECT_EQ(out.pwm, 0U) 
        << "Initial PWM must be zero";
    EXPECT_FALSE(out.fault) 
        << "Initial fault flag must be false";
}

// ============================================================================
// TEST CASE: TC-MC-RAMP-001 - PWM Ramps Up Over Time (Soft-Start)
// ============================================================================
// Requirement: Motor controller must implement gradual PWM ramp for soft-start
//
// Test Objective:
//   Verify that PWM increases gradually from 0 to target_pwm over a fixed
//   ramp time (500ms), providing smooth motor acceleration.
//
// Preconditions:
//   - MotorController_init() called
//   - Motor is in STOP state
//
// Test Steps:
//   1. At t=0, call update(MOTOR_UP, 255, 0)
//      → PWM should be 0 (start of ramp)
//   2. At t=250ms (50% of ramp), call update(MOTOR_UP, 255, 250)
//      → PWM should be ~127 (50% of target)
//   3. At t=500ms (100% of ramp), call update(MOTOR_UP, 255, 500)
//      → PWM should equal 255 (target reached)
//
// Expected Results:
//   - At t=0: PWM = 0
//   - At t=250: 120 ≤ PWM ≤ 135 (allowing ±5% tolerance)
//   - At t=500: PWM = 255
//   - Direction maintained as MOTOR_UP throughout
//
// Rationale:
//   - Soft-start reduces mechanical stress and inrush current
//   - Gradual acceleration protects mechanical components
//   - Ramp time tolerance allows implementation flexibility
// ============================================================================
TEST_F(MotorControllerUnitTest, TC_MC_RAMP_001_PWMRampsUpOverTime)
{
    MotorController_init();
    const uint32_t start_time = 0U;
    
    // At t=0, PWM should be 0 (start of ramp)
    MotorControllerOutput_t out1 = MotorController_update(MOTOR_UP, 255U, start_time);
    EXPECT_EQ(out1.dir, MOTOR_UP) 
        << "Direction must be UP";
    EXPECT_EQ(out1.pwm, 0U) 
        << "PWM must be zero at start of ramp";
    
    // At t=250ms (50% of 500ms ramp), PWM should be ~50% of target
    MotorControllerOutput_t out2 = MotorController_update(MOTOR_UP, 255U, start_time + 250U);
    EXPECT_EQ(out2.dir, MOTOR_UP) 
        << "Direction must remain UP";
    EXPECT_GE(out2.pwm, 120U) 
        << "PWM at 50% ramp time should be ≥120 (47%)";
    EXPECT_LE(out2.pwm, 135U) 
        << "PWM at 50% ramp time should be ≤135 (53%)";
    
    // At t=500ms (end of ramp), PWM should equal target
    MotorControllerOutput_t out3 = MotorController_update(MOTOR_UP, 255U, start_time + 500U);
    EXPECT_EQ(out3.dir, MOTOR_UP) 
        << "Direction must remain UP";
    EXPECT_EQ(out3.pwm, 255U) 
        << "PWM must reach target value at end of ramp";
}

// ============================================================================
// TEST CASE: TC-MC-RAMP-002 - Direction Change Resets Ramp
// ============================================================================
// Requirement: Changing direction must reset PWM ramp timer
//
// Test Objective:
//   Verify that when motor direction changes (UP→DOWN or vice versa),
//   the PWM ramp timer resets and begins new ramp from 0.
//
// Preconditions:
//   - Motor initialized in STOP state
//
// Test Steps:
//   1. At t=0, command MOTOR_UP with PWM=255
//      → Motor ramps up to full speed
//   2. At t=500ms, verify PWM=255 (ramp complete)
//   3. At t=510ms, command MOTOR_DOWN with PWM=255
//      → Ramp timer should reset, PWM should go back to 0
//   4. Verify direction changed to DOWN, PWM reset to 0
//
// Expected Results:
//   - After direction change: dir=MOTOR_DOWN, pwm=0 (ramp restarted)
//   - Previous ramp progress discarded
//   - New ramp begins from 0 with new direction
//
// Rationale:
//   - Prevents high PWM being applied to opposite direction
//   - Ensures smooth transition between directions
//   - Safety: avoids applying full power in unintended direction
// ============================================================================
TEST_F(MotorControllerUnitTest, TC_MC_RAMP_002_DirectionChangeResetsRamp)
{
    const uint32_t start_time = 0U;
    
    // Step 1-2: Ramp up to full speed
    MotorController_update(MOTOR_UP, 255U, start_time);
    MotorControllerOutput_t out1 = MotorController_update(MOTOR_UP, 255U, start_time + 500U);
    EXPECT_EQ(out1.pwm, 255U) 
        << "PWM should reach target after ramp completes";
    EXPECT_EQ(out1.dir, MOTOR_UP) 
        << "Direction should be UP";
    
    // Step 3-4: Change direction and verify ramp resets
    MotorControllerOutput_t out2 = MotorController_update(MOTOR_DOWN, 255U, start_time + 510U);
    EXPECT_EQ(out2.dir, MOTOR_DOWN) 
        << "Direction must change to DOWN";
    EXPECT_EQ(out2.pwm, 0U) 
        << "PWM must reset to 0 when direction changes (ramp restarted)";
}

// ============================================================================
// TEST CASE: TC-MC-STOP-001 - Stop Command Immediately Sets PWM to Zero
// ============================================================================
// Requirement: STOP command must immediately halt motor (emergency stop behavior)
//
// Test Objective:
//   Verify that issuing MOTOR_STOP command immediately sets PWM to 0,
//   regardless of previous speed or ramp state.
//
// Preconditions:
//   - Motor ramped to partial or full speed
//
// Test Steps:
//   1. At t=0, command MOTOR_UP with PWM=255
//      → Motor begins ramping up
//   2. At t=500ms, command MOTOR_STOP
//      → PWM should immediately become 0 (no soft-stop ramp)
//   3. Verify motor halted
//
// Expected Results:
//   - Motor command: MOTOR_STOP
//   - PWM: 0 (immediate stop, not gradual)
//   - No soft-stop ramp applied
//
// Rationale:
//   - STOP provides immediate halt capability
//   - May represent emergency stop or safety shutdown
//   - No gradual deceleration on STOP (binary command)
// ============================================================================
TEST_F(MotorControllerUnitTest, TC_MC_STOP_001_StopCommandSetsZeroPWM)
{
    const uint32_t now = HAL_getTime();
    
    // Start moving motor
    MotorController_update(MOTOR_UP, 255U, now);
    
    // Issue stop command
    MotorControllerOutput_t out = MotorController_update(MOTOR_STOP, 0U, now + 500U);
    
    EXPECT_EQ(out.dir, MOTOR_STOP) 
        << "Direction must be STOP";
    EXPECT_EQ(out.pwm, 0U) 
        << "PWM must be zero immediately on STOP";
    EXPECT_FALSE(out.fault) 
        << "No fault should occur from normal stop";
}

// ============================================================================
// TEST CASE: TC-MC-TARGET-001 - Varying Target PWM Values
// ============================================================================
// Requirement: Motor controller must support variable PWM targets (0-255)
//
// Test Objective:
//   Verify that different target PWM values are ramped correctly,
//   allowing variable speed control (not just full on/off).
//
// Preconditions:
//   - Motor initialized
//
// Test Steps:
//   1. Command MOTOR_UP with target_pwm=127 (50% speed)
//   2. At t=0: PWM should be 0
//   3. At t=500ms: PWM should be ~127
//   4. Verify ramp tracks target correctly
//
// Expected Results:
//   - PWM ramps from 0 to 127 (not to 255)
//   - Ramp slope matches target value
//   - Target PWM maintained after ramp completes
//
// Rationale:
//   - Allows smooth speed control independent of direction
//   - Enables graduated acceleration/deceleration
//   - Supports efficiency optimization
// ============================================================================
TEST_F(MotorControllerUnitTest, TC_MC_TARGET_001_VaryingTargetPWMValues)
{
    MotorController_init();
    const uint32_t start_time = 0U;
    const uint8_t target_pwm = 127U;  // 50% power
    
    // At start of ramp
    MotorControllerOutput_t out1 = MotorController_update(MOTOR_UP, target_pwm, start_time);
    EXPECT_EQ(out1.pwm, 0U) 
        << "PWM should start at 0 regardless of target";
    
    // At end of ramp
    MotorControllerOutput_t out2 = MotorController_update(MOTOR_UP, target_pwm, start_time + 500U);
    EXPECT_EQ(out2.pwm, target_pwm) 
        << "PWM should ramp to target value (127)";
    EXPECT_EQ(out2.dir, MOTOR_UP) 
        << "Direction should be UP";
}
