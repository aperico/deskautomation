#include "HAL.h"
#include "DeskController.h"

static DeskAppInputs_t inputs;
static DeskAppOutputs_t outputs;

void setup() {
  HAL_Init();
  delay(2000); // Allow time before any motion
  DeskApp_task_init(&inputs, &outputs);
}

void loop() {
  static DeskAppTask_Return_t ret;
  ret = DeskApp_task(&inputs, &outputs);
  HAL_ProcessAppState(ret, &outputs);
}
