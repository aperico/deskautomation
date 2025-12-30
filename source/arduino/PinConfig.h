// PinConfig.h
// -----------------------------------------------------------------------------
// Pin configuration for Desk Automation Project
// Maps Arduino pins to hardware functions (LEDs, buttons, motor driver, etc.)
// -----------------------------------------------------------------------------

#ifndef PINCONFIG_H
#define PINCONFIG_H

// Error indicator LED (usually onboard LED)
static const int ERROR_LED = 13; // Onboard LED for error indication

// Desk position indicator LEDs
static const int LED_LEFT_PIN = 5;  // Left position indicator
static const int LED_RIGHT_PIN = 4; // Right position indicator

// User input buttons
static const int BUTTON_UP_PIN = 3;   // Button to move desk up
static const int BUTTON_DOWN_PIN = 2; // Button to move desk down

// Motor driver pins (L298N)
static const int IN1 = 8; // Motor direction control 1
static const int IN2 = 9; // Motor direction control 2
static const int ENA = 10; // Motor speed (PWM)

#endif
