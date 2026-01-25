#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#include "desk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    MotorDirection_t dir;
    uint8_t pwm;
    bool fault; // true if stall or error detected
} MotorControllerOutput_t;

void MotorController_init(void);
MotorControllerOutput_t MotorController_update(MotorDirection_t cmd_dir, uint8_t target_pwm, uint32_t now_ms);

#ifdef __cplusplus
}
#endif

#endif // MOTOR_CONTROLLER_H
