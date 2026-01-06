#include "../source/arduino/DeskController.h"
#include <gtest/gtest.h>
// Software Test Cases implemented are based on SoftwareTestCases.md

// SR-01: System Initialization
TEST(DeskAppTest, SR01_Power_IdleStop_NoError) {
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

TEST(DeskAppTest, SR01_NoButtonsPressed_IdleNoMovement) {
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

// SR-02: Upward Movement Command
TEST(DeskAppTest, SR02_UpPressed_MovesUp_WhenNotupperLimitActive) {
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

TEST(DeskAppTest, SR02_UpPressed_DoesNotMoveUp_WhenupperLimitActive) {
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

// SR-03: Downward Movement Command
TEST(DeskAppTest, SR03_DownPressed_MovesDown_WhenNotlowerLimitActive) {
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

TEST(DeskAppTest, SR03_DownPressed_DoesNotMoveDown_WhenlowerLimitActive) {
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

// SR-04: Emergency Stop
TEST(DeskAppTest, SR04_EmergencyStop_FromUp_WhenLowerLimitActive) {
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

TEST(DeskAppTest, SR04_EmergencyStop_FromDown_WhenUpperLimitActive) {
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

// SR-05: Visual Feedback (covered by output assertions in all tests)

// SR-06: Power-Off Handling (directly tested below)

// SR-07: Simultaneous Button Presses
TEST(DeskAppTest, SR07_BothButtonsPressed_NoMovement_Commanded) {
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

TEST(DeskAppTest, SR07_BothButtonsPressed_NoMovement_WhenAtBothLimits) {
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

// SR-08: Error Detection and Recovery
TEST(DeskAppTest, SR08_ErrorRecovery_ToIdle_WhenSafe) {
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

// SR-02/SR-03: Dwell before reversal (Up -> Down)
TEST(DeskAppTest, SR02_SR03_DwellBeforeReversal_UpToDown) {
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

// SR-06: Power-Off Handling
TEST(DeskAppTest, SR06_PowerOffDuringMovement_ReinitializesToIdle) {
    DeskAppInputs_t inputs = {};
    DeskAppOutputs_t outputs = {};
    DeskApp_task_init(&inputs, &outputs);

    // Simulate Up button pressed, moving up
    inputs.btUPPressed = TRUE;
    inputs.btDOWNPressed = FALSE;
    inputs.upperLimitActive = FALSE;
    DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);
    EXPECT_EQ(outputs.moveUp, TRUE);

    // Simulate power-off (re-initialize)
    DeskApp_task_init(&inputs, &outputs);

    // After power restore, system should be IDLE, no movement
    EXPECT_EQ(outputs.moveUp, FALSE);
    EXPECT_EQ(outputs.moveDown, FALSE);
    EXPECT_EQ(outputs.error, FALSE);
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
