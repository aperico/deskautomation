#ifndef DESK_APP_H
#define DESK_APP_H

#include "desk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    bool button_up;
    bool button_down;
    bool limit_upper;
    bool limit_lower;
    bool fault_in;       // external fault input (e.g., motor controller)
    uint32_t timestamp_ms;
} AppInput_t;

typedef struct
{
    MotorDirection_t motor_cmd;
    uint8_t motor_speed;
    LEDStatus_t led_status;
    bool fault_out;     // latched fault state
} AppOutput_t;

typedef enum
{
    APP_STATE_IDLE = 0,
    APP_STATE_MOVING_UP = 1,
    APP_STATE_MOVING_DOWN = 2,
    APP_STATE_FAULT = 3
} AppState_t;

void APP_Init(void);
void APP_Task(const AppInput_t *inputs, AppOutput_t *outputs);
AppState_t APP_GetState(void);

#ifdef __cplusplus
}
#endif

#endif // DESK_APP_H
