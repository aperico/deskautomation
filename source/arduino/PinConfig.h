/**
 * @file PinConfig.h
 * @brief Pin configuration for Desk Automation Project
 * 
 * @module MODULE-001
 * @implements ARCH-COMP-003
 * @requirements SWE-REQ-001
 * @architecture ARCH-IF-003
 * @detailed_design 09_SoftwareDetailedDesign.md
 * 
 * Maps Arduino pins to hardware functions:
 * - IBT-2/BTS7960 motor driver control (PWM and enable)
 * - ON/OFF/ON rocker switch inputs
 * - Current sensing analog inputs
 */

// PinConfig.h
// -----------------------------------------------------------------------------
// Pin configuration for Desk Automation Project
// Maps Arduino pins to hardware functions (motor driver, power, etc.)
// -----------------------------------------------------------------------------

#ifndef PINCONFIG_H
#define PINCONFIG_H


// IBT-2/BTS7960 Motor Driver Pin Mapping
// IMPORTANT: R_EN_PIN and L_EN_PIN must always be driven identically (defensive safety)
// @constants CONST-001 through CONST-004
// @implements SWE-REQ-005, SWE-REQ-006
static constexpr int RPWM_PIN = 5;   // Right PWM (motor direction/speed)
static constexpr int LPWM_PIN = 6;   // Left PWM (motor direction/speed)
static constexpr int R_EN_PIN = 9;   // Right Enable (moved from 7)
static constexpr int L_EN_PIN = 10;  // Left Enable (moved from 8)

// ON/OFF/ON switch (center-off)
// @constants CONST-005, CONST-006
// @implements SWE-REQ-003, SWE-REQ-004
static constexpr int SWITCH_UP_PIN = 7;    // Switch UP position
static constexpr int SWITCH_DOWN_PIN = 8;  // Switch DOWN position

// Current sensing analog inputs
// @constants CONST-007, CONST-008
// @implements SWE-REQ-021
#if defined(ARDUINO)
#  include <Arduino.h>
#  ifdef A0
static constexpr int R_IS_PIN = A0;  // Right current sense (analog)
static constexpr int L_IS_PIN = A1;  // Left current sense (analog)
#  else
static constexpr int R_IS_PIN = 14;
static constexpr int L_IS_PIN = 15;
#  endif
#else
static constexpr int R_IS_PIN = 14;  // Right current sense (analog)
static constexpr int L_IS_PIN = 15;   // Left current sense (analog)
#endif
// Power pins (B+ and B-) are not controlled by Arduino, but document for clarity
// B+ (12V/24V motor power), B- (GND)

#endif // PINCONFIG_H
