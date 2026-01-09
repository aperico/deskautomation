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

/* Forward declarations for per-state handlers */
TEST_EXPORT void handle_idle(const DeskAppInputs_t *inputs, DeskAppOutputs_t *outputs);
TEST_EXPORT void handle_move_up(const DeskAppInputs_t *inputs, DeskAppOutputs_t *outputs);
TEST_EXPORT void handle_move_down(const DeskAppInputs_t *inputs, DeskAppOutputs_t *outputs);
TEST_EXPORT void handle_dwell(const DeskAppInputs_t *inputs, DeskAppOutputs_t *outputs);

void DeskApp_task_init(const DeskAppInputs_t *inputs, DeskAppOutputs_t *outputs) {
  (void)inputs;
  if (outputs != NULL) {
    outputs->moveUp = false;
    outputs->moveDown = false;
    outputs->stop = true;
    outputs->error = false;
  }
  appState = APP_STATE_IDLE;
  dwellStartMs = 0;
}

DeskAppTask_Return_t DeskApp_task(const DeskAppInputs_t *inputs, DeskAppOutputs_t *outputs) {
  /* defensive: require valid pointers */
  if (inputs == NULL || outputs == NULL) {
    return APP_TASK_ERROR;
  }

  /* default outputs */
  outputs->moveUp = false;
  outputs->moveDown = false;
  outputs->stop = false;
  outputs->error = false;

  /* invalid sensor combination -> fatal error state */
  if (inputs->upperLimitActive && inputs->lowerLimitActive) {
    outputs->stop = true;
    outputs->error = true;
    appState = APP_STATE_ERROR;
    return APP_TASK_ERROR;
  }

  /* simultaneous buttons -> safe stop, remain non-fatal (IDLE) */
  if (inputs->btUPPressed && inputs->btDOWNPressed) {
    outputs->stop = true;
    outputs->error = false;
    appState = APP_STATE_IDLE;
    return APP_TASK_SUCCESS;
  }

  /* dispatch to per-state handler for clarity */
  switch (appState) {
    case APP_STATE_IDLE:
      handle_idle(inputs, outputs);
      break;

    case APP_STATE_MOVE_UP:
      handle_move_up(inputs, outputs);
      break;

    case APP_STATE_MOVE_DOWN:
      handle_move_down(inputs, outputs);
      break;

    case APP_STATE_DWELL:
      handle_dwell(inputs, outputs);
      break;

    case APP_STATE_ERROR:
    default:
      outputs->stop = true;
      outputs->error = true;
      return APP_TASK_ERROR;
  }

  return APP_TASK_SUCCESS;
}

/* State handlers — keep each small and focused for easier testing */
TEST_EXPORT void handle_idle(const DeskAppInputs_t *inputs, DeskAppOutputs_t *outputs) {
  // Both buttons pressed is a safety condition - stop and stay in IDLE
  if (inputs->btUPPressed && inputs->btDOWNPressed) {
    outputs->stop = true;
    return;
  }
  
  if (inputs->btUPPressed) {
    if (inputs->upperLimitActive) {
      outputs->stop = true;
    } else {
      outputs->moveUp = true;
      outputs->stop = false;  // Explicitly clear stop when starting movement
      appState = APP_STATE_MOVE_UP;
      movementStartMs = now_ms(); // Start timeout timer (SWE-REQ-018)
    }
  } else if (inputs->btDOWNPressed) {
    if (inputs->lowerLimitActive) {
      outputs->stop = true;
    } else {
      outputs->moveDown = true;
      outputs->stop = false;  // Explicitly clear stop when starting movement
      appState = APP_STATE_MOVE_DOWN;
      movementStartMs = now_ms(); // Start timeout timer (SWE-REQ-018)
    }
  } else {
    outputs->stop = true;
  }
}

TEST_EXPORT void handle_move_up(const DeskAppInputs_t *inputs, DeskAppOutputs_t *outputs) {
  // Check timeout first (SWE-REQ-018)
  if ((now_ms() - movementStartMs) >= kMovementTimeoutMs) {
    outputs->stop = true;
    appState = APP_STATE_IDLE;
    return;
  }
  
  if (inputs->upperLimitActive) {
    outputs->stop = true;
    appState = APP_STATE_DWELL;
    dwellStartMs = now_ms();
  } else if (!inputs->btUPPressed) {
    outputs->stop = true;
    appState = APP_STATE_IDLE;
  } else {
    outputs->moveUp = true;
    outputs->stop = false;  // Explicitly clear stop during normal movement
  }
}

TEST_EXPORT void handle_move_down(const DeskAppInputs_t *inputs, DeskAppOutputs_t *outputs) {
  // Check timeout first (SWE-REQ-018)
  if ((now_ms() - movementStartMs) >= kMovementTimeoutMs) {
    outputs->stop = true;
    appState = APP_STATE_IDLE;
    return;
  }
  
  if (inputs->lowerLimitActive) {
    outputs->stop = true;
    appState = APP_STATE_DWELL;
    dwellStartMs = now_ms();
  } else if (!inputs->btDOWNPressed) {
    outputs->stop = true;
    appState = APP_STATE_IDLE;
  } else {
    outputs->moveDown = true;
    outputs->stop = false;  // Explicitly clear stop during normal movement
  }
}

TEST_EXPORT void handle_dwell(const DeskAppInputs_t *inputs, DeskAppOutputs_t *outputs) {
  (void)inputs;
  if ((now_ms() - dwellStartMs) >= kDwellMs) {
    appState = APP_STATE_IDLE;
  }
  outputs->stop = true;
}

// Test interface implementation
#ifdef DESK_CONTROLLER_ENABLE_TEST_INTERFACE

#ifdef __cplusplus
extern "C" {
#endif

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
    state->currentState = static_cast<int>(appState);
    state->dwellStartMs = dwellStartMs;
    state->movementStartMs = movementStartMs;
  }
}

void DeskApp_set_internal_state(const DeskControllerInternalState_t *state) {
  if (state != NULL) {
    appState = static_cast<AppState_t>(state->currentState);
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

// Export constants with external linkage
extern "C" const unsigned long kDwellMs_test = kDwellMs;
extern "C" const unsigned long kMovementTimeoutMs_test = kMovementTimeoutMs;

#ifdef __cplusplus
}
#endif

#endif // DESK_CONTROLLER_ENABLE_TEST_INTERFACE
