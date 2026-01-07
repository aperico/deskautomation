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
static DebounceState upDebounce = { .lastReading = false, .stableState = false, .changed = false, .lastDebounceMs = 0 };
static DebounceState downDebounce = { .lastReading = false, .stableState = false, .changed = false, .lastDebounceMs = 0 };
const unsigned long DEBOUNCE_DELAY = 50; // ms

// -----------------------------------------------------------------------------
// Arduino setup function
// Initializes hardware and application state
// -----------------------------------------------------------------------------

void setup() {
  HAL_init(); // Initialize hardware abstraction layer (FUNC-001)
  HAL_wait_startup(); // Allow time before any motion (non-blocking via HAL_Task)
  DeskApp_task_init(&inputs, &outputs); // Initialize application logic (FUNC-016)
#ifdef DEBUG
  Serial.begin(9600);  // Initialize serial communication for debugging
  HAL_set_logger([](const char *msg){ Serial.println(msg); });
#endif
}

// -----------------------------------------------------------------------------
// Arduino main loop
// Reads inputs, runs application logic, updates outputs
// -----------------------------------------------------------------------------

void loop() {
  static DeskAppTask_Return_t ret;

  // PRE-Process inputs
  {
    // read debounced buttons
    inputs.btUPPressed   = HAL_debounceButton(BUTTON_UP_PIN, &upDebounce, DEBOUNCE_DELAY);
    inputs.btDOWNPressed = HAL_debounceButton(BUTTON_DOWN_PIN, &downDebounce, DEBOUNCE_DELAY);
  }

  // Run application logic and update hardware state
  {
    ret = DeskApp_task(&inputs, &outputs);
  }

  // POST-Process outputs
  {
    HAL_ProcessAppState(ret, &outputs); // Ensure all outputs are processed
  }

#ifdef DEBUG
  Serial.print("App State: ");
  Serial.println(ret);
#endif
}
