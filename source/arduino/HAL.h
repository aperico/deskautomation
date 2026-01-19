/**
 * @file HAL.h
 * @brief Hardware Abstraction Layer for Desk Automation Project
 * 
 * @module MODULE-002
 * @implements ARCH-COMP-002
 * @requirements SWE-REQ-001, SWE-REQ-003, SWE-REQ-004, SWE-REQ-012, SWE-REQ-013, SWE-REQ-017
 * @architecture 08_SoftwareArchitecture.md
 * @detailed_design 09_SoftwareDetailedDesign.md
 * 
 * Provides hardware abstraction for:
 * - IBT-2/BTS7960 motor driver control
 * - ON/OFF/ON rocker switch input
 * - Current sensing (analog inputs)
 * - Error detection and handling
 */

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

/**
 * @brief Read the ON/OFF/ON switch state (UP, OFF, DOWN)
 * @function FUNC-002
 * @implements SWE-REQ-003, SWE-REQ-004
 * @returns SwitchState_t - SWITCH_STATE_UP, SWITCH_STATE_OFF, or SWITCH_STATE_DOWN
 */
#include "DeskController.h" // for SwitchState_t
SwitchState_t HAL_ReadSwitchState(void);
/**
 * @brief Query if HAL has detected a hardware error (overcurrent, stall, no-load)
 * @function FUNC-008
 * @implements SWE-REQ-015, SWE-REQ-021
 * @returns true if error detected, false otherwise
 */
bool HAL_HasError(void);

/**
 * @brief Clear the HAL error state (after user/application acknowledges)
 * @function FUNC-009
 * @implements SWE-REQ-016
 */
void HAL_ClearError(void);
// HAL.h
// -----------------------------------------------------------------------------
// Hardware Abstraction Layer for Desk Automation Project
// Provides functions to control and read hardware components
// --

typedef struct {
  int r_current;         /**< Right current sense (ADC value) */
  int l_current;         /**< Left current sense (ADC value) */
  bool up_switch;       /**< UP switch state (true=pressed) */
  bool down_switch;     /**< DOWN switch state (true=pressed) */
} HAL_Ouputs_t;

/* HAL configuration constants are internal to HAL implementation; do not export globals here */

/**
 * @brief Initialize HAL subsystem and configure pins
 * @function FUNC-001
 * @implements SWE-REQ-001
 * @architecture ARCH-IF-002
 * 
 * Configures all GPIO pins for motor driver, switch inputs, and current sensing.
 * Sets all outputs to safe default states (motor stopped).
 */
void HAL_init(void);

/**
 * @brief Periodic HAL task to process timed events (blinking, timeouts)
 *
 * Call from main loop at least as often as the blink interval.
 */
void HAL_Task(HAL_Ouputs_t *hal_outputs, bool motor_enable, uint8_t motor_pwm);

/**
 * @brief Apply application outputs to hardware and enforce prioritization
 *
 * @param ret Application task return code
 * @param outputs Pointer to application outputs (may be NULL)
 */
void HAL_ProcessAppState(const DeskAppTask_Return_t ret, const DeskAppOutputs_t *outputs, HAL_Ouputs_t *hal_outputs);

float HAL_adc_to_amps(int adc_value, float vref = 5.0, float volts_per_amp = 1.0);


/**
 * @brief Motor controls for IBT-2/BTS7960 driver
 * @function FUNC-003 (MoveUp), FUNC-004 (MoveDown), FUNC-005 (StopMotor)
 * @implements SWE-REQ-005, SWE-REQ-006, SWE-REQ-007, SWE-REQ-008
 * @param speed PWM value (0-255) for MoveUp/MoveDown
 */
void HAL_MoveUp(const uint8_t speed);
void HAL_MoveDown(const uint8_t speed);
void HAL_StopMotor(void);


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



#endif // HAL_H