#ifndef DESK_TYPES_H
#define DESK_TYPES_H

#include <stdint.h>

typedef enum
{
    BUTTON_UP = 0,
    BUTTON_DOWN = 1,
    BUTTON_COUNT = 2
} ButtonID_t;

typedef enum
{
    LIMIT_UPPER = 0,
    LIMIT_LOWER = 1,
    LIMIT_COUNT = 2
} LimitID_t;

typedef enum
{
    MOTOR_STOP = 0,
    MOTOR_UP = 1,
    MOTOR_DOWN = 2
} MotorDirection_t;

/**
 * @brief LED identifier for independent LED control
 * 
 * System has 3 status LEDs for visual feedback:
 * - LED_BT_UP: Indicates UP button is pressed
 * - LED_BT_DOWN: Indicates DOWN button is pressed
 * - LED_ERROR: Indicates system error/fault state
 */
typedef enum
{
    LED_BT_UP = 0,      ///< UP button pressed indicator (Pin 11)
    LED_BT_DOWN = 1,    ///< DOWN button pressed indicator (Pin 12)
    LED_ERROR = 2,      ///< Error state indicator (Pin 13, built-in LED)
    LED_COUNT = 3       ///< Total number of LEDs
} LEDID_t;

/**
 * @brief LED state (ON or OFF)
 * 
 * Simple binary control for each LED.
 */
typedef enum
{
    LED_OFF = 0,        ///< LED turned off
    LED_ON = 1          ///< LED turned on
} LEDState_t;

#endif // DESK_TYPES_H
