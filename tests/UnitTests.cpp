#include "../source/arduino/DeskController.h"
#include <gtest/gtest.h>


TEST(SmokeTest, BasicTruth) {
  DeskAppInputs_t inputs;
  DeskAppOutputs_t outputs;
  DeskAppTask_Return_t ret;

  inputs.btUPPressed = FALSE;
  inputs.btDOWNPressed = FALSE;
  inputs.upperLimitActive = FALSE;
  inputs.lowerLimitActive = FALSE;

  ret = DeskApp_task(&inputs, &outputs);
  EXPECT_EQ(ret, APP_TASK_SUCCESS);

}

// UC-01: Power the Desk Control System
TEST(DeskAppTest, UC01_Power_IdleStop_NoError) {
  DeskAppInputs_t inputs = {};
  DeskAppOutputs_t outputs = {};
  DeskApp_task_init(&inputs, &outputs);

  // No buttons pressed, no limits active
  inputs.btUPPressed = FALSE;
  inputs.btDOWNPressed = FALSE;
  inputs.upperLimitActive = FALSE;
  inputs.lowerLimitActive = FALSE;

  DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);

  EXPECT_EQ(ret, APP_TASK_SUCCESS);
  EXPECT_TRUE(outputs.stop);
  EXPECT_FALSE(outputs.error);
  EXPECT_FALSE(outputs.moveUp);
  EXPECT_FALSE(outputs.moveDown);
}

TEST(DeskAppTest, UC02_UpPressed_MovesUp_WhenNotAtUpperLimit) {
  DeskAppInputs_t inputs = {};
  DeskAppOutputs_t outputs = {};
  inputs.btUPPressed = TRUE;
  inputs.btDOWNPressed = FALSE;
  inputs.upperLimitActive = FALSE;
  inputs.lowerLimitActive = FALSE;
  // First cycle transitions to MOVE_UP state
  DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);
  EXPECT_EQ(ret, APP_TASK_SUCCESS);
  // Second cycle produces MOVE_UP outputs
  ret = DeskApp_task(&inputs, &outputs);
  EXPECT_EQ(ret, APP_TASK_SUCCESS);
  EXPECT_TRUE(outputs.moveUp);
  EXPECT_FALSE(outputs.moveDown);
}

// UC-02: Raise Desk
TEST(DeskAppTest, UC02_UpPressed_DoesNotMoveUp_WhenAtUpperLimit) {
  DeskAppInputs_t inputs = {};
  DeskAppOutputs_t outputs = {};
  inputs.btUPPressed = TRUE;
  inputs.btDOWNPressed = FALSE;
  inputs.upperLimitActive = TRUE;
  inputs.lowerLimitActive = FALSE;

  DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);

  EXPECT_EQ(ret, APP_TASK_SUCCESS);
  EXPECT_FALSE(outputs.moveUp);
  EXPECT_FALSE(outputs.moveDown);
}

TEST(DeskAppTest, UC03_DownPressed_MovesDown_WhenNotAtLowerLimit) {
  DeskAppInputs_t inputs = {};
  DeskAppOutputs_t outputs = {};
  inputs.btUPPressed = FALSE;
  inputs.btDOWNPressed = TRUE;
  inputs.upperLimitActive = FALSE;
  inputs.lowerLimitActive = FALSE;
  // First cycle transitions to MOVE_DOWN state
  DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);
  EXPECT_EQ(ret, APP_TASK_SUCCESS);
  // Second cycle produces MOVE_DOWN outputs
  ret = DeskApp_task(&inputs, &outputs);
  EXPECT_EQ(ret, APP_TASK_SUCCESS);
  EXPECT_FALSE(outputs.moveUp);
  EXPECT_TRUE(outputs.moveDown);
}

// UC-03: Lower Desk
TEST(DeskAppTest, UC03_DownPressed_DoesNotMoveDown_WhenAtLowerLimit) {
  DeskAppInputs_t inputs = {};
  DeskAppOutputs_t outputs = {};
  inputs.btUPPressed = FALSE;
  inputs.btDOWNPressed = TRUE;
  inputs.upperLimitActive = FALSE;
  inputs.lowerLimitActive = TRUE;

  DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);

  EXPECT_EQ(ret, APP_TASK_SUCCESS);
  EXPECT_FALSE(outputs.moveUp);
  EXPECT_FALSE(outputs.moveDown);
}

// UC-04: Emergency Stop (Software-Based) from Up when lower limit becomes active
TEST(DeskAppTest, UC04_EmergencyStop_FromUp_WhenLowerLimitActive) {
  DeskAppInputs_t inputs = {};
  DeskAppOutputs_t outputs = {};
  // Start moving up
  inputs.btUPPressed = TRUE;
  inputs.btDOWNPressed = FALSE;
  inputs.upperLimitActive = FALSE;
  inputs.lowerLimitActive = FALSE;
  // First cycle transitions to MOVE_UP state
  DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);
  EXPECT_EQ(ret, APP_TASK_SUCCESS);
  // Second cycle outputs moveUp
  ret = DeskApp_task(&inputs, &outputs);
  EXPECT_EQ(ret, APP_TASK_SUCCESS);
  EXPECT_TRUE(outputs.moveUp);

  // Lower limit unexpectedly active: triggers error state transition
  inputs.lowerLimitActive = TRUE;
  ret = DeskApp_task(&inputs, &outputs);
  EXPECT_EQ(ret, APP_TASK_SUCCESS);

  // Next cycle: verify error flagged and motor stopped
  inputs.btUPPressed = FALSE;
  inputs.btDOWNPressed = FALSE;
  ret = DeskApp_task(&inputs, &outputs);
  EXPECT_EQ(ret, APP_TASK_SUCCESS);
  EXPECT_TRUE(outputs.stop);
  EXPECT_TRUE(outputs.error);
  EXPECT_FALSE(outputs.moveUp);
  EXPECT_FALSE(outputs.moveDown);
}

// UC-04: Emergency Stop (Software-Based) from Down when upper limit becomes active
TEST(DeskAppTest, UC04_EmergencyStop_FromDown_WhenUpperLimitActive) {
  DeskAppInputs_t inputs = {};
  DeskAppOutputs_t outputs = {};
  // Start moving down
  inputs.btUPPressed = FALSE;
  inputs.btDOWNPressed = TRUE;
  inputs.upperLimitActive = FALSE;
  inputs.lowerLimitActive = FALSE;
  // First cycle transitions to MOVE_DOWN state
  DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);
  EXPECT_EQ(ret, APP_TASK_SUCCESS);
  // Second cycle outputs moveDown
  ret = DeskApp_task(&inputs, &outputs);
  EXPECT_EQ(ret, APP_TASK_SUCCESS);
  EXPECT_TRUE(outputs.moveDown);

  // Upper limit unexpectedly active: triggers error state transition
  inputs.upperLimitActive = TRUE;
  ret = DeskApp_task(&inputs, &outputs);
  EXPECT_EQ(ret, APP_TASK_SUCCESS);

  // Next cycle: verify error flagged and motor stopped
  inputs.btUPPressed = FALSE;
  inputs.btDOWNPressed = FALSE;
  ret = DeskApp_task(&inputs, &outputs);
  EXPECT_EQ(ret, APP_TASK_SUCCESS);
  EXPECT_TRUE(outputs.stop);
  EXPECT_TRUE(outputs.error);
  EXPECT_FALSE(outputs.moveUp);
  EXPECT_FALSE(outputs.moveDown);
}

// UC-09: Error Recovery when inputs released and limits inactive
TEST(DeskAppTest, UC09_ErrorRecovery_ToIdle_WhenSafe) {
  DeskAppInputs_t inputs = {};
  DeskAppOutputs_t outputs = {};
  // Induce error by contradictory limit while moving up
  inputs.btUPPressed = TRUE;
  inputs.btDOWNPressed = FALSE;
  inputs.upperLimitActive = FALSE;
  inputs.lowerLimitActive = FALSE;
  DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);
  ret = DeskApp_task(&inputs, &outputs);
  EXPECT_TRUE(outputs.moveUp);

  // Trigger error
  inputs.lowerLimitActive = TRUE;
  ret = DeskApp_task(&inputs, &outputs);
  // Release inputs and clear limits
  inputs.btUPPressed = FALSE;
  inputs.btDOWNPressed = FALSE;
  inputs.upperLimitActive = FALSE;
  inputs.lowerLimitActive = FALSE;
  ret = DeskApp_task(&inputs, &outputs);
  EXPECT_EQ(ret, APP_TASK_SUCCESS);
  EXPECT_TRUE(outputs.stop);
  EXPECT_FALSE(outputs.error);
  EXPECT_FALSE(outputs.moveUp);
  EXPECT_FALSE(outputs.moveDown);
}

// UC-07: Dual Button Handling (Up priority)
TEST(DeskAppTest, UC07_BothButtonsPressed_UpHasPriority_WhenNotAtUpperLimit) {
  DeskAppInputs_t inputs = {};
  DeskAppOutputs_t outputs = {};
  inputs.btUPPressed = TRUE;
  inputs.btDOWNPressed = TRUE;
  inputs.upperLimitActive = FALSE;
  inputs.lowerLimitActive = FALSE;
  // First cycle transitions to MOVE_UP because Up has priority
  DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);
  EXPECT_EQ(ret, APP_TASK_SUCCESS);
  // Second cycle produces MOVE_UP outputs
  ret = DeskApp_task(&inputs, &outputs);
  EXPECT_EQ(ret, APP_TASK_SUCCESS);
  EXPECT_TRUE(outputs.moveUp);
  EXPECT_FALSE(outputs.moveDown);
}

// UC-08: Limit Lockout (both limits)
TEST(DeskAppTest, UC08_BothButtonsPressed_NoMovement_WhenAtBothLimits) {
  DeskAppInputs_t inputs = {};
  DeskAppOutputs_t outputs = {};
  inputs.btUPPressed = TRUE;
  inputs.btDOWNPressed = TRUE;
  inputs.upperLimitActive = TRUE;
  inputs.lowerLimitActive = TRUE;

  DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);

  EXPECT_EQ(ret, APP_TASK_SUCCESS);
  EXPECT_FALSE(outputs.moveUp);
  EXPECT_FALSE(outputs.moveDown);
}

// UC-10: Dwell before reversal (Up -> Down)
TEST(DeskAppTest, UC10_DwellBeforeReversal_UpToDown) {
  DeskAppInputs_t inputs = {};
  DeskAppOutputs_t outputs = {};
  // Start moving up
  inputs.btUPPressed = TRUE;
  inputs.btDOWNPressed = FALSE;
  inputs.upperLimitActive = FALSE;
  inputs.lowerLimitActive = FALSE;
  DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);
  ret = DeskApp_task(&inputs, &outputs);
  EXPECT_TRUE(outputs.moveUp);

  // Request reversal to down
  inputs.btUPPressed = FALSE;
  inputs.btDOWNPressed = TRUE;
  ret = DeskApp_task(&inputs, &outputs);
  // During dwell, outputs should be stop
  EXPECT_TRUE(outputs.stop);
  EXPECT_FALSE(outputs.error);
  EXPECT_FALSE(outputs.moveUp);
  EXPECT_FALSE(outputs.moveDown);

  // Still within dwell (first dwell tick)
  ret = DeskApp_task(&inputs, &outputs);
  EXPECT_TRUE(outputs.stop);
  EXPECT_FALSE(outputs.moveUp);
  EXPECT_FALSE(outputs.moveDown);

  // After dwell period elapses (second dwell tick), motion down is allowed
  ret = DeskApp_task(&inputs, &outputs);
  EXPECT_FALSE(outputs.moveUp);
  EXPECT_TRUE(outputs.moveDown);
}

// UC-01: Idle behavior with no inputs
TEST(DeskAppTest, UC01_NoButtonsPressed_IdleNoMovement) {
  DeskAppInputs_t inputs = {};
  DeskAppOutputs_t outputs = {};
  inputs.btUPPressed = FALSE;
  inputs.btDOWNPressed = FALSE;
  inputs.upperLimitActive = FALSE;
  inputs.lowerLimitActive = FALSE;

  DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);

  EXPECT_EQ(ret, APP_TASK_SUCCESS);
  EXPECT_FALSE(outputs.moveUp);
  EXPECT_FALSE(outputs.moveDown);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
