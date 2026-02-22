/**
 * @file motor_config.h
 * @brief Motor Driver Configuration - Selectable Motor Driver Type
 * 
 * This configuration enables the system to support multiple motor driver implementations:
 * - MT_BASIC: L298N dual H-bridge motor driver (lower cost, fully featured)
 * - MT_ROBUST: IBT_2 intelligent motor driver (advanced diagnostics, integrated features)
 * 
 * Selection is made via MOTOR_TYPE macro, which determines:
 * - Pin assignments
 * - Control signal scheme
 * - Current sensing method
 * - Diagnostic capabilities
 * 
 * @version 1.0
 * @date 2026-02-21
 */

#ifndef MOTOR_CONFIG_H
#define MOTOR_CONFIG_H


// ============================================================================
// MOTOR DRIVER TYPE SELECTION
// ============================================================================
/**
 * @brief Motor driver enumeration
 * 
 * Choose one:
 * - MT_BASIC: L298N (standard implementation, 2 enable pins + 1 PWM)
 * - MT_ROBUST: IBT_2 (advanced implementation, 2 PWM pins, integrated diagnostics)
 */
typedef enum
{
    MT_BASIC = 0u,      ///< L298N Dual H-Bridge Motor Driver
    MT_ROBUST = 1u      ///< IBT_2 Intelligent Motor Driver
} MotorType_t;

/**
 * @brief Active motor driver type
 * 
 * Set this to select which motor driver hardware is in use.
 * Affects: pin assignments, control signals, diagnostic capabilities
 * 
 * Default: MT_BASIC (L298N)
 * Use MT_ROBUST for IBT_2 driver
 */
#define MOTOR_TYPE MT_BASIC

// ============================================================================
// MOTOR DRIVER FEATURE MATRIX
// ============================================================================

/**
 * Motor Driver Comparison:
 * 
 * ┌─────────────────────────────────────────────────────────────────┐
 * │ Feature                 │ L298N (MT_BASIC) │ IBT_2 (MT_ROBUST)   │
 * ├─────────────────────────────────────────────────────────────────┤
 * │ Cost                    │ $$ Low           │ $$$ Moderate       │
 * │ Max Current             │ 2A               │ 5-10A              │
 * │ Control Pins            │ 3 (EN1,EN2,PWM)  │ 2 (LPWM, RPWM)     │
 * │ Direction Control       │ Via EN1/EN2      │ Via PWM ratio      │
 * │ Speed Control           │ PWM 0-255        │ PWM 0-255          │
 * │ Current Sensing         │ Analog (external)│ Analog (external)  │
 * │ Diagnostic Output       │ None             │ CIN pin            │
 * │ Thermal Protection      │ None             │ Integrated         │
 * │ Dead-Time Protection    │ Manual           │ Integrated         │
 * │ Soft-Start Capability   │ Software         │ Hardware + Software│
 * │ Efficiency              │ ~80%             │ ~92%               │
 * └─────────────────────────────────────────────────────────────────┘
 */

// ============================================================================
// L298N (MOTOR_TYPE = MT_BASIC) CONFIGURATION
// ============================================================================

/**
 * L298N Control Scheme:
 * 
 * Direction + Speed:
 * - MOTOR_STOP:  EN1=0, EN2=0, PWM=X     (both stops, motor coasts)
 * - MOTOR_UP:    EN1=1, EN2=0, PWM=0-255 (IN1=high, speed via PWM)
 * - MOTOR_DOWN:  EN1=0, EN2=1, PWM=0-255 (IN2=high, speed via PWM)
 * 
 * Pins:
 * - PIN_MOTOR_EN1 (6):   Enable for UP direction (active HIGH)
 * - PIN_MOTOR_EN2 (7):   Enable for DOWN direction (active HIGH)
 * - PIN_MOTOR_PWM (9):   PWM signal (0-255) controls speed
 */

// ============================================================================
// IBT_2 (MOTOR_TYPE = MT_ROBUST) CONFIGURATION
// ============================================================================

/**
 * IBT_2 Control Scheme:
 * 
 * Direction + Speed via PWM ratio:
 * - MOTOR_STOP:  LPWM=0,   RPWM=0       (no power, motor coasts)
 * - MOTOR_UP:    LPWM=255, RPWM=0-127   (left PWM high, right ramped)
 * - MOTOR_DOWN:  LPWM=0-127, RPWM=255   (right PWM high, left ramped)
 * 
 * Pins:
 * - PIN_MOTOR_LPWM (9):  Left PWM (0-255) - rotates motor counterclockwise
 * - PIN_MOTOR_RPWM (10): Right PWM (0-255) - rotates motor clockwise
 * - PIN_MOTOR_CIN (A1):  Diagnostic current input (optional)
 */

// ============================================================================
// VALIDATION
// ============================================================================

#if !defined(MOTOR_TYPE)
    #error "MOTOR_TYPE must be defined as MT_BASIC or MT_ROBUST"
#endif

#if (MOTOR_TYPE != MT_BASIC) && (MOTOR_TYPE != MT_ROBUST)
    #error "MOTOR_TYPE must be MT_BASIC (0u) or MT_ROBUST (1u)"
#endif

#endif // MOTOR_CONFIG_H
