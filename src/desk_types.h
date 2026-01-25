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

typedef enum
{
    LED_OFF = 0,
    LED_IDLE = 1,
    LED_ACTIVE = 2,
    LED_ERROR = 3
} LEDStatus_t;

#endif // DESK_TYPES_H
