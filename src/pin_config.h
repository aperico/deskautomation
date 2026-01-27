/**
 * @file pin_config.h
 * @brief Arduino Pin Configuration for Automated Desk Lift System
 * 
 * @hardware_platform Arduino UNO (ATmega328P)
 * @motor_driver L298N Dual H-Bridge Motor Driver
 * 
 * @pin_assignments
 * - Digital pins 2-5: Input signals (buttons, limit switches)
 * - Digital pins 6-7: Motor driver enable signals
 * - PWM pins 9-10: Motor driver PWM control
 * - Digital pins 11-13: Status LEDs (visual feedback)
 * 
 * @requirements
 * - SysReq-001: Button inputs for UP/DOWN commands
 * - SysReq-007: Limit switches for mechanical protection
 * - User feedback: Status LEDs for button press and error indication
 * 
 * @version 1.1
 * @date 2026-01-25
 * @changes Added 3 status LEDs (UP, DOWN, ERROR); updated from BTS7960 to L298N
 */

#ifndef PIN_CONFIG_H
#define PIN_CONFIG_H

#include <stdint.h>


// ============================================================================
// STATUS LED PINS (Visual Feedback Indicators)
// ============================================================================
const uint8_t PIN_LED_BT_UP = 11;
const uint8_t PIN_LED_BT_DOWN = 10;
const uint8_t PIN_LED_ERROR = 4;
// ============================================================================
// INPUT PINS (Digital Inputs with Internal Pull-ups)
// ============================================================================
const uint8_t PIN_BUTTON_UP = 2;
const uint8_t PIN_BUTTON_DOWN = 3;
const uint8_t PIN_LIMIT_UPPER = 7;
const uint8_t PIN_LIMIT_LOWER = 8;
// ============================================================================
// MOTOR DRIVER PINS (L298N Dual H-Bridge)
// ============================================================================
 //@brief L298N Enable 1 (EN1) - Motor direction control UP
const uint8_t PIN_MOTOR_EN1 = 6;
const uint8_t PIN_MOTOR_EN2 = 5;
const uint8_t PIN_MOTOR_PWM = 9;





#endif // PIN_CONFIG_H


