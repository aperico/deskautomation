#if defined(ARDUINO)
#include <Arduino.h>
#endif
#include "HAL.h"

// Debounce helper function moved from arduino.ino
/**
 * @brief Debounces a button input on a specified pin.
 *
 * This function checks the state of a button connected to the given pin and applies a debounce algorithm
 * to filter out noise caused by mechanical button presses. It uses the provided DebounceState structure
 * to track the last stable state and the last time the state changed. If the button state remains stable
 * for longer than the specified debounce delay, the function returns the new state.
 *
 * @param pin The digital pin number where the button is connected.
 * @param state Reference to a DebounceState structure holding the last state and last debounce time.
 * @param debounceDelay The minimum time (in milliseconds) the button state must remain stable to be considered valid.
 * @return true if the button state has changed and is stable; false otherwise.
 */
bool HAL_debounceButton(const int pin, DebounceState &state, const unsigned long debounceDelay) {
  bool reading = digitalRead(pin) == LOW ? false : true;
  unsigned long currentTime = millis();
  if (reading != state.lastState) {
    state.lastDebounceTime = currentTime;
  }
  if ((currentTime - state.lastDebounceTime) > debounceDelay) {
    state.lastState = reading;
    return reading;
  }
  state.lastState = reading;
  return false;
}

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
    if (outputs->moveUp == true) {
      HAL_SetErrorLED(false);
      HAL_MoveUp(MOTOR_SPEED);
    }
    if (outputs->moveDown == true) {
      HAL_SetErrorLED(false);
      HAL_MoveDown(MOTOR_SPEED);
    }
    if (outputs->stop == true) {
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
// L298N 

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

