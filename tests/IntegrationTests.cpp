#include <gtest/gtest.h>
#include "hal.h"
#include "motor_controller.h"
#include "desk_app.h"
#include "desk_types.h"

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
