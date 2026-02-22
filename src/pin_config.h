/**
 * @file pin_config.h
 * @brief Arduino Pin Configuration for Automated Desk Lift System
 * 
 * @hardware_platform Arduino UNO (ATmega328P)
 * @motor_drivers Configurable: L298N (MT_BASIC) or IBT_2 (MT_ROBUST)
 * 
 * @pin_assignments
 * - Digital pins 2-5: Input signals (buttons, limit switches)
 * - Digital pins 6-7: Motor driver signals (varies by driver type)
 * - PWM pins 9-10: Motor driver PWM control
 * - Digital pins 11-13: Status LEDs (visual feedback)
 * 
 * @motor_type_pinouts
 * L298N (MT_BASIC):
 *   - PIN_MOTOR_EN1=6, EN2=7, PWM=9 (direction via EN1/EN2, speed via PWM)
 * IBT_2 (MT_ROBUST):
 *   - PIN_MOTOR_LPWM=9, RPWM=10, CIN=A1 (direction/speed via LPWM/RPWM ratio)
 * 
 * @requirements
 * - SysReq-001: Button inputs for UP/DOWN commands
 * - SysReq-007: Limit switches for mechanical protection
 * - User feedback: Status LEDs for button press and error indication
 * - Motor type selection via motor_config.h MOTOR_TYPE
 * 
 * @version 2.0
 * @date 2026-02-21
 * @changes Added configurable motor driver support (L298N & IBT_2)
 */

#ifndef PIN_CONFIG_H
#define PIN_CONFIG_H

#include <stdint.h>
#include "motor_config.h"

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
const uint8_t PIN_LIMIT_UPPER = 7;     // Digital input with internal pull-up (SysReq-007)
const uint8_t PIN_LIMIT_LOWER = 8;     // Digital input with internal pull-up (SysReq-007)

// ============================================================================
// MOTOR DRIVER PINS - CONFIGURABLE (All pins defined for both motor types)
// ============================================================================
// Note: HAL now determines which pins to use at runtime via motor_type input
//       Comment next to each pin indicates primary motor type usage

// L298N (MT_BASIC) pins:
const uint8_t PIN_MOTOR_EN1 = 5;        // Enable 1 (direction control UP) - MT_BASIC
const uint8_t PIN_MOTOR_EN2 = 6;        // Enable 2 (direction control DOWN) - MT_BASIC
const uint8_t PIN_MOTOR_PWM = 9;        // PWM signal for motor speed control - MT_BASIC

// IBT_2 (MT_ROBUST) pins:
const uint8_t PIN_MOTOR_LPWM = 9;       // Left PWM (counterclockwise, UP direction) - MT_ROBUST
const uint8_t PIN_MOTOR_RPWM = 10;      // Right PWM (clockwise, DOWN direction) - MT_ROBUST
const uint8_t PIN_MOTOR_CIN = 15;       // Current Input for diagnostics (optional) - Analog pin A1 - MT_ROBUST
const uint8_t PIN_MOTOR_SENSE = 14;     // Analog pin A0 (current sensing) - MT_ROBUST only





#endif // PIN_CONFIG_H


