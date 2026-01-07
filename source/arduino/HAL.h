
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

static const unsigned int BLINK_INTERVAL_MS = 500;
static const unsigned char MOTOR_SPEED = 255;

void HAL_Init(); // Initialize all hardware pins and peripherals
void HAL_ProcessAppState(const DeskAppTask_Return_t ret, const DeskAppOutputs_t *outputs);

void HAL_SetErrorLED(const bool state);
void HAL_SetWarningLED(const bool state);
void HAL_SetPowerLED(const bool state);
void HAL_SetMotorDirection(const bool state);
void HAL_SetMovingDownLED(const bool state);
void HAL_SetMovingUpLED(const bool state);
bool HAL_GetMovingDownLED();
bool HAL_GetMovingUpLED();
bool HAL_GetErrorLED();
void HAL_MoveUp(const unsigned char speed);
void HAL_MoveDown(const unsigned char speed);
void HAL_StopMotor();
void HAL_BlinkErrorLED();
void HAL_BlinkUPLED();
void HAL_BlinkDOWNLED();

// Debounce state for buttons
typedef struct {
  bool lastState;
  unsigned long lastDebounceTime;
} DebounceState;

bool HAL_debounceButton(const int pin, DebounceState &state, const unsigned long debounceDelay);

#endif