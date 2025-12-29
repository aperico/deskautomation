#include "HAL.h"
#include "DeskController.h"

static DeskAppInputs_t inputs;
static DeskAppOutputs_t outputs;

void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ERROR_LED, OUTPUT);
  HAL_StopMotor();
  delay(2000); // Allow time before any motion

  // app inputs init
  inputs.btUPPressed = FALSE;
  inputs.btDOWNPressed = FALSE;
  inputs.upperLimitActive = FALSE;
  inputs.lowerLimitActive = FALSE;

  // app outputs init
  outputs.moveUp = FALSE;
  outputs.moveDown = FALSE;
  outputs.stop = TRUE;
  outputs.error = FALSE;
}

void loop() {
  static DeskAppTask_Return_t ret;

  ret = DeskApp_task(&inputs, &outputs);

  if (ret == APP_TASK_SUCCESS) {
    if (outputs.moveUp == TRUE) {
      HAL_SetErrorLED(false);
      HAL_BlinkUPLED();
      HAL_MoveUp(MOTOR_SPEED);
    }
    if (outputs.moveDown == TRUE) {
      HAL_SetErrorLED(false);
      HAL_BlinkDOWNLED();
      HAL_MoveDown(MOTOR_SPEED);
    }
    if (outputs.stop == TRUE) {
      HAL_SetErrorLED(false);
      HAL_SetMovingUpLED(false);
      HAL_SetMovingDownLED(false);
      HAL_StopMotor();
    }
  } else {
    HAL_SetErrorLED(true);
    HAL_SetMovingUpLED(false);
    HAL_SetMovingDownLED(false);
    HAL_BlinkErrorLED();
    HAL_StopMotor();
  }
}
