#include "HAL.h"

#if defined(ARDUINO)
#include <Arduino.h>
#endif

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

