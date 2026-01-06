// arduino.ino
// -----------------------------------------------------------------------------
// Main application file for Desk Automation Project
// Handles setup, main loop, and high-level control flow
// -----------------------------------------------------------------------------

#include "HAL.h"
#include "DeskController.h"

// Application input and output structures
static DeskAppInputs_t inputs;
static DeskAppOutputs_t outputs;

// -----------------------------------------------------------------------------
// Arduino setup function
// Initializes hardware and application state
// -----------------------------------------------------------------------------
void setup() {
  HAL_Init(); // Initialize hardware abstraction layer
  delay(1000); // Allow time before any motion
  DeskApp_task_init(&inputs, &outputs); // Initialize application logic
  Serial.begin(9600);  // Initialize serial communication for debugging
}

// -----------------------------------------------------------------------------
// Arduino main loop
// Reads inputs, runs application logic, updates outputs
// -----------------------------------------------------------------------------
void loop() {
  static DeskAppTask_Return_t ret;

  // Read button states
  inputs.btUPPressed = digitalRead(BUTTON_UP_PIN) == LOW ? FALSE : TRUE;
  inputs.btDOWNPressed = digitalRead(BUTTON_DOWN_PIN) == LOW ? FALSE : TRUE;

  // Run application logic and update hardware state
  ret = DeskApp_task(&inputs, &outputs);
  //HAL_ProcessAppState(ret, &outputs);
   digitalWrite(LED_RIGHT_PIN, HIGH); // Test: Turn ON LED
  digitalWrite(LED_LEFT_PIN, HIGH);  // Test: Turn ON LED
  digitalWrite(ERROR_LED, HIGH);      // Test: Turn OFF ERROR LED
}
