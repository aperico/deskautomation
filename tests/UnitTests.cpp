#include "../source/arduino/DeskController.h"
#include <gtest/gtest.h>


TEST(SmokeTest, BasicTruth) {
  DeskAppInputs_t inputs;
  DeskAppOutputs_t outputs;

  inputs.btUPPressed = FALSE;
  inputs.btDOWNPressed = FALSE;
  inputs.upperLimitActive = FALSE;
  inputs.lowerLimitActive = FALSE;

  DeskApp_task(&inputs, &outputs);

  EXPECT_EQ(1 + 1, 2);
}

TEST(SmokeTest, BasicTruth2) {
  DeskAppInputs_t inputs;
  DeskAppOutputs_t outputs;

  inputs.btUPPressed = FALSE;
  inputs.btDOWNPressed = FALSE;
  inputs.upperLimitActive = FALSE;
  inputs.lowerLimitActive = FALSE;

  DeskApp_task(&inputs, &outputs);

  EXPECT_EQ(1 + 1, 2);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
