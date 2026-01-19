// PinConfig.h
// -----------------------------------------------------------------------------
// Pin configuration for Desk Automation Project
// Maps Arduino pins to hardware functions (LEDs, buttons, motor driver, etc.)
// -----------------------------------------------------------------------------

#ifndef PINCONFIG_H
#define PINCONFIG_H

// CONST-001: Error indicator LED 
static constexpr int ERROR_LED = 4; // Onboard LED for error indication

// CONST-002, CONST-003: Desk position indicator LEDs
static constexpr int LED_LEFT_PIN = 11;  // Up movement LED (blue) 11
static constexpr int LED_RIGHT_PIN = 10; // Down movement LED (yellow)

// CONST-004, CONST-005: User input buttons
static constexpr int BUTTON_UP_PIN = 2;   // Button to move desk up
static constexpr int BUTTON_DOWN_PIN = 3; // Button to move desk down

static constexpr int BUTTON_UPPER_LIMIT_PIN = 7; // Upper limit switch input
static constexpr int BUTTON_LOWER_LIMIT_PIN = 8; // Lower limit switch input


// CONST-006, CONST-007, CONST-008: Motor driver pins (L298N)
static constexpr int IN1 = 5;  // Motor direction control 1
static constexpr int IN2 = 6;  // Motor direction control 2
static constexpr int ENA = 9;  // Motor speed (PWM)

#endif // PINCONFIG_H
