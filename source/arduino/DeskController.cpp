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

// Expose constants for test validation (v2.0 tests)
#ifdef DESK_CONTROLLER_ENABLE_TEST_INTERFACE
  extern "C" const unsigned long kDwellMs_test = 300UL;
  extern "C" const unsigned long kMovementTimeoutMs_test = 30000UL;
#endif

static unsigned long now_ms(void) {
#ifdef DESK_CONTROLLER_ENABLE_TEST_INTERFACE
  return mockTimeMs;
#elif defined(ARDUINO)
  return millis();
#else
  return now_ms_host();
#endif
}

// ============================================================================
// Private State Handler Functions (exposed for white-box testing)
// ============================================================================

/**
 * @brief Handle IDLE state: wait for user input to begin movement
 * @private
 * @test_exposure Exposed as handle_idle_test() for white-box unit testing
 */
TEST_EXPORT void handle_idle(const DeskAppInputs_t *inputs, DeskAppOutputs_t *outputs) {
  if (outputs == NULL) {
    return;
  }

  // Default: stay off
  outputs->motor_enable = false;
  outputs->motor_direction = false;
  outputs->motor_pwm = 0;

  if (inputs != NULL) {
    if (inputs->switch_state == SWITCH_STATE_UP) {
      outputs->motor_enable = true;
      outputs->motor_direction = false;
      outputs->motor_pwm = 255;
      movementStartMs = now_ms();
      appState = APP_STATE_MOVE_UP;
    } else if (inputs->switch_state == SWITCH_STATE_DOWN) {
      outputs->motor_enable = true;
      outputs->motor_direction = true;
      outputs->motor_pwm = 255;
      movementStartMs = now_ms();
      appState = APP_STATE_MOVE_DOWN;
    }
  }
}

/**
 * @brief Handle MOVE_UP state: keep moving up until timeout or direction change
 * @private
 * @test_exposure Exposed as handle_move_up_test() for white-box unit testing
 */
TEST_EXPORT void handle_move_up(const DeskAppInputs_t *inputs, DeskAppOutputs_t *outputs) {
  if (outputs == NULL) {
    return;
  }

  // Check timeout (v2.0 feature)
  if (now_ms() - movementStartMs > kMovementTimeoutMs) {
    outputs->motor_enable = false;
    outputs->motor_pwm = 0;
    appState = APP_STATE_IDLE;
    return;
  }

  if (inputs == NULL) {
    outputs->motor_enable = false;
    outputs->motor_pwm = 0;
    return;
  }

  if (inputs->switch_state == SWITCH_STATE_OFF) {
    // Release switch → stop
    outputs->motor_enable = false;
    outputs->motor_pwm = 0;
    appState = APP_STATE_IDLE;
  } else if (inputs->switch_state == SWITCH_STATE_DOWN) {
    // Reverse direction → enter dwell
    outputs->motor_enable = false;
    outputs->motor_pwm = 0;
    dwellStartMs = now_ms();
    appState = APP_STATE_DWELL;
  } else {
    // Continue moving up
    outputs->motor_enable = true;
    outputs->motor_direction = false;
    outputs->motor_pwm = 255;
  }
}

/**
 * @brief Handle MOVE_DOWN state: keep moving down until timeout or direction change
 * @private
 * @test_exposure Exposed as handle_move_down_test() for white-box unit testing
 */
TEST_EXPORT void handle_move_down(const DeskAppInputs_t *inputs, DeskAppOutputs_t *outputs) {
  if (outputs == NULL) {
    return;
  }

  // Check timeout (v2.0 feature)
  if (now_ms() - movementStartMs > kMovementTimeoutMs) {
    outputs->motor_enable = false;
    outputs->motor_pwm = 0;
    appState = APP_STATE_IDLE;
    return;
  }

  if (inputs == NULL) {
    outputs->motor_enable = false;
    outputs->motor_pwm = 0;
    return;
  }

  if (inputs->switch_state == SWITCH_STATE_OFF) {
    // Release switch → stop
    outputs->motor_enable = false;
    outputs->motor_pwm = 0;
    appState = APP_STATE_IDLE;
  } else if (inputs->switch_state == SWITCH_STATE_UP) {
    // Reverse direction → enter dwell
    outputs->motor_enable = false;
    outputs->motor_pwm = 0;
    dwellStartMs = now_ms();
    appState = APP_STATE_DWELL;
  } else {
    // Continue moving down
    outputs->motor_enable = true;
    outputs->motor_direction = true;
    outputs->motor_pwm = 255;
  }
}

/**
 * @brief Handle DWELL state: motor off, waiting for direction reversal settling
 * @private
 * @test_exposure Exposed as handle_dwell_test() for white-box unit testing
 */
TEST_EXPORT void handle_dwell(const DeskAppInputs_t *inputs, DeskAppOutputs_t *outputs) {
  if (outputs == NULL) {
    return;
  }

  // Always off during dwell
  outputs->motor_enable = false;
  outputs->motor_pwm = 0;

  // Check if dwell period is complete
  if (now_ms() - dwellStartMs >= kDwellMs) {
    // Dwell complete, transition based on switch
    if (inputs != NULL) {
      if (inputs->switch_state == SWITCH_STATE_UP) {
        outputs->motor_enable = true;
        outputs->motor_direction = false;
        outputs->motor_pwm = 255;
        movementStartMs = now_ms();
        appState = APP_STATE_MOVE_UP;
      } else if (inputs->switch_state == SWITCH_STATE_DOWN) {
        outputs->motor_enable = true;
        outputs->motor_direction = true;
        outputs->motor_pwm = 255;
        movementStartMs = now_ms();
        appState = APP_STATE_MOVE_DOWN;
      } else {
        // Switch released → return to idle
        appState = APP_STATE_IDLE;
      }
    } else {
      appState = APP_STATE_IDLE;
    }
  }
}

// ============================================================================
// Test Interface Implementation
// ============================================================================

#ifdef DESK_CONTROLLER_ENABLE_TEST_INTERFACE

// Expose state handler functions for white-box testing
extern "C" {
  void handle_idle_test(const DeskAppInputs_t *inputs, DeskAppOutputs_t *outputs) {
    handle_idle(inputs, outputs);
  }

  void handle_move_up_test(const DeskAppInputs_t *inputs, DeskAppOutputs_t *outputs) {
    handle_move_up(inputs, outputs);
  }

  void handle_move_down_test(const DeskAppInputs_t *inputs, DeskAppOutputs_t *outputs) {
    handle_move_down(inputs, outputs);
  }

  void handle_dwell_test(const DeskAppInputs_t *inputs, DeskAppOutputs_t *outputs) {
    handle_dwell(inputs, outputs);
  }

  void DeskApp_get_internal_state(DeskControllerInternalState_t *state) {
    if (state != NULL) {
      state->currentState = (int)appState;
      state->dwellStartMs = dwellStartMs;
      state->movementStartMs = movementStartMs;
    }
  }

  void DeskApp_set_internal_state(const DeskControllerInternalState_t *state) {
    if (state != NULL) {
      appState = (AppState_t)state->currentState;
      dwellStartMs = state->dwellStartMs;
      movementStartMs = state->movementStartMs;
    }
  }

  void DeskApp_set_time_ms(unsigned long timeMs) {
    mockTimeMs = timeMs;
  }

  unsigned long DeskApp_get_time_ms(void) {
    return mockTimeMs;
  }
}

#endif


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


