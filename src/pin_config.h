#ifndef PIN_CONFIG_H
#define PIN_CONFIG_H

#include <stdint.h>

// Input pins
const uint8_t PIN_BUTTON_UP = 2;
const uint8_t PIN_BUTTON_DOWN = 3;
const uint8_t PIN_LIMIT_UPPER = 4;
const uint8_t PIN_LIMIT_LOWER = 5;

// Motor driver pins (BTS7960)
const uint8_t PIN_MOTOR_REN = 6;
const uint8_t PIN_MOTOR_LEN = 7;
const uint8_t PIN_MOTOR_RPWM = 9;
const uint8_t PIN_MOTOR_LPWM = 10;

// Status LED
const uint8_t PIN_LED_STATUS = 13;

#endif // PIN_CONFIG_H
