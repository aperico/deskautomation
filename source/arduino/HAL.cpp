#include "HAL.h"


#if defined(ARDUINO)
#include <Arduino.h>
#endif

void HAL_Init() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ERROR_LED, OUTPUT);
  pinMode(MOVING_UP_LED, OUTPUT);
  pinMode(MOVING_DOWN_LED, OUTPUT);
  HAL_StopMotor();
}

void HAL_ProcessAppState(const DeskAppTask_Return_t ret, const DeskAppOutputs_t *outputs){
if (ret == APP_TASK_SUCCESS) {
    if (outputs->moveUp == TRUE) {
      HAL_SetErrorLED(false);
      HAL_BlinkUPLED();
      HAL_MoveUp(MOTOR_SPEED);
    }
    if (outputs->moveDown == TRUE) {
      HAL_SetErrorLED(false);
      HAL_BlinkDOWNLED();
      HAL_MoveDown(MOTOR_SPEED);
    }
    if (outputs->stop == TRUE) {
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


void HAL_SetErrorLED(const bool state) { 
  digitalWrite(ERROR_LED, state); 
}
void HAL_SetMovingUpLED(const bool state) { 
  digitalWrite(MOVING_UP_LED, state); 
}
void HAL_SetMovingDownLED(const bool state) {
  digitalWrite(MOVING_DOWN_LED, state); 
}

bool HAL_GetMovingDownLED() { return digitalRead(MOVING_DOWN_LED); }
bool HAL_GetMovingUpLED() { return digitalRead(MOVING_UP_LED); }
bool HAL_GetErrorLED() { return digitalRead(ERROR_LED); }

void HAL_MoveUp(const unsigned char speed) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, speed);
}

void HAL_MoveDown(const unsigned char speed) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, speed);
}

void HAL_StopMotor() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 0);
}

void HAL_BlinkErrorLED() {
  static unsigned long lastBlinkTime = 0;
  if (millis() - lastBlinkTime >= BLINK_INTERVAL_MS) {
    lastBlinkTime = millis();
    HAL_SetErrorLED(!HAL_GetErrorLED());
  }
}

void HAL_BlinkUPLED() {
  static unsigned long lastBlinkTime = 0;
  if (millis() - lastBlinkTime >= BLINK_INTERVAL_MS) {
    lastBlinkTime = millis();
    HAL_SetMovingUpLED(!HAL_GetMovingUpLED());
  }
}

void HAL_BlinkDOWNLED() {
  static unsigned long lastBlinkTime = 0;
  if (millis() - lastBlinkTime >= BLINK_INTERVAL_MS) {
    lastBlinkTime = millis();
    HAL_SetMovingDownLED(!HAL_GetMovingDownLED());
  }
}

