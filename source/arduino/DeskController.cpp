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
  inputs->btUPPressed = FALSE;
  inputs->btDOWNPressed = FALSE;
  inputs->upperLimitActive = FALSE;
  inputs->lowerLimitActive = FALSE;

  // app outputs init
  outputs->moveUp = FALSE;
  outputs->moveDown = FALSE;
  outputs->stop = TRUE;
  outputs->error = FALSE;
}


DeskAppTask_Return_t DeskApp_task(const DeskAppInputs_t *inputs,
                                  DeskAppOutputs_t *outputs) {
  DeskAppTask_Return_t ret = APP_TASK_SUCCESS;

  // Default outputs
  outputs->moveUp = FALSE;
  outputs->moveDown = FALSE;
  outputs->stop = FALSE;

  switch (appState) {
  case APP_STATE_IDLE:
    // Idle: no motion unless a button is pressed
    outputs->error = FALSE;
    if (inputs->btUPPressed) {
      if (inputs->upperLimitActive == TRUE) {
        outputs->stop = TRUE;
        // remain IDLE due to upper limit
      } else {
        outputs->moveUp = TRUE;
        outputs->stop = FALSE;
        process_move_up();
      }
    } else if (inputs->btDOWNPressed) {
      if (inputs->lowerLimitActive == TRUE) {
        outputs->stop = TRUE;
        // remain IDLE due to lower limit
      } else {
        outputs->moveDown = TRUE;
        outputs->stop = FALSE;
        process_move_down();
      }
    } else {
      outputs->stop = TRUE;
      // remain in IDLE
    }
    break;
  case APP_STATE_MOVE_UP:
    outputs->moveUp = TRUE;
    if ((inputs->upperLimitActive == TRUE) || (inputs->btUPPressed == FALSE)) {
      if (inputs->btDOWNPressed == TRUE && inputs->lowerLimitActive == FALSE) {
        // immediate stop and enter dwell before reversing
        outputs->moveUp = FALSE;
        outputs->moveDown = FALSE;
        outputs->stop = TRUE;
        process_dwell();
      } else {
        process_stop();
      }
    }

    if (inputs->lowerLimitActive == TRUE) {
      appState = APP_STATE_ERROR;
    }
    break;
  case APP_STATE_MOVE_DOWN:
    outputs->moveDown = TRUE;

    if ((inputs->lowerLimitActive == TRUE) ||
        (inputs->btDOWNPressed == FALSE)) {
      if (inputs->btUPPressed == TRUE && inputs->upperLimitActive == FALSE) {
        // immediate stop and enter dwell before reversing
        outputs->moveUp = FALSE;
        outputs->moveDown = FALSE;
        outputs->stop = TRUE;
        process_dwell();
      } else {
        process_stop();
      }
    }
    if (inputs->upperLimitActive == TRUE) {
      appState = APP_STATE_ERROR;
    }

    break;
  case APP_STATE_DWELL:
    outputs->stop = TRUE;
    outputs->error = FALSE;
    if (now_ms() - dwellStartMs >= DWELL_TIME_MS) {
      if (inputs->btUPPressed == TRUE && inputs->upperLimitActive == FALSE) {
        outputs->moveUp = TRUE;
        outputs->stop = FALSE;
        process_move_up();
      } else if (inputs->btDOWNPressed == TRUE && inputs->lowerLimitActive == FALSE) {
        outputs->moveDown = TRUE;
        outputs->stop = FALSE;
        process_move_down();
      } else {
        process_stop();
      }
    }
    break;
  case APP_STATE_ERROR:
    outputs->stop = TRUE;
    // Latched error until safe conditions are met
    if ((inputs->btUPPressed == FALSE) && (inputs->btDOWNPressed == FALSE) &&
        (inputs->upperLimitActive == FALSE) && (inputs->lowerLimitActive == FALSE)) {
      // Clear error and return to idle when no inputs and no limits
      outputs->error = FALSE;
      appState = APP_STATE_IDLE;
    } else {
      outputs->error = TRUE;
    }
    break;
  default:
    appState = APP_STATE_IDLE;
    outputs->stop = TRUE;
    break;
  }

  return ret;
}
