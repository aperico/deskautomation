#include "HAL.h"
#include "DeskController.h"

static DeskAppInputs_t inputs;
static DeskAppOutputs_t outputs;

void setup() {
  HAL_Init();
  delay(2000); // Allow time before any motion
  DeskApp_task_init(&inputs, &outputs);
  Serial.begin(9600);  // Initialize serial communication
}

void loop() {
  static DeskAppTask_Return_t ret;

  // Read inputs
  inputs.btUPPressed = digitalRead(BUTTON_UP_PIN) == LOW ? FALSE : TRUE;
  inputs.btDOWNPressed = digitalRead(BUTTON_DOWN_PIN) == LOW ? FALSE : TRUE;
  static unsigned long lastPrintTime = 0;
  unsigned long currentTime = millis();
  if (currentTime - lastPrintTime >= 500) {
    // do nothing
    lastPrintTime = currentTime;
  }

  
  ret = DeskApp_task(&inputs, &outputs);
  HAL_ProcessAppState(ret, &outputs);

  
}
