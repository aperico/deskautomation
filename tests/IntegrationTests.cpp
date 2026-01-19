
// IntegrationTests.cpp - Minimal integration test for IBT-2 hardware (no user inputs, no limiters, no LEDs)
#include <gtest/gtest.h>
#include "DeskController.h"

class IntegrationTest : public ::testing::Test {
protected:
    DeskAppInputs_t inputs;
    DeskAppOutputs_t outputs;
    void SetUp() override {
        DeskApp_task_init(&inputs, &outputs);
    }
};

// Test: OFF state (center)
TEST_F(IntegrationTest, OutputsOffWhenSwitchOff) {
    inputs.switch_state = SWITCH_STATE_OFF;
    DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);
    EXPECT_FALSE(outputs.motor_enable);
    EXPECT_FALSE(outputs.motor_direction);
    EXPECT_EQ(outputs.motor_pwm, 0);
    EXPECT_EQ(ret, APP_TASK_SUCCESS);
}

// Test: UP state
TEST_F(IntegrationTest, OutputsUpWhenSwitchUp) {
    inputs.switch_state = SWITCH_STATE_UP;
    DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);
    EXPECT_TRUE(outputs.motor_enable);
    EXPECT_FALSE(outputs.motor_direction);
    EXPECT_EQ(outputs.motor_pwm, 255);
    EXPECT_EQ(ret, APP_TASK_SUCCESS);
}

// Test: DOWN state
TEST_F(IntegrationTest, OutputsDownWhenSwitchDown) {
    inputs.switch_state = SWITCH_STATE_DOWN;
    DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);
    EXPECT_TRUE(outputs.motor_enable);
    EXPECT_TRUE(outputs.motor_direction);
    EXPECT_EQ(outputs.motor_pwm, 255);
    EXPECT_EQ(ret, APP_TASK_SUCCESS);
}

// All other integration tests related to user inputs, limit switches, and LEDs have been removed for minimal IBT-2 hardware.

