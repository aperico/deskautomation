/**
 * @file arduino.ino
 * @brief Main application file for Desk Automation Project
 * 
 * @module MODULE-004
 * @implements ARCH-COMP-004
 * @requirements SWE-REQ-001, SWE-REQ-009
 * @architecture 08_SoftwareArchitecture.md
 * @detailed_design 09_SoftwareDetailedDesign.md
 * 
 * Handles:
 * - System initialization (FUNC-025)
 * - Main control loop (FUNC-026)
 * - Integration of HAL and application logic
 * - Debug output and diagnostics
 * 
 * Hardware Wiring:
 * 3-Position Rocker Switch (ON-OFF-ON) to Arduino:
 *   [ 7 ]---+         +---[ GND ]
 *           |         |
 *   [ 8 ]---+         +---[ GND ]
 * 
 * Switch positions:
 *   - UP:   Pin 7 LOW, Pin 8 HIGH
 *   - DOWN: Pin 8 LOW, Pin 7 HIGH
 *   - OFF:  Both pins HIGH
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
/**
 * @brief Arduino initialization entry point
 * @function FUNC-025
 * @implements SWE-REQ-001
 * 
 * Initialization sequence:
 * 1. HAL_init() - Configure hardware pins (FUNC-001)
 * 2. HAL_wait_startup() - Brief settling delay
 * 3. DeskApp_task_init() - Initialize application to safe state (FUNC-016)
 * 4. Serial debug setup (if DEBUG defined)
 */
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

/**
 * @brief Main control loop, executes continuously
 * @function FUNC-026
 * @implements SWE-REQ-013, SWE-REQ-019
 * 
 * Loop sequence:
 * 1. Read switch state via HAL_ReadSwitchState() (FUNC-002)
 * 2. Populate application inputs (DATA-001)
 * 3. Execute application logic via DeskApp_task() (FUNC-017)
 * 4. Apply outputs to hardware via HAL_ProcessAppState()
 * 5. Debug logging (if DEBUG defined)
 * 
 * Timing: Target loop time 10-50ms for responsive control
 */
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
