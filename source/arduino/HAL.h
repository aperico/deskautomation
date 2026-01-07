// HAL.h
// -----------------------------------------------------------------------------
// Hardware Abstraction Layer for Desk Automation Project
// Provides functions to control and read hardware components
// -----------------------------------------------------------------------------

#ifndef HAL_H
#define HAL_H

#if defined(ARDUINO)
    #define PLATFORM_ARDUINO 1
#else
    #define PLATFORM_HOST 1
#endif

#ifndef PLATFORM_ARDUINO
  #include "hal_mock/HALMock.h"
#endif

#include "DeskController.h"
#include "PinConfig.h"
#include <stdint.h>
#include <stdbool.h>

/* Configured in HAL.cpp to avoid multiple-definition from header inclusion */
extern const uint32_t BLINK_INTERVAL_MS;
extern const uint8_t  DEFAULT_MOTOR_SPEED;

/* HAL lifecycle */
void HAL_Init(void);
void HAL_Task(void); /* call periodically from main loop to handle blink/timers */
void HAL_ProcessAppState(const DeskAppTask_Return_t ret, const DeskAppOutputs_t *outputs);

/* LED controls */
void HAL_SetErrorLED(const bool state);
void HAL_SetWarningLED(const bool state);
void HAL_SetPowerLED(const bool state);
void HAL_SetMovingUpLED(const bool state);
void HAL_SetMovingDownLED(const bool state);
bool HAL_GetMovingUpLED(void);
bool HAL_GetMovingDownLED(void);
bool HAL_GetErrorLED(void);

/* Motor controls */
void HAL_MoveUp(const uint8_t speed);
void HAL_MoveDown(const uint8_t speed);
void HAL_StopMotor(void);

/* Debounce state for buttons */
typedef struct {
  bool     lastReading;       /* last raw reading (true = pressed) */
  bool     stableState;       /* debounced stable state */
  bool     changed;           /* set true when stableState changed since last call */
  uint32_t lastDebounceMs;    /* last time reading changed */
} DebounceState;

/* Button input + debounce
   Returns current debounced state (true = pressed). Updates state->changed flag. */
bool HAL_readButton(const int pin);
bool HAL_debounceButton(const int pin, DebounceState *state, const uint32_t debounceDelay);

/* Blink helpers (used by HAL_Task) */
void HAL_BlinkErrorLED(void);
void HAL_BlinkUPLED(void);
void HAL_BlinkDOWNLED(void);

#endif // HAL_H