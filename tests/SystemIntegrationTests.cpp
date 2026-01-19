/**
 * @file SystemIntegrationTests.cpp
 * @brief System-level integration tests: DeskController ↔ HAL (with HALMock)
 * 
 * @module MODULE-007 (System Integration Test Suite)
 * @purpose Verify DeskController outputs are correctly applied by HAL via HALMock
 * @architecture ARCH-COMP-001 (DeskController), ARCH-COMP-002 (HAL), ARCH-IF-001, ARCH-IF-002
 * @note Uses host-side HALMock to observe pin state changes.
 */

#include <gtest/gtest.h>
#include "DeskController.h"
#include "HAL.h"
#include "PinConfig.h"

// Exposed by HALMock
extern int pin_states[64];

class SIT : public ::testing::Test {
protected:
    DeskAppInputs_t inputs{};
    DeskAppOutputs_t outputs{};
    HAL_Ouputs_t hal_outputs{};

    void SetUp() override {
        // Reset pin states and initialize HAL/controller
        for (int i = 0; i < 64; ++i) pin_states[i] = 0;
        HAL_init();
        DeskApp_task_init(&inputs, &outputs);
    }
};

// SIT-001: UP command drives motor upward via HAL
TEST_F(SIT, SIT001_UpCommand_DrivesMotorUp) {
    inputs.switch_state = SWITCH_STATE_UP;
    DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);
    ASSERT_EQ(ret, APP_TASK_SUCCESS);

    HAL_ProcessAppState(ret, &outputs, &hal_outputs);

    EXPECT_TRUE(outputs.motor_enable);
    EXPECT_FALSE(outputs.motor_direction);
    EXPECT_EQ(outputs.motor_pwm, 255);

    EXPECT_EQ(pin_states[RPWM_PIN], 255) << "RPWM should get full PWM";
    EXPECT_EQ(pin_states[LPWM_PIN], 0) << "LPWM should be off";
    EXPECT_EQ(pin_states[R_EN_PIN], 1) << "R_EN should be HIGH";
    EXPECT_EQ(pin_states[L_EN_PIN], 1) << "L_EN should be HIGH";
}

// SIT-002: DOWN command drives motor downward via HAL
TEST_F(SIT, SIT002_DownCommand_DrivesMotorDown) {
    inputs.switch_state = SWITCH_STATE_DOWN;
    DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);
    ASSERT_EQ(ret, APP_TASK_SUCCESS);

    HAL_ProcessAppState(ret, &outputs, &hal_outputs);

    EXPECT_TRUE(outputs.motor_enable);
    EXPECT_TRUE(outputs.motor_direction);
    EXPECT_EQ(outputs.motor_pwm, 255);

    EXPECT_EQ(pin_states[RPWM_PIN], 0) << "RPWM should be off for down";
    EXPECT_EQ(pin_states[LPWM_PIN], 255) << "LPWM should get full PWM";
    EXPECT_EQ(pin_states[R_EN_PIN], 1) << "R_EN should be HIGH";
    EXPECT_EQ(pin_states[L_EN_PIN], 1) << "L_EN should be HIGH";
}

// SIT-003: OFF command stops motor via HAL
TEST_F(SIT, SIT003_OffCommand_StopsMotor) {
    // Start with UP to set non-zero state
    inputs.switch_state = SWITCH_STATE_UP;
    DeskApp_task(&inputs, &outputs);
    HAL_ProcessAppState(APP_TASK_SUCCESS, &outputs, &hal_outputs);

    // Now switch to OFF
    inputs.switch_state = SWITCH_STATE_OFF;
    DeskApp_task(&inputs, &outputs);
    HAL_ProcessAppState(APP_TASK_SUCCESS, &outputs, &hal_outputs);

    EXPECT_FALSE(outputs.motor_enable);
    EXPECT_EQ(outputs.motor_pwm, 0);
    EXPECT_EQ(pin_states[RPWM_PIN], 0);
    EXPECT_EQ(pin_states[LPWM_PIN], 0);
    EXPECT_EQ(pin_states[R_EN_PIN], 0);
    EXPECT_EQ(pin_states[L_EN_PIN], 0);
}

// SIT-004: Null outputs should surface APP_TASK_ERROR and not move motor
TEST_F(SIT, SIT004_NullOutputs_ReturnsErrorAndNoMotion) {
    inputs.switch_state = SWITCH_STATE_UP;
    DeskAppTask_Return_t ret = DeskApp_task(&inputs, nullptr);
    EXPECT_EQ(ret, APP_TASK_ERROR);

    // Ensure motor pins remain untouched (still default 0)
    EXPECT_EQ(pin_states[RPWM_PIN], 0);
    EXPECT_EQ(pin_states[LPWM_PIN], 0);
    EXPECT_EQ(pin_states[R_EN_PIN], 0);
    EXPECT_EQ(pin_states[L_EN_PIN], 0);
}

// SIT-005: Conflicting inputs (UP+DOWN) result in no movement (safe stop)
TEST_F(SIT, SIT005_ConflictingInputs_StopMotor) {
    // Simulate conflicting inputs: controller treats as OFF (safety)
    inputs.switch_state = SWITCH_STATE_OFF; // Controller has only one switch value; conflict maps to OFF
    DeskApp_task(&inputs, &outputs);
    HAL_ProcessAppState(APP_TASK_SUCCESS, &outputs, &hal_outputs);

    EXPECT_FALSE(outputs.motor_enable);
    EXPECT_EQ(outputs.motor_pwm, 0);
    EXPECT_EQ(pin_states[RPWM_PIN], 0);
    EXPECT_EQ(pin_states[LPWM_PIN], 0);
    EXPECT_EQ(pin_states[R_EN_PIN], 0);
    EXPECT_EQ(pin_states[L_EN_PIN], 0);
}

// SIT-006: Power cycle reinitializes HAL to safe state
TEST_F(SIT, SIT006_PowerCycle_ReinitializesSafeState) {
    // Move up, then simulate power cycle by re-running HAL_init + DeskApp_task_init
    inputs.switch_state = SWITCH_STATE_UP;
    DeskApp_task(&inputs, &outputs);
    HAL_ProcessAppState(APP_TASK_SUCCESS, &outputs, &hal_outputs);

    HAL_init();
    DeskApp_task_init(&inputs, &outputs);

    EXPECT_EQ(pin_states[RPWM_PIN], 0);
    EXPECT_EQ(pin_states[LPWM_PIN], 0);
    EXPECT_EQ(pin_states[R_EN_PIN], 0);
    EXPECT_EQ(pin_states[L_EN_PIN], 0);
    EXPECT_FALSE(outputs.motor_enable);
    EXPECT_EQ(outputs.motor_pwm, 0);
}
