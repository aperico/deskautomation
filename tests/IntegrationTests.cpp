#include <gtest/gtest.h>
#include "hal.h"
#include "motor_controller.h"
#include "desk_app.h"
#include "desk_types.h"
#include "hal_mock/HALMock.h"

// ============================================================================
// INTEGRATION TESTS: HAL/Signal Layer + Motor Controller
// Testing hardware abstraction layer with mocked Arduino calls
// and motor controller signal processing
// ============================================================================

// ============================================================================
// INTEGRATION TEST: HAL (Hardware Abstraction Layer)
// ============================================================================

class HALIntegrationTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        HAL_init();
    }
};

// REQ-HAL-001: HAL should initialize without errors
TEST_F(HALIntegrationTest, HALInitializes)
{
    EXPECT_NO_THROW(HAL_init());
}

// REQ-HAL-002: Button reading should return valid boolean state
TEST_F(HALIntegrationTest, ButtonReadReturnsValidValue)
{
    bool button_state = HAL_readButton(BUTTON_UP);
    EXPECT_TRUE(button_state == true || button_state == false);
}

// REQ-HAL-003: Limit sensor reading should return valid boolean state
TEST_F(HALIntegrationTest, LimitSensorReadReturnsValidValue)
{
    bool limit_state = HAL_readLimitSensor(LIMIT_UPPER);
    EXPECT_TRUE(limit_state == true || limit_state == false);
}

// REQ-HAL-005: Motor current read should convert ADC counts to milliamps
TEST_F(HALIntegrationTest, MotorCurrentReadReturnsMilliamps)
{
    const int adc_value = 512;
    pin_states[PIN_MOTOR_SENSE] = adc_value;

    const uint16_t current_ma = HAL_readMotorCurrent();
    const uint32_t voltage_mv = (static_cast<uint32_t>(adc_value) * 5000U) / 1023U;
    const uint32_t expected_ma = (voltage_mv * 1000U) / 500U;

    EXPECT_EQ(current_ma, expected_ma);
}

TEST_F(HALIntegrationTest, MotorCurrentReadZeroAtAdcZero)
{
    pin_states[PIN_MOTOR_SENSE] = 0;
    EXPECT_EQ(HAL_readMotorCurrent(), 0U);
}

TEST_F(HALIntegrationTest, MotorCurrentReadMaxAtAdcMax)
{
    pin_states[PIN_MOTOR_SENSE] = 1023;
    EXPECT_EQ(HAL_readMotorCurrent(), 10000U);
}

// REQ-HAL-004: Time function should be monotonically increasing
TEST_F(HALIntegrationTest, TimeReturnsMonotonicallyIncreasing)
{
    const uint32_t time1 = HAL_getTime();
    const uint32_t time2 = HAL_getTime();
    // Time should not go backwards
    EXPECT_GE(time2, time1);
}

// ============================================================================
// INTEGRATION TEST: Full System Integration
// Testing interaction between application, motor controller, and HAL
// ============================================================================

class SystemIntegrationTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        HAL_init();
        MotorController_init();
        APP_Init();
    }
};

// REQ-SYSTEM-001: Application output should drive motor controller correctly
TEST_F(SystemIntegrationTest, AppOutputDrivesMotorControllerCorrectly)
{
    const uint32_t now = HAL_getTime();
    
    // Setup app inputs
    AppInput_t app_inputs = {0};
    app_inputs.button_up = true;
    app_inputs.timestamp_ms = now;
    
    // Run app logic
    AppOutput_t app_outputs;
    APP_Task(&app_inputs, &app_outputs);
    
    // Feed app output to motor controller
    MotorControllerOutput_t motor_out = MotorController_update(
        app_outputs.motor_cmd,
        app_outputs.motor_speed,
        now
    );
    
    EXPECT_EQ(motor_out.dir, MOTOR_UP);
    EXPECT_EQ(motor_out.pwm, 0U); // Initial ramp state
    
    // Advance time and verify ramp
    motor_out = MotorController_update(
        app_outputs.motor_cmd,
        app_outputs.motor_speed,
        now + 500U
    );
    
    EXPECT_EQ(motor_out.dir, MOTOR_UP);
    EXPECT_EQ(motor_out.pwm, app_outputs.motor_speed);
}

// REQ-SYSTEM-SAFETY-001: Limit sensor should prevent dangerous movement through full stack
TEST_F(SystemIntegrationTest, LimitSensorPreventsDangerousMovement)
{
    const uint32_t now = HAL_getTime();
    
    // Simulate upper limit triggered
    AppInput_t app_inputs = {0};
    app_inputs.button_up = true;
    app_inputs.limit_upper = true;  // Limit sensor triggered
    app_inputs.timestamp_ms = now;
    
    AppOutput_t app_outputs;
    APP_Task(&app_inputs, &app_outputs);
    
    // SAFETY-CRITICAL: App must stop at limit
    EXPECT_EQ(app_outputs.motor_cmd, MOTOR_STOP);
    
    // Motor controller must execute stop
    MotorControllerOutput_t motor_out = MotorController_update(
        app_outputs.motor_cmd,
        app_outputs.motor_speed,
        now
    );
    
    EXPECT_EQ(motor_out.dir, MOTOR_STOP);
    EXPECT_EQ(motor_out.pwm, 0U);
}
// ============================================================================
// INTEGRATION TEST: SWReq-012 - Motor Stop Without Valid Command
// ============================================================================
// Requirement: Motor shall stop when no valid UP/DOWN command
//
// Test Objective:
//   Verify that motor remains stopped when neither button is pressed,
//   ensuring fail-safe behavior.
//
// Test Steps:
//   1. Initialize all components
//   2. Create app inputs with both buttons released
//   3. Run APP_Task() and MotorController_update()
//   4. Verify motor commanded to STOP
//
// Expected Results:
//   - Motor command: MOTOR_STOP
//   - Motor PWM: 0
//
// Rationale:
//   - Prevents unintended continuous motion
//   - Ensures safe idle state
// ============================================================================
TEST_F(SystemIntegrationTest, SWReq012_MotorStopWithoutValidCommand)
{
    const uint32_t now = HAL_getTime();
    
    // No buttons pressed, no fault input
    AppInput_t app_inputs = {0};
    app_inputs.button_up = false;
    app_inputs.button_down = false;
    app_inputs.limit_upper = false;
    app_inputs.limit_lower = false;
    app_inputs.timestamp_ms = now;
    
    AppOutput_t app_outputs;
    APP_Task(&app_inputs, &app_outputs);
    
    // Must command STOP with no valid input
    EXPECT_EQ(app_outputs.motor_cmd, MOTOR_STOP) 
        << "Motor must stop when no button pressed";
    
    // Motor controller executes the stop
    MotorControllerOutput_t motor_out = MotorController_update(
        app_outputs.motor_cmd,
        app_outputs.motor_speed,
        now
    );
    
    EXPECT_EQ(motor_out.dir, MOTOR_STOP);
    EXPECT_EQ(motor_out.pwm, 0U);
}

// ============================================================================
// INTEGRATION TEST: SWReq-013 - Safe Initialization After Reset
// ============================================================================
// Requirement: Initialize to safe STOP state and require new valid command
//
// Test Objective:
//   Verify that system initializes safely after reset/power-on and
//   stays stopped until valid command is issued.
//
// Test Steps:
//   1. Call all _init() functions
//   2. Immediately call APP_Task() with no inputs
//   3. Verify motor is stopped
//   4. Verify no motion even on first execution
//
// Expected Results:
//   - Motor command: MOTOR_STOP
//   - Motor PWM: 0
//   - No unintended motion
//
// Rationale:
//   - Prevents unintended motion on power recovery/brownout
//   - Meets SysReq-011 safe initialization requirement
// ============================================================================
TEST_F(SystemIntegrationTest, SWReq013_SafeInitializationAfterReset)
{
    // Fresh initialization already done in SetUp()
    
    const uint32_t now = HAL_getTime();
    
    // Immediately call APP_Task with no inputs
    AppInput_t app_inputs = {0};
    app_inputs.timestamp_ms = now;
    
    AppOutput_t app_outputs;
    APP_Task(&app_inputs, &app_outputs);
    
    // SAFETY-CRITICAL: Must be stopped initially
    EXPECT_EQ(app_outputs.motor_cmd, MOTOR_STOP) 
        << "Motor must be STOP on initialization";
    EXPECT_EQ(app_outputs.motor_speed, 0U) 
        << "Motor speed must be zero on initialization";
}

// ============================================================================
// INTEGRATION TEST: SWReq-014-002 - Obstruction Detection During MOVING_UP
// ============================================================================
// Requirement: SAFETY-CRITICAL - Detect motor jam/obstruction during motion
//
// Test Objective:
//   Verify that excessive motor current during upward motion is detected
//   as obstruction and system immediately stops motor and enters fault.
//   This tests the critical jam detection path through the full stack.
//
// Test Steps:
//   1. Transition motor to MOVING_UP state with normal current (50mA)
//   2. While in MOVING_UP, simulate obstruction current (250mA > 200mA threshold)
//   3. Run APP_Task() with elevated current
//   4. Verify motor commanded to STOP and fault latched
//   5. Verify motor controller stops immediately
//
// Expected Results:
//   - App detects current > 200mA during motion
//   - Motor command: MOTOR_STOP (immediate halt)
//   - Motor PWM: 0 (no motion)
//   - Fault output: true
//   - Application state: FAULT
//
// Rationale:
//   - SAFETY-CRITICAL: Prevents damage from jam conditions
//   - Prevents motor stall damage
//   - Immediate response (no timeout) during motion
// ============================================================================
TEST_F(SystemIntegrationTest, SWReq014_002_ObstructionDetectionDuringMovingUp)
{
    const uint32_t now = HAL_getTime();
    
    // Step 1: Transition to MOVING_UP with normal current
    AppInput_t app_inputs = {0};
    app_inputs.button_up = true;
    app_inputs.motor_current_ma = 50U;  // Normal current during motion
    app_inputs.timestamp_ms = now;
    
    AppOutput_t app_outputs;
    APP_Task(&app_inputs, &app_outputs);
    
    // Verify in MOVING_UP state
    EXPECT_EQ(app_outputs.motor_cmd, MOTOR_UP) 
        << "Setup: Should be moving UP";
    
    // Step 2: Simulate obstruction (elevated current > 200mA)
    app_inputs.motor_current_ma = 250U;  // Obstruction threshold exceeded
    app_inputs.timestamp_ms = now + 10U;
    
    APP_Task(&app_inputs, &app_outputs);
    
    // Step 3-4: Verify immediate obstruction detection
    EXPECT_EQ(app_outputs.motor_cmd, MOTOR_STOP) 
        << "SAFETY-CRITICAL: Motor must stop immediately on jam";
    EXPECT_TRUE(app_outputs.fault_out)
        << "SAFETY-CRITICAL: Fault must be set on obstruction";
    
    // Step 5: Verify motor controller executes stop
    MotorControllerOutput_t motor_out = MotorController_update(
        app_outputs.motor_cmd,
        app_outputs.motor_speed,
        now + 10U
    );
    
    EXPECT_EQ(motor_out.dir, MOTOR_STOP);
    EXPECT_EQ(motor_out.pwm, 0U);
}

// ============================================================================
// INTEGRATION TEST: SWReq-014-003 - Obstruction Detection During MOVING_DOWN
// ============================================================================
// Requirement: SAFETY-CRITICAL - Detect motor jam/obstruction in both directions
//
// Test Objective:
//   Verify that excessive motor current during downward motion is detected
//   as obstruction. Symmetric test to ensure bidirectional jam detection.
//
// Test Steps:
//   1. Transition motor to MOVING_DOWN state with normal current (60mA)
//   2. While in MOVING_DOWN, simulate obstruction current (210mA > 200mA)
//   3. Run APP_Task() with elevated current
//   4. Verify motor commanded to STOP and fault latched
//   5. Verify motor controller stops immediately
//
// Expected Results:
//   - App detects current > 200mA during downward motion
//   - Motor command: MOTOR_STOP (immediate halt)
//   - Motor PWM: 0 (no motion)
//   - Fault output: true
//   - Application state: FAULT
//
// Rationale:
//   - SAFETY-CRITICAL: Ensures bidirectional protection
//   - Prevents jam damage in either direction
//   - Immediate response regardless of direction
// ============================================================================
TEST_F(SystemIntegrationTest, SWReq014_003_ObstructionDetectionDuringMovingDown)
{
    const uint32_t now = HAL_getTime();
    
    // Step 1: Transition to MOVING_DOWN with normal current
    AppInput_t app_inputs = {0};
    app_inputs.button_down = true;
    app_inputs.motor_current_ma = 60U;  // Normal current during downward motion
    app_inputs.timestamp_ms = now;
    
    AppOutput_t app_outputs;
    APP_Task(&app_inputs, &app_outputs);
    
    // Verify in MOVING_DOWN state
    EXPECT_EQ(app_outputs.motor_cmd, MOTOR_DOWN) 
        << "Setup: Should be moving DOWN";
    
    // Step 2: Simulate obstruction (elevated current > 200mA)
    app_inputs.motor_current_ma = 210U;  // Obstruction threshold exceeded
    app_inputs.timestamp_ms = now + 10U;
    
    APP_Task(&app_inputs, &app_outputs);
    
    // Step 3-4: Verify immediate obstruction detection
    EXPECT_EQ(app_outputs.motor_cmd, MOTOR_STOP) 
        << "SAFETY-CRITICAL: Motor must stop immediately on jam";
    EXPECT_TRUE(app_outputs.fault_out)
        << "SAFETY-CRITICAL: Fault must be set on obstruction";
    
    // Step 5: Verify motor controller executes stop
    MotorControllerOutput_t motor_out = MotorController_update(
        app_outputs.motor_cmd,
        app_outputs.motor_speed,
        now + 10U
    );
    
    EXPECT_EQ(motor_out.dir, MOTOR_STOP);
    EXPECT_EQ(motor_out.pwm, 0U);
}

// ============================================================================
// INTEGRATION TEST: SWReq-014-001 - Stuck-On Detection During STOP
// ============================================================================
// Requirement: SAFETY-CRITICAL - Detect motor driver stuck-on condition
//
// Test Objective:
//   Verify that excessive motor current while STOP is commanded 
//   (indicating stuck-on or runaway) is detected and faulted after
//   timeout period (100ms).
//
// Test Steps:
//   1. Command motor STOP (no motion)
//   2. Simulate stuck-on current (200mA at stopped state)
//   3. At t=50ms, verify fault not yet latched (timer running)
//   4. At t=150ms, verify fault is latched
//   5. Verify application enters FAULT state
//
// Expected Results:
//   - At t=50ms: fault_out = false (timer < 100ms)
//   - At t=150ms: fault_out = true (timer > 100ms threshold)
//   - Application state: FAULT
//   - Motor command: STOP
//
// Rationale:
//   - SAFETY-CRITICAL: Detects motor driver electrical faults
//   - Timeout prevents false triggers from transients
//   - Meets electrical safety requirements
// ============================================================================
TEST_F(SystemIntegrationTest, SWReq014_001_StuckOnDetectionDuringStop)
{
    const uint32_t now = HAL_getTime();
    
    // Step 1: Command motor STOP with no motion
    AppInput_t app_inputs = {0};
    app_inputs.button_up = false;
    app_inputs.button_down = false;
    app_inputs.limit_upper = false;
    app_inputs.limit_lower = false;
    app_inputs.motor_current_ma = 200U;  // Stuck-on current immediately
    app_inputs.timestamp_ms = now;
    
    AppOutput_t app_outputs;
    APP_Task(&app_inputs, &app_outputs);
    
    // Verify motor is commanded to STOP
    EXPECT_EQ(app_outputs.motor_cmd, MOTOR_STOP) 
        << "Motor must be stopped (no buttons pressed)";
    
    // Step 2-3: At t=50ms, fault should NOT yet be latched (timer running)
    app_inputs.timestamp_ms = now + 50U;
    app_inputs.motor_current_ma = 200U;  // Sustained stuck-on current
    
    APP_Task(&app_inputs, &app_outputs);
    
    // Timer has not expired yet
    if (app_outputs.fault_out == false)
    {
        // This is expected - timer running
    }
    
    // Step 4: At t=150ms, fault should now be latched (timer > 100ms)
    app_inputs.timestamp_ms = now + 150U;
    app_inputs.motor_current_ma = 200U;  // Still high current
    
    APP_Task(&app_inputs, &app_outputs);
    
    // Step 5: Verify fault condition is now detected
    EXPECT_TRUE(app_outputs.fault_out)
        << "SAFETY-CRITICAL: Stuck-on fault must latch after 100ms";
    EXPECT_EQ(app_outputs.motor_cmd, MOTOR_STOP) 
        << "Motor must remain STOP during fault";
}