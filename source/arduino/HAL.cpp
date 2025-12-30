#include "HAL.h"
#if defined(ARDUINO)
#include <Arduino.h>
#endif

void HAL_Init() {
  pinMode(BUTTON_UP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  
  pinMode(ERROR_LED, OUTPUT);
  pinMode(LED_RIGHT_PIN, OUTPUT);
  pinMode(LED_LEFT_PIN, OUTPUT);
  
  HAL_StopMotor();

  digitalWrite(LED_RIGHT_PIN, HIGH); // Test: Turn ON LED
  digitalWrite(LED_LEFT_PIN, HIGH);  // Test: Turn ON LED
  digitalWrite(ERROR_LED, HIGH);      // Test: Turn OFF ERROR LED
  delay(1000); // Wait for 2 seconds

  digitalWrite(LED_RIGHT_PIN, LOW); // Test: Turn ON LED
  digitalWrite(LED_LEFT_PIN, LOW);  // Test: Turn ON LED
  digitalWrite(ERROR_LED, LOW);      // Test: Turn OFF ERROR LED
}

void HAL_ProcessAppState(const DeskAppTask_Return_t ret, const DeskAppOutputs_t *outputs){
  
  
  if (ret == APP_TASK_SUCCESS) {
    if (outputs->moveUp == TRUE) {
      HAL_SetErrorLED(false);
      HAL_MoveUp(MOTOR_SPEED);
    }
    if (outputs->moveDown == TRUE) {
      HAL_SetErrorLED(false);
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
  digitalWrite(ERROR_LED, state? HIGH : LOW); 
}

void HAL_SetMovingUpLED(const bool state) { 
  digitalWrite(LED_LEFT_PIN, state? HIGH : LOW); 
}
void HAL_SetMovingDownLED(const bool state) {
  digitalWrite(LED_RIGHT_PIN, state? HIGH : LOW); 
}

bool HAL_GetMovingDownLED() { return digitalRead(LED_RIGHT_PIN); }
bool HAL_GetMovingUpLED() { return digitalRead(LED_LEFT_PIN); }
bool HAL_GetErrorLED() { return digitalRead(ERROR_LED); }

void HAL_MoveUp(const unsigned char speed) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, speed);
  HAL_SetMovingUpLED(true); // Turn ON LED when moving up
}

void HAL_MoveDown(const unsigned char speed) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, speed);
  HAL_SetMovingDownLED(true); // Turn ON LED when moving down
}

void HAL_StopMotor() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 0);
  HAL_SetMovingUpLED(false);    // Turn OFF LEDs when stopped
  HAL_SetMovingDownLED(false);
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

