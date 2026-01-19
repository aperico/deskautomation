/*
3-Position Rocker Switch (ON-OFF-ON) to Arduino Wiring

    [ 7 ]---+         +---[ GND ]
            |         |
    [ 8 ]---+         +---[ GND ]

Legend:
  [ 7 ] = SWITCH_UP_PIN (Arduino digital input, e.g. D7)
  [ 8 ] = SWITCH_DOWN_PIN (Arduino digital input, e.g. D8)
  [ GND ] = Arduino GND

Switch positions:
  - UP:   [ 7 ] connected to GND (LOW), [ 8 ] HIGH
  - DOWN: [ 8 ] connected to GND (LOW), [ 7 ] HIGH
  - OFF:  both [ 7 ] and [ 8 ] HIGH (not connected)

Wire both GND terminals of the switch to Arduino GND.
Wire each signal terminal to its respective Arduino input pin.
Use INPUT_PULLUP in code for both pins.
*/
// arduino.ino
// -----------------------------------------------------------------------------
// Main application file for Desk Automation Project
// Handles setup, main loop, and high-level control flow
//
// Best Practices:
// - All pin assignments are defined in PinConfig.h
// - Debug code is wrapped in DEBUG macros
// - No user input or limiters in this hardware revision
// - All outputs are processed via HAL_ProcessAppState
// - No test code in production loop
// -----------------------------------------------------------------------------
#define DEBUG

#include <Arduino.h>
#include "HAL.h"
#include "DeskController.h"
#include "PinConfig.h"


// Application input and output structures
static DeskAppInputs_t app_logical_inputs;
static DeskAppOutputs_t app_logical_outputs;
static HAL_Ouputs_t hal_outputs;



// -----------------------------------------------------------------------------
// Arduino setup function
// Initializes hardware and application state
// -----------------------------------------------------------------------------

void setup() {
  HAL_init(); // Initialize hardware abstraction layer (FUNC-001)
  HAL_wait_startup(); // Allow time before any motion (non-blocking via HAL_Task)
  DeskApp_task_init(&app_logical_inputs, &app_logical_outputs); // Initialize application logic (FUNC-016)
#ifdef DEBUG
  Serial.begin(9600);  // Initialize serial communication for debugging
  HAL_set_logger([](const char *msg){ Serial.println(msg); });
#endif

}

void loop() {
  static DeskAppTask_Return_t ret;
  // Read ON/OFF/ON switch (active low: pressed = LOW)
  SwitchState_t sw_state = HAL_ReadSwitchState();
  app_logical_inputs.switch_state = sw_state;

  ret = DeskApp_task(&app_logical_inputs, &app_logical_outputs);

  unsigned long now = millis();

  // Error handling: if HAL error detected, stop motor and log error
  static unsigned long errorTime = 0;
  /*if (HAL_HasError()) {
    app_logical_outputs.motor_enable = false;
    app_logical_outputs.motor_pwm = 0;
    if (errorTime == 0) errorTime = now;
    #ifdef DEBUG
    Serial.println("[ERROR] HAL detected hardware fault! Motor stopped.");
    #endif
  }*/

  HAL_ProcessAppState(ret, &app_logical_outputs, &hal_outputs);

  // For demonstration: auto-clear error after 5 seconds
  /*if (errorTime && (now - errorTime > 5000)) {
    HAL_ClearError();
    #ifdef DEBUG
    Serial.println("[INFO] HAL error state cleared (demo).");
    #endif
    errorTime = 0;
  }*/

  static unsigned long lastLog = 0;
  #ifdef DEBUG
  if (now - lastLog >= 1000) {
    Serial.print("App State: ");
    Serial.println(ret);
    lastLog = now;

    Serial.print("Motor Enable: ");
    Serial.println(app_logical_outputs.motor_enable);
    Serial.print("Motor Direction: ");
    Serial.println(app_logical_outputs.motor_direction);
    Serial.print("Motor PWM: ");
    Serial.println(app_logical_outputs.motor_pwm);
    Serial.print("Right Current Sense: ");
    Serial.println(hal_outputs.r_current);
    Serial.print("Left Current Sense: ");
    Serial.println(hal_outputs.l_current);
    Serial.print("Switch State: ");
    bool up = digitalRead(SWITCH_UP_PIN) == LOW;
    bool down = digitalRead(SWITCH_DOWN_PIN) == LOW;
    Serial.print(" UP="); Serial.print(up);
    Serial.print(" DOWN="); Serial.println(down);
  }
  #endif
}
