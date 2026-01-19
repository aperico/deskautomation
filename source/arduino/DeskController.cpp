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



void DeskApp_task_init(const DeskAppInputs_t *inputs, DeskAppOutputs_t *outputs) {
  (void)inputs;
  if (outputs != NULL) {
    outputs->motor_enable = false;
    outputs->motor_direction = false;
    outputs->motor_pwm = 0;
  }
  appState = APP_STATE_IDLE;
}


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


