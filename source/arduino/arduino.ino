// arduino.ino
// -----------------------------------------------------------------------------
// Main application file for Desk Automation Project
// Handles setup, main loop, and high-level control flow
//
// Best Practices:
// - All pin assignments are defined in PinConfig.h
// - Debug code is wrapped in DEBUG macros
// - Button inputs are debounced
// - All outputs are processed via HAL_ProcessAppState
// - No test code in production loop
// -----------------------------------------------------------------------------


#include <Arduino.h>
#include "HAL.h"
#include "DeskController.h"
#include "PinConfig.h"


// Application input and output structures
static DeskAppInputs_t inputs;
static DeskAppOutputs_t outputs;

// Debounce states for buttons
static DebounceState upDebounce = {false, 0};
static DebounceState downDebounce = {false, 0};
const unsigned long debounceDelay = 50; // ms

// -----------------------------------------------------------------------------
// Arduino setup function
// Initializes hardware and application state
// -----------------------------------------------------------------------------

void setup() {
  HAL_Init(); // Initialize hardware abstraction layer
  delay(1000); // Allow time before any motion
  DeskApp_task_init(&inputs, &outputs); // Initialize application logic
#ifdef DEBUG
  Serial.begin(9600);  // Initialize serial communication for debugging
#endif
}

// -----------------------------------------------------------------------------
// Arduino main loop
// Reads inputs, runs application logic, updates outputs
// -----------------------------------------------------------------------------

void loop() {
  static DeskAppTask_Return_t ret;

  // read debounced buttons
  inputs.btUPPressed   = HAL_debounceButton(BUTTON_UP_PIN, &upDebounce, debounceDelay);
  inputs.btDOWNPressed = HAL_debounceButton(BUTTON_DOWN_PIN, &downDebounce, debounceDelay);

  // Run application logic and update hardware state
  ret = DeskApp_task(&inputs, &outputs);
  HAL_ProcessAppState(ret, &outputs); // Ensure all outputs are processed

#ifdef DEBUG
  Serial.print("App State: ");
  Serial.println(ret);
#endif
}
