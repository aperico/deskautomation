/**
 * @file hal.h
 * @brief Hardware Abstraction Layer for Desk Automation System
 * 
 * Provides hardware-independent interface for:
 * - Button input reading (UP/DOWN rocker switch)
 * - Limit sensor reading (upper/lower mechanical limits)
 * - Motor control (L298N dual H-bridge driver)
 * - LED status indicators (3× independent LEDs)
 * - System timing (millisecond counter)
 * 
 * @hardware L298N motor driver, Arduino UNO platform
 * @version 1.1
 * @date 2026-01-25
 */

#ifndef HAL_H
#define HAL_H

#include "desk_types.h"
#include "pin_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize hardware abstraction layer
 * 
 * Configures all GPIO pins and peripherals:
 * - Input pins: Buttons and limit sensors (with pull-ups)
 * - Output pins: Motor driver control signals
 * - Output pins: 3× Status LEDs
 * - Timer: millis() counter
 * 
 * @preconditions Arduino hardware powered and ready
 * @postconditions All pins configured; system ready for operation
 */
void HAL_init(void);

/**
 * @brief Read button state (UP or DOWN)
 * 
 * @param button - Button identifier (BUTTON_UP or BUTTON_DOWN)
 * @return bool - true if pressed (active LOW), false if released
 */
bool HAL_readButton(ButtonID_t button);

/**
 * @brief Read limit sensor state (upper or lower)
 * 
 * @param sensor - Sensor identifier (LIMIT_UPPER or LIMIT_LOWER)
 * @return bool - true if triggered (active LOW), false if open
 */
bool HAL_readLimitSensor(LimitID_t sensor);

/**
 * @brief Set motor direction and speed
 * 
 * Controls L298N motor driver via PWM and enable signals.
 * 
 * @param dir - Motor direction (MOTOR_STOP, MOTOR_UP, MOTOR_DOWN)
 * @param speed - PWM duty cycle (0-255, where 0=stop, 255=full speed)
 */
void HAL_setMotor(MotorDirection_t dir, uint8_t speed);

/**
 * @brief Set individual LED state
 * 
 * Controls one of the 3 status LEDs independently.
 * 
 * @param led - LED identifier (LED_BT_UP, LED_BT_DOWN, LED_ERROR)
 * @param state - LED state (LED_OFF or LED_ON)
 * 
 * @usage
 * ```c
 * HAL_setLED(LED_BT_UP, LED_ON);      // Turn on UP button indicator
 * HAL_setLED(LED_ERROR, LED_ON);      // Turn on error indicator
 * HAL_setLED(LED_BT_DOWN, LED_OFF);   // Turn off DOWN button indicator
 * ```
 */
void HAL_setLED(LEDID_t led, LEDState_t state);

/**
 * @brief Get system time in milliseconds
 * 
 * @return uint32_t - Milliseconds since system startup (wraps after ~49 days)
 */
uint32_t HAL_getTime(void);

#ifdef __cplusplus
}
#endif

#endif 
