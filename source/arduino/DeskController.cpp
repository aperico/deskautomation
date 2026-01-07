#include "DeskController.h"

#if defined(ARDUINO)
  #include <Arduino.h>
#else
  #include <time.h>
  static unsigned long now_ms_host(void) { return (unsigned long)(clock() * 1000 / CLOCKS_PER_SEC); }
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

static unsigned long now_ms(void) {
#if defined(ARDUINO)
  return millis();
#else
  return now_ms_host();
#endif
}

/* Forward declarations for per-state handlers */
static void handle_idle(const DeskAppInputs_t *inputs, DeskAppOutputs_t *outputs);
static void handle_move_up(const DeskAppInputs_t *inputs, DeskAppOutputs_t *outputs);
static void handle_move_down(const DeskAppInputs_t *inputs, DeskAppOutputs_t *outputs);
static void handle_dwell(const DeskAppInputs_t *inputs, DeskAppOutputs_t *outputs);

void DeskApp_task_init(DeskAppInputs_t *inputs, DeskAppOutputs_t *outputs) {
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
static void handle_idle(const DeskAppInputs_t *inputs, DeskAppOutputs_t *outputs) {
  if (inputs->btUPPressed) {
    if (inputs->upperLimitActive) {
      outputs->stop = true;
    } else {
      outputs->moveUp = true;
      appState = APP_STATE_MOVE_UP;
    }
  } else if (inputs->btDOWNPressed) {
    if (inputs->lowerLimitActive) {
      outputs->stop = true;
    } else {
      outputs->moveDown = true;
      appState = APP_STATE_MOVE_DOWN;
    }
  } else {
    outputs->stop = true;
  }
}

static void handle_move_up(const DeskAppInputs_t *inputs, DeskAppOutputs_t *outputs) {
  if (inputs->upperLimitActive) {
    outputs->stop = true;
    appState = APP_STATE_DWELL;
    dwellStartMs = now_ms();
  } else if (!inputs->btUPPressed) {
    outputs->stop = true;
    appState = APP_STATE_IDLE;
  } else {
    outputs->moveUp = true;
  }
}

static void handle_move_down(const DeskAppInputs_t *inputs, DeskAppOutputs_t *outputs) {
  if (inputs->lowerLimitActive) {
    outputs->stop = true;
    appState = APP_STATE_DWELL;
    dwellStartMs = now_ms();
  } else if (!inputs->btDOWNPressed) {
    outputs->stop = true;
    appState = APP_STATE_IDLE;
  } else {
    outputs->moveDown = true;
  }
}

static void handle_dwell(const DeskAppInputs_t *inputs, DeskAppOutputs_t *outputs) {
  (void)inputs;
  if ((now_ms() - dwellStartMs) >= kDwellMs) {
    appState = APP_STATE_IDLE;
  }
  outputs->stop = true;
}
