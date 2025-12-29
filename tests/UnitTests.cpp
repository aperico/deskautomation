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

TEST(SmokeTest, BasicTruth2) {
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

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
