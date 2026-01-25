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
// INPUT PINS (Digital Inputs with Internal Pull-ups)
// ============================================================================

/**
 * @brief UP button input (rocker switch UP position)
 * Pin 2: Digital input with internal pull-up
 * Active LOW (pressed = LOW, released = HIGH)
 */
const uint8_t PIN_BUTTON_UP = 2;

/**
 * @brief DOWN button input (rocker switch DOWN position)
 * Pin 3: Digital input with internal pull-up
 * Active LOW (pressed = LOW, released = HIGH)
 */
const uint8_t PIN_BUTTON_DOWN = 3;

/**
 * @brief Upper mechanical limit switch input
 * Pin 4: Digital input with internal pull-up
 * Active LOW (triggered = LOW, open = HIGH)
 * Prevents upward motion when desk reaches maximum height (120 cm)
 */
const uint8_t PIN_LIMIT_UPPER = 7;

/**
 * @brief Lower mechanical limit switch input
 * Pin 5: Digital input with internal pull-up
 * Active LOW (triggered = LOW, open = HIGH)
 * Prevents downward motion when desk reaches minimum height (30 cm)
 */
const uint8_t PIN_LIMIT_LOWER = 8;

// ============================================================================
// MOTOR DRIVER PINS (L298N Dual H-Bridge)
// ============================================================================

/**
 * @brief L298N Enable 1 (EN1) - Motor direction control UP
 * Pin 6: Digital output (HIGH = enable UP direction)
 * Controls motor direction in combination with EN2
 * 
 * Motor Direction Truth Table:
 *   EN1=HIGH, EN2=LOW  → Motor UP (forward)
 *   EN1=LOW,  EN2=HIGH → Motor DOWN (reverse)
 *   EN1=LOW,  EN2=LOW  → Motor STOP (brake)
 */
const uint8_t PIN_MOTOR_EN1 = 6;

/**
 * @brief L298N Enable 2 (EN2) - Motor direction control DOWN
 * Pin 5: Digital output (HIGH = enable DOWN direction)
 * Controls motor direction in combination with EN1
 */
const uint8_t PIN_MOTOR_EN2 = 5;

/**
 * @brief L298N PWM (IN1/IN2) - Speed control
 * Pin 9: PWM output (0-255 duty cycle)
 * Modulates motor speed regardless of direction
 * Timer1 PWM output for smooth frequency control
 */
const uint8_t PIN_MOTOR_PWM = 9;




// ============================================================================
// STATUS LED PINS (Visual Feedback Indicators)
// ============================================================================

/**
 * @brief UP button pressed indicator LED
 * Pin 11: Digital output (HIGH = LED on, LOW = LED off)
 * Illuminates when UP button is actively pressed
 * Provides visual confirmation of button input
 */
const uint8_t PIN_LED_BT_UP = 11;

/**
 * @brief DOWN button pressed indicator LED
 * Pin 12: Digital output (HIGH = LED on, LOW = LED off)
 * Illuminates when DOWN button is actively pressed
 * Provides visual confirmation of button input
 */
const uint8_t PIN_LED_BT_DOWN = 10;

/**
 * @brief Error state indicator LED
 * Pin 13: Digital output (HIGH = LED on, LOW = LED off)
 * Illuminates when system enters ERROR state (fault detected)
 * Indicates: motor stall, limit switch failure, or other faults
 * Built-in LED on Arduino UNO for easy debugging
 */
const uint8_t PIN_LED_ERROR = 4;

#endif // PIN_CONFIG_H


