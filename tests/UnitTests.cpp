
#include <gtest/gtest.h>
#include "DeskController.h"

// Minimal test fixture for IBT-2 hardware (ON/OFF/ON switch)
class DeskControllerMinimalTest : public ::testing::Test {
protected:
    DeskAppInputs_t inputs;
    DeskAppOutputs_t outputs;

    void SetUp() override {
        DeskApp_task_init(&inputs, &outputs);
    }
};

// Test: OFF state (center)
TEST_F(DeskControllerMinimalTest, Unit_OutputsOffWhenSwitchOff) {
    inputs.switch_state = SWITCH_STATE_OFF;
    DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);
    EXPECT_FALSE(outputs.motor_enable);
    EXPECT_FALSE(outputs.motor_direction);
    EXPECT_EQ(outputs.motor_pwm, 0);
    EXPECT_EQ(ret, APP_TASK_SUCCESS);
}

// Test: UP state
TEST_F(DeskControllerMinimalTest, Unit_OutputsUpWhenSwitchUp) {
    inputs.switch_state = SWITCH_STATE_UP;
    DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);
    EXPECT_TRUE(outputs.motor_enable);
    EXPECT_FALSE(outputs.motor_direction);
    EXPECT_EQ(outputs.motor_pwm, 255);
    EXPECT_EQ(ret, APP_TASK_SUCCESS);
}

// Test: DOWN state
TEST_F(DeskControllerMinimalTest, Unit_OutputsDownWhenSwitchDown) {
    inputs.switch_state = SWITCH_STATE_DOWN;
    DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);
    EXPECT_TRUE(outputs.motor_enable);
    EXPECT_TRUE(outputs.motor_direction);
    EXPECT_EQ(outputs.motor_pwm, 255);
    EXPECT_EQ(ret, APP_TASK_SUCCESS);
}