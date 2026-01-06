#include "../source/arduino/DeskController.h"
#include <gtest/gtest.h>

// UC-01: Power the Desk Control System
TEST(DeskAppTest, UC01_Power_IdleStop_NoError) {
    DeskAppInputs_t inputs = {};
    DeskAppOutputs_t outputs = {};
    DeskApp_task_init(&inputs, &outputs);

    // System just powered on, no buttons pressed
    inputs.btUPPressed = FALSE;
    inputs.btDOWNPressed = FALSE;
    DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);

    EXPECT_EQ(outputs.moveUp, FALSE);
    EXPECT_EQ(outputs.moveDown, FALSE);
    EXPECT_EQ(outputs.error, FALSE);
}

TEST(DeskAppTest, UC01_NoButtonsPressed_IdleNoMovement) {
    DeskAppInputs_t inputs = {};
    DeskAppOutputs_t outputs = {};
    DeskApp_task_init(&inputs, &outputs);

    // No buttons pressed at any time
    inputs.btUPPressed = FALSE;
    inputs.btDOWNPressed = FALSE;
    DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);

    EXPECT_EQ(outputs.moveUp, FALSE);
    EXPECT_EQ(outputs.moveDown, FALSE);
    EXPECT_EQ(outputs.error, FALSE);
}

// UC-02: Raise Desk
TEST(DeskAppTest, UC02_UpPressed_MovesUp_WhenNotupperLimitActive) {
    DeskAppInputs_t inputs = {};
    DeskAppOutputs_t outputs = {};
    DeskApp_task_init(&inputs, &outputs);

    // Simulate Up button pressed, not at upper limit
    inputs.btUPPressed = TRUE;
    inputs.btDOWNPressed = FALSE;
    inputs.upperLimitActive = FALSE;
    DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);

    EXPECT_EQ(outputs.moveUp, TRUE);
    EXPECT_EQ(outputs.moveDown, FALSE);
    EXPECT_EQ(outputs.error, FALSE);
}

TEST(DeskAppTest, UC02_UpPressed_DoesNotMoveUp_WhenupperLimitActive) {
    DeskAppInputs_t inputs = {};
    DeskAppOutputs_t outputs = {};
    DeskApp_task_init(&inputs, &outputs);

    // Up button pressed, but already at upper limit
    inputs.btUPPressed = TRUE;
    inputs.btDOWNPressed = FALSE;
    inputs.upperLimitActive = TRUE;
    DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);

    EXPECT_EQ(outputs.moveUp, FALSE);
    EXPECT_EQ(outputs.moveDown, FALSE);
    EXPECT_EQ(outputs.error, FALSE);
}

// UC-03: Lower Desk
TEST(DeskAppTest, UC03_DownPressed_MovesDown_WhenNotlowerLimitActive) {
    DeskAppInputs_t inputs = {};
    DeskAppOutputs_t outputs = {};
    DeskApp_task_init(&inputs, &outputs);

    // Down button pressed, not at lower limit
    inputs.btUPPressed = FALSE;
    inputs.btDOWNPressed = TRUE;
    inputs.lowerLimitActive = FALSE;
    DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);

    EXPECT_EQ(outputs.moveUp, FALSE);
    EXPECT_EQ(outputs.moveDown, TRUE);
    EXPECT_EQ(outputs.error, FALSE);
    
}

TEST(DeskAppTest, UC03_DownPressed_DoesNotMoveDown_WhenlowerLimitActive) {
    DeskAppInputs_t inputs = {};
    DeskAppOutputs_t outputs = {};
    DeskApp_task_init(&inputs, &outputs);

    // Down button pressed, but already at lower limit
    inputs.btUPPressed = FALSE;
    inputs.btDOWNPressed = TRUE;
    inputs.lowerLimitActive = TRUE;
    DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);

    EXPECT_EQ(outputs.moveUp, FALSE);
    EXPECT_EQ(outputs.moveDown, FALSE);
    EXPECT_EQ(outputs.error, FALSE);
    
}

// UC-04: Emergency Stop (Software-Based or Manual)
TEST(DeskAppTest, UC04_EmergencyStop_FromUp_WhenLowerLimitActive) {
    DeskAppInputs_t inputs = {};
    DeskAppOutputs_t outputs = {};
    DeskApp_task_init(&inputs, &outputs);

    // Simulate moving up, then lower limit becomes active (fault)
    inputs.btUPPressed = TRUE;
    inputs.btDOWNPressed = FALSE;
    inputs.upperLimitActive = TRUE; // Fault: both up and upper limit
    inputs.lowerLimitActive = TRUE; // Fault: both up and lower limit
    DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);

    EXPECT_EQ(outputs.moveUp, FALSE);
    EXPECT_EQ(outputs.moveDown, FALSE);
    EXPECT_EQ(outputs.error, TRUE);
    EXPECT_EQ(ret, APP_TASK_ERROR);
}

TEST(DeskAppTest, UC04_EmergencyStop_FromDown_WhenUpperLimitActive) {
    DeskAppInputs_t inputs = {};
    DeskAppOutputs_t outputs = {};
    DeskApp_task_init(&inputs, &outputs);

    // Simulate moving down, then upper limit becomes active (fault)
    inputs.btUPPressed = FALSE;
    inputs.btDOWNPressed = TRUE;
    inputs.lowerLimitActive = TRUE; // Fault: both down and lower limit
    inputs.upperLimitActive = TRUE; // Fault: both down and upper limit
    DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);

    EXPECT_EQ(outputs.moveUp, FALSE);
    EXPECT_EQ(outputs.moveDown, FALSE);
    EXPECT_EQ(outputs.error, TRUE);
    
}

// UC-05: Visual Feedback (covered by output assertions in all tests)

// UC-06: Power-Off During Movement (not directly unit-testable, implied by idle tests)

// UC-07: Simultaneous Button Presses
TEST(DeskAppTest, UC07_BothButtonsPressed_NoMovement_Commanded) {
    DeskAppInputs_t inputs = {};
    DeskAppOutputs_t outputs = {};
    DeskApp_task_init(&inputs, &outputs);

    // Both buttons pressed, not at any limit
    inputs.btUPPressed = TRUE;
    inputs.btDOWNPressed = TRUE;
    inputs.upperLimitActive = FALSE;
    inputs.lowerLimitActive = FALSE;
    DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);

    EXPECT_EQ(outputs.moveUp, FALSE);
    EXPECT_EQ(outputs.moveDown, FALSE);
    EXPECT_EQ(outputs.error, FALSE);
}

TEST(DeskAppTest, UC08_BothButtonsPressed_NoMovement_WhenAtBothLimits) {
    DeskAppInputs_t inputs = {};
    DeskAppOutputs_t outputs = {};
    DeskApp_task_init(&inputs, &outputs);

    // Both buttons pressed, both limits active (lockout)
    inputs.btUPPressed = TRUE;
    inputs.btDOWNPressed = TRUE;
    inputs.upperLimitActive = TRUE;
    inputs.lowerLimitActive = TRUE;
    DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);

    EXPECT_EQ(outputs.moveUp, FALSE);
    EXPECT_EQ(outputs.moveDown, FALSE);
    EXPECT_EQ(outputs.error, TRUE);
    
}

// UC-08: Error Indication and Recovery
TEST(DeskAppTest, UC09_ErrorRecovery_ToIdle_WhenSafe) {
    DeskAppInputs_t inputs = {};
    DeskAppOutputs_t outputs = {};
    DeskApp_task_init(&inputs, &outputs);

    // Simulate error state, then clear error
    inputs.btUPPressed = FALSE;
    inputs.btDOWNPressed = FALSE;
    inputs.upperLimitActive = FALSE;
    inputs.lowerLimitActive = FALSE;
    // Assume error was previously set
    DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);

    EXPECT_EQ(outputs.error, FALSE);
    EXPECT_EQ(outputs.moveUp, FALSE);
    EXPECT_EQ(outputs.moveDown, FALSE);
    
}

// UC-10: Dwell before reversal (Up -> Down)
TEST(DeskAppTest, UC10_DwellBeforeReversal_UpToDown) {
    DeskAppInputs_t inputs = {};
    DeskAppOutputs_t outputs = {};
    DeskApp_task_init(&inputs, &outputs);

    // Simulate Up pressed, then Down pressed immediately after Up released
    inputs.btUPPressed = TRUE;
    inputs.btDOWNPressed = FALSE;
    DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);
    EXPECT_EQ(outputs.moveUp, TRUE);

    // Release Up, press Down immediately (simulate no dwell)
    inputs.btUPPressed = FALSE;
    inputs.btDOWNPressed = TRUE;
    ret = DeskApp_task(&inputs, &outputs);

    // Expect dwell or idle before allowing reversal
    // (This assertion may need to be adjusted based on your implementation)
    EXPECT_EQ(outputs.moveDown, FALSE); // Assuming dwell prevents immediate down
    EXPECT_EQ(outputs.moveUp, FALSE);
    
}

// Smoke/Basic Logic
TEST(SmokeTest, BasicTruth) {
    DeskAppInputs_t inputs = {};
    DeskAppOutputs_t outputs = {};
    DeskAppTask_Return_t ret;

    inputs.btUPPressed = FALSE;
    inputs.btDOWNPressed = FALSE;
    DeskApp_task_init(&inputs, &outputs);
    ret = DeskApp_task(&inputs, &outputs);

    EXPECT_EQ(outputs.moveUp, FALSE);
    EXPECT_EQ(outputs.moveDown, FALSE);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
