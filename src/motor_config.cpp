/**
 * @file motor_config.cpp
 * @brief Motor Configuration Implementation - Encapsulated Motor Type
 * 
 * Implements the motor type configuration and accessor function. This file
 * encapsulates the MOTOR_TYPE macro, enabling:
 * - Controlled access via MotorConfig_getMotorType() getter function
 * - Future NVM (non-volatile memory) integration for runtime configuration
 * - Test environment overrides via TEST_MOTOR_TYPE preprocessor define
 * 
 * Motor Type Selection:
 * - MT_BASIC (0):   L298N dual H-bridge driver (lower cost, fully featured)
 * - MT_ROBUST (1):  IBT_2 intelligent driver (advanced diagnostics, current sensing)
 *
 * Future Enhancement:
 * MOTOR_TYPE will be read from NVM instead of being compile-time defined,
 * allowing runtime motor type configuration without recompilation.
 *
 * @version 1.0
 * @date 2026-02-22
 * @trace SWReq-XXX Motor type configuration
 */

#include "motor_config.h"

// ============================================================================
// MOTOR TYPE CONFIGURATION (encapsulated)
// ============================================================================
/**
 * @brief Active motor driver type (compile-time configuration)
 * 
 * Current behavior: Compile-time macro definition (default: MT_BASIC)
 * Future behavior: Will read from NVM for runtime configuration
 * 
 * Affects:
 * - Pin assignments (see pin_config.h)
 * - Control signal scheme (HAL pin/PWM combinations)
 * - Diagnostic capabilities (current sensing availability)
 * 
 * Migration plan:
 * 1. Current: Define as compile-time macro
 * 2. Future: Create NVM_getMotorType() function
 * 3. Eventually: Replace #define with read from NVM
 */
#define MOTOR_TYPE MT_BASIC

// ============================================================================
// COMPILE-TIME VALIDATION
// ============================================================================
#if !defined(MOTOR_TYPE)
    #error "MOTOR_TYPE must be defined as MT_BASIC or MT_ROBUST"
#endif

#if (MOTOR_TYPE != MT_BASIC) && (MOTOR_TYPE != MT_ROBUST)
    #error "MOTOR_TYPE must be MT_BASIC (0u) or MT_ROBUST (1u)"
#endif

// ============================================================================
// ACCESSOR FUNCTION IMPLEMENTATION
// ============================================================================
/**
 * @brief Get the configured motor driver type
 * 
 * Returns the motor type set at compile-time via MOTOR_TYPE macro.
 * In test environments, this function can be overridden by defining 
 * TEST_MOTOR_TYPE at compile time.
 * 
 * Current Implementation (Compile-time):
 *   - Returns the MOTOR_TYPE value from this file
 *   - Single configuration per build
 * 
 * Test Override Behavior (when TEST_MOTOR_TYPE is defined):
 *   - Returns the override value
 *   - Allows testing both motors without recompilation
 *   - Useful for CI/CD testing of both MT_BASIC and MT_ROBUST configurations
 * 
 * Future Implementation (Runtime NVM):
 *   - Will read motor type from non-volatile memory
 *   - Enable runtime motor configuration without rebuild
 *   - Support hardware-based motor type detection on startup
 * 
 * @return MotorType_t - MT_BASIC (L298N) or MT_ROBUST (IBT_2)
 */
MotorType_t MotorConfig_getMotorType(void)
{
#ifdef TEST_MOTOR_TYPE
    // Test environment override - allows testing both motor configurations
    // without recompilation. TEST_MOTOR_TYPE is set during compilation.
    return TEST_MOTOR_TYPE;
#else
    // Production: Use compile-time MOTOR_TYPE macro encapsulated in this file
    // Future: Replace with NVM_getMotorType() call for runtime configuration
    return MOTOR_TYPE;
#endif
}



