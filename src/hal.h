#ifndef HAL_H
#define HAL_H

#include "desk_types.h"
#include "pin_config.h"

#ifdef __cplusplus
extern "C" {
#endif

void HAL_init(void);
bool HAL_readButton(ButtonID_t button);
bool HAL_readLimitSensor(LimitID_t sensor);
void HAL_setMotor(MotorDirection_t dir, uint8_t speed);
void HAL_setLED(LEDStatus_t status);
uint32_t HAL_getTime(void);

#ifdef __cplusplus
}
#endif

#endif 
