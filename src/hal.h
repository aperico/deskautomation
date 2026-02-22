/**
 * @file hal.h
 * @brief Hardware Abstraction Layer for Desk Automation System
 * 
 * Provides hardware-independent interface for:
 * - Button input reading (UP/DOWN rocker switch)
 * - Limit sensor reading (upper/lower mechanical limits)
 * - Motor control (configurable driver: L298N or IBT_2)
 * - LED status indicators (3× independent LEDs)
 * - System timing (millisecond counter)
 * 
 * @motor_driver Configurable via motor_config.h MOTOR_TYPE:
 *   - MT_BASIC: L298N dual H-bridge (3-pin control: EN1, EN2, PWM)
 *   - MT_ROBUST: IBT_2 intelligent driver (2-pin PWM: LPWM, RPWM)
 * 
 * @hardware Arduino UNO platform with configurable motor driver backend
 * @version 2.0
 * @date 2026-02-21
 * @changes Added support for configurable motor drivers (L298N & IBT_2)
 */

#ifndef HAL_H
#define HAL_H

#include "desk_types.h"
#include "motor_config.h"
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
 * @brief Set motor driver type for runtime motor control adaptation
 * 
 * Configures HAL to use the correct pin initialization, control logic,
 * and current sensing based on the specified motor driver type.
 * Must be called before HAL_init() or immediately after if motor type
 * needs to change at runtime.
 * 
 * @param motor_type - Motor type identifier (MT_BASIC for L298N, MT_ROBUST for IBT_2)
 * 
 * @postconditions HAL functions adapt to specified motor driver hardware
 */
void HAL_setMotorType(MotorType_t motor_type);

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
 * @brief Read motor current sense (mA)
 * 
 * @return uint16_t - Motor current in milliamps
 */
uint16_t HAL_readMotorCurrent(void);

/**
 * @brief Set motor direction and speed
 * 
 * Controls motor driver (L298N or IBT_2 based on motor_config.h MOTOR_TYPE).
 * Provides uniform interface regardless of underlying driver implementation.
 * 
 * Implementation details (hardware-specific):
 * - L298N (MT_BASIC): Controls via EN1, EN2 (direction) and PWM (speed)
 * - IBT_2 (MT_ROBUST): Controls via LPWM, RPWM (direction and speed)
 * 
 * @param dir - Motor direction (MOTOR_STOP, MOTOR_UP, MOTOR_DOWN)
 * @param speed - PWM duty cycle (0-255, where 0=stop, 255=full speed)
 * 
 * @note This function provides a unified interface. The actual driver
 *       implementation is selected at compile-time via MOTOR_TYPE.
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
