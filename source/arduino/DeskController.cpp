/**
 * @file DeskController.cpp
 * @brief Desk Controller Application Logic Implementation
 * 
 * @module MODULE-003
 * @implements ARCH-COMP-001, ARCH-COMP-005
 * @see DeskController.h for interface documentation
 * @see 09_SoftwareDetailedDesign.md for detailed design
 * 
 * IMPLEMENTATION NOTE (v1.0):
 * This version implements minimal switch-to-motor control logic.
 * Full state machine (IDLE/MOVING_UP/MOVING_DOWN/DWELL/ERROR), timeout enforcement,
 * and safety interlocks are planned for v2.0.
 * 
 * Current v1.0 features:
 * - Direct rocker switch to motor mapping
 * - Safe initialization
 * - Basic state enum defined but not fully used
 * 
 * Deferred to v2.0:
 * - 30-second movement timeout (SWE-REQ-018)
 * - Direction reversal dwell period
 * - Emergency stop detection (SWE-REQ-010, 011)
 * - Error state machine (SWE-REQ-015, 016)
 */

#include "DeskController.h"

#if defined(ARDUINO)
  #include <Arduino.h>
#else
  #include <chrono>
  static unsigned long now_ms_host(void) {
    using namespace std::chrono;
    static const steady_clock::time_point start = steady_clock::now();
    return static_cast<unsigned long>(duration_cast<milliseconds>(steady_clock::now() - start).count());
  }
#endif

// Test interface support
#ifdef DESK_CONTROLLER_ENABLE_TEST_INTERFACE
  static unsigned long mockTimeMs = 0;
  #define TEST_EXPORT
  
  // Forward declare the struct for test interface
  typedef struct {
    int currentState;  // using int instead of AppState_t to avoid circular dependency
    unsigned long dwellStartMs;
    unsigned long movementStartMs;
  } DeskControllerInternalState_t;
#else
  #define TEST_EXPORT static
#endif

typedef enum {
  APP_STATE_IDLE = 0,
  APP_STATE_MOVE_UP,
  APP_STATE_MOVE_DOWN,
  APP_STATE_DWELL,
  APP_STATE_ERROR
} AppState_t;

static AppState_t appState = APP_STATE_IDLE;
static unsigned long dwellStartMs = 0;
static const unsigned long kDwellMs = 300UL;
static unsigned long movementStartMs = 0;
static const unsigned long kMovementTimeoutMs = 30000UL; // 30 seconds per SWE-REQ-018

static unsigned long now_ms(void) {
#ifdef DESK_CONTROLLER_ENABLE_TEST_INTERFACE
  return mockTimeMs;
#elif defined(ARDUINO)
  return millis();
#else
  return now_ms_host();
#endif
}


/**
 * @brief Initialize application logic to safe default state
 * @function FUNC-016
 * @implements SWE-REQ-001, SWE-REQ-002
 */
void DeskApp_task_init(const DeskAppInputs_t *inputs, DeskAppOutputs_t *outputs) {
  (void)inputs;
  if (outputs != NULL) {
    outputs->motor_enable = false;
    outputs->motor_direction = false;
    outputs->motor_pwm = 0;
  }
  appState = APP_STATE_IDLE;
}

/**
 * @brief Execute one step of application logic (v1.0 minimal implementation)
 * @function FUNC-017
 * @implements SWE-REQ-003, SWE-REQ-004, SWE-REQ-005, SWE-REQ-006
 * @algorithm ALG-001
 * 
 * Current v1.0 implementation:
 * - Reads rocker switch position
 * - Commands motor based on switch state
 * - No timeout, dwell, or complex FSM (planned for v2.0)
 */
DeskAppTask_Return_t DeskApp_task(const DeskAppInputs_t *inputs, DeskAppOutputs_t *outputs) {
  if (outputs == NULL || inputs == NULL) {
    return APP_TASK_ERROR;
  }

  // Default: stop motor
  outputs->motor_enable = false;
  outputs->motor_direction = false;
  outputs->motor_pwm = 0;

  if (inputs->switch_state == SWITCH_STATE_UP) {
    outputs->motor_enable = true;
    outputs->motor_direction = false; // UP direction
    outputs->motor_pwm = 255;
  } else if (inputs->switch_state == SWITCH_STATE_DOWN) {
    outputs->motor_enable = true;
    outputs->motor_direction = true; // DOWN direction
    outputs->motor_pwm = 255;
  }
  // If OFF, motor remains stopped
  return APP_TASK_SUCCESS;
}


