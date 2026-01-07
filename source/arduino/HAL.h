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

/* HAL configuration constants are internal to HAL implementation; do not export globals here */

/**
 * @brief Initialize HAL subsystem and configure pins
 */
void HAL_init(void);

/**
 * @brief Periodic HAL task to process timed events (blinking, timeouts)
 *
 * Call from main loop at least as often as the blink interval.
 */
void HAL_Task(void);

/**
 * @brief Apply application outputs to hardware and enforce prioritization
 *
 * @param ret Application task return code
 * @param outputs Pointer to application outputs (may be NULL)
 */
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
bool HAL_debounceButton(const int pin, DebounceState *state, const uint32_t debounceDelay);

/**
 * @brief Convenience wrapper: perform debounce for a named pin and return stable state
 *
 * This wrapper centralizes debounce defaults and reduces boilerplate in application code.
 */
bool HAL_readDebounced(const int pin);

/**
 * @brief Wait during startup for a short period while hardware settles
 *
 * Implementations may be blocking for a short time (e.g. 1000ms) and are intended
 * to be called from `setup()` only.
 */
void HAL_wait_startup(void);

/**
 * @brief Logger callback signature. If set, HAL_LOG will call this instead of Serial.
 */
typedef void (*HAL_Logger_t)(const char *msg);

/**
 * @brief Set an optional logger callback for HAL diagnostic output
 */
void HAL_set_logger(HAL_Logger_t logger);

/* Blink helpers (used by HAL_Task) */
void HAL_BlinkErrorLED(void);
void HAL_BlinkUPLED(void);
void HAL_BlinkDOWNLED(void);

#endif // HAL_H