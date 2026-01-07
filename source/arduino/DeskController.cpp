#include "DeskController.h"

typedef enum {
  APP_STATE_IDLE = 0,
  APP_STATE_MOVE_UP,
  APP_STATE_MOVE_DOWN,
  APP_STATE_DWELL,
  APP_STATE_ERROR
} AppState_t;

static AppState_t appState = APP_STATE_IDLE;

void process_move_up() { appState = APP_STATE_MOVE_UP; }

void process_move_down() { appState = APP_STATE_MOVE_DOWN; }

void process_stop() { appState = APP_STATE_IDLE; }

#if defined(ARDUINO)
#include <Arduino.h>
static unsigned long now_ms() { return millis(); }
#else
static unsigned long fake_ms = 0;
static unsigned long now_ms() { fake_ms += 100; return fake_ms; }
#endif

static const unsigned long DWELL_TIME_MS = 200;
static unsigned long dwellStartMs = 0;
static void process_dwell() { appState = APP_STATE_DWELL; dwellStartMs = now_ms(); }

void DeskApp_task_init(DeskAppInputs_t *inputs,
                                  DeskAppOutputs_t *outputs){
  // app inputs init
  inputs->btUPPressed = false;
  inputs->btDOWNPressed = false;
  inputs->upperLimitActive = false;
  inputs->lowerLimitActive = false;

  // app outputs init
  outputs->moveUp = false;
  outputs->moveDown = false;
  outputs->stop = true;
  outputs->error = false;
}


DeskAppTask_Return_t DeskApp_task(const DeskAppInputs_t *inputs,
                                  DeskAppOutputs_t *outputs) {
  DeskAppTask_Return_t ret = APP_TASK_SUCCESS;

  // Clear outputs by default
  outputs->moveUp = false;
  outputs->moveDown = false;
  outputs->stop = false;
  outputs->error = false;

  // --- Both limits active (lockout) ---
  if (inputs->upperLimitActive && inputs->lowerLimitActive) {
    outputs->error = true;
    appState = APP_STATE_ERROR;
    return APP_TASK_ERROR;
  }

  // --- Emergency stop: moving up but lower limit active, or moving down but upper limit active ---
  if ((appState == APP_STATE_MOVE_UP && inputs->lowerLimitActive) ||
      (appState == APP_STATE_MOVE_DOWN && inputs->upperLimitActive)) {
    outputs->moveUp = false;
    outputs->moveDown = false;
    outputs->stop = true;
    outputs->error = true;
    appState = APP_STATE_ERROR;
    return APP_TASK_ERROR;
  }

  switch (appState) {
  case APP_STATE_IDLE:
    outputs->error = false;
    // UC-07: If both buttons pressed, no movement should be commanded
    if (inputs->btUPPressed && inputs->btDOWNPressed) {
      outputs->moveUp = false;
      outputs->moveDown = false;
      outputs->stop = true;
      // remain in IDLE
    } else if (inputs->btUPPressed) {
      if (inputs->upperLimitActive == true) {
        outputs->stop = true;
        // remain IDLE due to upper limit
      } else {
        outputs->moveUp = true;
        outputs->stop = false;
        process_move_up();
      }
    } else if (inputs->btDOWNPressed) {
      if (inputs->lowerLimitActive == true) {
        outputs->stop = true;
        // remain IDLE due to lower limit
      } else {
        outputs->moveDown = true;
        outputs->stop = false;
        process_move_down();
      }
    } else {
      outputs->stop = true;
      // remain in IDLE
    }
    break;
  case APP_STATE_MOVE_UP:
    // UC-07: If both buttons pressed, stop movement
    if (inputs->btUPPressed && inputs->btDOWNPressed) {
      outputs->moveUp = false;
      outputs->moveDown = false;
      outputs->stop = true;
      process_stop();
      break;
    }
    outputs->moveUp = true;
    if ((inputs->upperLimitActive == true) || (inputs->btUPPressed == false)) {
      if (inputs->btDOWNPressed == true && inputs->lowerLimitActive == false) {
        // immediate stop and enter dwell before reversing
        outputs->moveUp = false;
        outputs->moveDown = false;
        outputs->stop = true;
        process_dwell();
      } else {
        process_stop();
      }
    }
    // Emergency stop if lower limit becomes active while moving up
    if (inputs->lowerLimitActive == true) {
      outputs->moveUp = false;
      outputs->moveDown = false;
      outputs->stop = true;
      outputs->error = true;
      appState = APP_STATE_ERROR;
      return APP_TASK_ERROR;
    }
    break;
  case APP_STATE_MOVE_DOWN:
    // UC-07: If both buttons pressed, stop movement
    if (inputs->btUPPressed && inputs->btDOWNPressed) {
      outputs->moveUp = false;
      outputs->moveDown = false;
      outputs->stop = true;
      process_stop();
      break;
    }
    outputs->moveDown = true;
    if ((inputs->lowerLimitActive == true) ||
        (inputs->btDOWNPressed == false)) {
      if (inputs->btUPPressed == true && inputs->upperLimitActive == false) {
        // immediate stop and enter dwell before reversing
        outputs->moveUp = false;
        outputs->moveDown = false;
        outputs->stop = true;
        process_dwell();
      } else {
        process_stop();
      }
    }
    // Emergency stop if upper limit becomes active while moving down
    if (inputs->upperLimitActive == true) {
      outputs->moveUp = false;
      outputs->moveDown = false;
      outputs->stop = true;
      outputs->error = true;
      appState = APP_STATE_ERROR;
      return APP_TASK_ERROR;
    }
    break;
  case APP_STATE_DWELL:
    outputs->stop = true;
    outputs->error = false;
    if (now_ms() - dwellStartMs >= DWELL_TIME_MS) {
      if (inputs->btUPPressed == true && inputs->upperLimitActive == false) {
        outputs->moveUp = true;
        outputs->stop = false;
        process_move_up();
      } else if (inputs->btDOWNPressed == true && inputs->lowerLimitActive == false) {
        outputs->moveDown = true;
        outputs->stop = false;
        process_move_down();
      } else {
        process_stop();
      }
    }
    break;
  case APP_STATE_ERROR:
    outputs->stop = true;
    // Latched error until safe conditions are met
    if ((inputs->btUPPressed == false) && (inputs->btDOWNPressed == false) &&
        (inputs->upperLimitActive == false) && (inputs->lowerLimitActive == false)) {
      // Clear error and return to idle when no inputs and no limits
      outputs->error = false;
      appState = APP_STATE_IDLE;
    } else {
      outputs->error = true;
    }
    break;
  default:
    appState = APP_STATE_IDLE;
    outputs->stop = true;
    break;
  }

  return ret;
}
