// SoftwareTests.cpp - Minimal tests for IBT-2 hardware (no user inputs, no limiters, no LEDs)
#include "../source/arduino/DeskController.h"
#include <gtest/gtest.h>


// Test: OFF state (center)
TEST(DeskAppTest, OutputsOffWhenSwitchOff) {
    DeskAppInputs_t inputs = {};
    DeskAppOutputs_t outputs = {};
    inputs.switch_state = SWITCH_STATE_OFF;
    DeskApp_task_init(&inputs, &outputs);
    DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);
    EXPECT_FALSE(outputs.motor_enable);
    EXPECT_FALSE(outputs.motor_direction);
    EXPECT_EQ(outputs.motor_pwm, 0);
    EXPECT_EQ(ret, APP_TASK_SUCCESS);
}

// Test: UP state
TEST(DeskAppTest, OutputsUpWhenSwitchUp) {
    DeskAppInputs_t inputs = {};
    DeskAppOutputs_t outputs = {};
    inputs.switch_state = SWITCH_STATE_UP;
    DeskApp_task_init(&inputs, &outputs);
    DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);
    EXPECT_TRUE(outputs.motor_enable);
    EXPECT_FALSE(outputs.motor_direction);
    EXPECT_EQ(outputs.motor_pwm, 255);
    EXPECT_EQ(ret, APP_TASK_SUCCESS);
}

// Test: DOWN state
TEST(DeskAppTest, OutputsDownWhenSwitchDown) {
    DeskAppInputs_t inputs = {};
    DeskAppOutputs_t outputs = {};
    inputs.switch_state = SWITCH_STATE_DOWN;
    DeskApp_task_init(&inputs, &outputs);
    DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);
    EXPECT_TRUE(outputs.motor_enable);
    EXPECT_TRUE(outputs.motor_direction);
    EXPECT_EQ(outputs.motor_pwm, 255);
    EXPECT_EQ(ret, APP_TASK_SUCCESS);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
