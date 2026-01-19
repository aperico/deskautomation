/**
 * @file ComponentTests.cpp
 * @brief Component Test Suite for Desk Controller
 * 
 * Test Specification: 10_ComponentTestCasesSpecification.md
 * Requirements: 07_SoftwareRequirements.md
 * Design: 09_SoftwareDetailedDesign.md
 * 
 * Framework: Google Test
 * Module Under Test: MODULE-003 (DeskController)
 * Functions Under Test: FUNC-016 (DeskApp_task_init), FUNC-017 (DeskApp_task)
 */

// ============================================================================
// COMPONENT TEST GUIDE FOR NEW DEVELOPERS
// ============================================================================
/**
 * WHAT ARE COMPONENT TESTS?
 * 
 * Component tests verify individual software components (modules/functions) in
 * ISOLATION from hardware and other components. They test the logic and behavior
 * of a single component.
 * 
 * KEY CHARACTERISTICS:
 * - Test Level: COMPONENT (module/function level)
 * - Scope: Single component (DeskController)
 * - Dependencies: Mocked or stubbed
 * - Focus: Function behavior, logic correctness, requirement validation
 * - Naming: Component_TC###_DescriptiveName
 * 
 * WHEN TO WRITE COMPONENT TESTS:
 * - Testing individual functions (DeskApp_task, DeskApp_task_init)
 * - Verifying logic without hardware
 * - Testing edge cases and error conditions
 * - Validating requirements (SWE-REQ-XXX)
 * - Testing state machine logic
 * 
 * EXAMPLE COMPONENT TEST:
 * 
 * TEST(DeskController, Component_TC003_SwitchUp_CommandsUpward) {
 *     // Test documentation
 *     // @test Component_TC003_SwitchUp_CommandsUpward
 *     // @req SWE-REQ-003, SWE-REQ-005, SWE-REQ-020
 *     // @function FUNC-017 (DeskApp_task)
 *     
 *     // ARRANGE: Setup test data and initial state
 *     DeskAppInputs_t inputs = {};
 *     DeskAppOutputs_t outputs = {};
 *     DeskApp_task_init(&inputs, &outputs);
 *     inputs.switch_state = SWITCH_STATE_UP;
 *     
 *     // ACT: Execute the component function under test
 *     DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);
 *     
 *     // ASSERT: Verify expected behavior
 *     EXPECT_EQ(ret, APP_TASK_SUCCESS) << "Task should return success";
 *     EXPECT_TRUE(outputs.motor_enable) << "Motor should be enabled";
 *     EXPECT_FALSE(outputs.motor_direction) << "Direction should be UP (false)";
 *     EXPECT_EQ(outputs.motor_pwm, 255) << "PWM should be full speed";
 * }
 * 
 * TEST STRUCTURE (Arrange-Act-Assert):
 * 1. ARRANGE: Set up test inputs, initialize state
 * 2. ACT: Call the function being tested
 * 3. ASSERT: Verify outputs match expectations
 * 
 * KEY POINTS:
 * 1. Naming: Start with "Component_TC###" + descriptive name
 * 2. Documentation: Add @test, @req, @function tags for traceability
 * 3. Messages: Include descriptive messages in EXPECT/ASSERT macros
 * 4. Isolation: Test only the component, don't depend on hardware
 * 5. Coverage: Test normal cases, edge cases, and error conditions
 * 6. Clarity: Use clear variable names and comments
 * 
 * DIFFERENCE FROM OTHER TEST LEVELS:
 * - Component: Tests single component in isolation
 * - Integration: Tests multiple components working together
 * - Unit: Minimal basic functionality tests
 */

#include "../source/arduino/DeskController.h"
#include <gtest/gtest.h>

// ============================================================================
// TC-001: System Initialization (SWE-REQ-001, SWE-REQ-002)
// ============================================================================

/**
 * @test TC-001-1: Init_SetsSafeDefaults
 * @req SWE-REQ-001, SWE-REQ-002
 * @function FUNC-016
 */
TEST(DeskController, Component_TC001_Init_SetsSafeDefaults) {
    DeskAppInputs_t inputs = {};
    DeskAppOutputs_t outputs = {};
    
    // Initialize outputs to non-safe values
    outputs.motor_enable = true;
    outputs.motor_direction = true;
    outputs.motor_pwm = 200;
    
    // Initialize system
    DeskApp_task_init(&inputs, &outputs);
    
    // Verify safe defaults
    EXPECT_FALSE(outputs.motor_enable) << "Motor should be disabled after init";
    EXPECT_EQ(outputs.motor_pwm, 0) << "PWM should be zero after init";
}

/**
 * @test TC-001-2: SwitchOff_NoMovement
 * @req SWE-REQ-002, SWE-REQ-007, SWE-REQ-008
 * @function FUNC-017
 */
TEST(DeskController, Component_TC001_SwitchOff_NoMovement) {
    DeskAppInputs_t inputs = {};
    DeskAppOutputs_t outputs = {};
    
    inputs.switch_state = SWITCH_STATE_OFF;
    DeskApp_task_init(&inputs, &outputs);
    
    DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);
    
    EXPECT_FALSE(outputs.motor_enable) << "Motor should remain disabled when switch is OFF";
    EXPECT_EQ(outputs.motor_pwm, 0) << "PWM should be zero when switch is OFF";
    EXPECT_EQ(ret, APP_TASK_SUCCESS) << "Task should return SUCCESS";
}

// ============================================================================
// TC-003: Upward Movement (SWE-REQ-003, SWE-REQ-005)
// ============================================================================

/**
 * @test TC-003-1: SwitchUp_CommandsUpward
 * @req SWE-REQ-003, SWE-REQ-005
 * @function FUNC-017
 */
TEST(DeskController, Component_TC003_SwitchUp_CommandsUpward) {
    DeskAppInputs_t inputs = {};
    DeskAppOutputs_t outputs = {};
    
    inputs.switch_state = SWITCH_STATE_UP;
    DeskApp_task_init(&inputs, &outputs);
    
    DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);
    
    EXPECT_TRUE(outputs.motor_enable) << "Motor should be enabled for UP movement";
    EXPECT_FALSE(outputs.motor_direction) << "Direction should be false (UP)";
    EXPECT_EQ(outputs.motor_pwm, 255) << "PWM should be full speed (255)";
    EXPECT_EQ(ret, APP_TASK_SUCCESS) << "Task should return SUCCESS";
}

/**
 * @test TC-003-2: SwitchUp_ConsistentOutput
 * @req SWE-REQ-003, SWE-REQ-005, SWE-REQ-020
 * @function FUNC-017
 */
TEST(DeskController, Component_TC003_SwitchUp_ConsistentOutput) {
    DeskAppInputs_t inputs = {};
    DeskAppOutputs_t outputs = {};
    
    inputs.switch_state = SWITCH_STATE_UP;
    DeskApp_task_init(&inputs, &outputs);
    
    // Call multiple times and verify consistency
    for (int i = 0; i < 10; i++) {
        DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);
        
        EXPECT_TRUE(outputs.motor_enable) << "Output should be consistent on iteration " << i;
        EXPECT_FALSE(outputs.motor_direction) << "Direction should remain UP on iteration " << i;
        EXPECT_EQ(outputs.motor_pwm, 255) << "PWM should remain 255 on iteration " << i;
        EXPECT_EQ(ret, APP_TASK_SUCCESS) << "Return should be SUCCESS on iteration " << i;
    }
}

// ============================================================================
// TC-005: Downward Movement (SWE-REQ-004, SWE-REQ-006)
// ============================================================================

/**
 * @test TC-005-1: SwitchDown_CommandsDownward
 * @req SWE-REQ-004, SWE-REQ-006
 * @function FUNC-017
 */
TEST(DeskController, Component_TC005_SwitchDown_CommandsDownward) {
    DeskAppInputs_t inputs = {};
    DeskAppOutputs_t outputs = {};
    
    inputs.switch_state = SWITCH_STATE_DOWN;
    DeskApp_task_init(&inputs, &outputs);
    
    DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);
    
    EXPECT_TRUE(outputs.motor_enable) << "Motor should be enabled for DOWN movement";
    EXPECT_TRUE(outputs.motor_direction) << "Direction should be true (DOWN)";
    EXPECT_EQ(outputs.motor_pwm, 255) << "PWM should be full speed (255)";
    EXPECT_EQ(ret, APP_TASK_SUCCESS) << "Task should return SUCCESS";
}

/**
 * @test TC-005-2: SwitchDown_ConsistentOutput
 * @req SWE-REQ-004, SWE-REQ-006, SWE-REQ-020
 * @function FUNC-017
 */
TEST(DeskController, Component_TC005_SwitchDown_ConsistentOutput) {
    DeskAppInputs_t inputs = {};
    DeskAppOutputs_t outputs = {};
    
    inputs.switch_state = SWITCH_STATE_DOWN;
    DeskApp_task_init(&inputs, &outputs);
    
    // Call multiple times and verify consistency
    for (int i = 0; i < 10; i++) {
        DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);
        
        EXPECT_TRUE(outputs.motor_enable) << "Output should be consistent on iteration " << i;
        EXPECT_TRUE(outputs.motor_direction) << "Direction should remain DOWN on iteration " << i;
        EXPECT_EQ(outputs.motor_pwm, 255) << "PWM should remain 255 on iteration " << i;
        EXPECT_EQ(ret, APP_TASK_SUCCESS) << "Return should be SUCCESS on iteration " << i;
    }
}

// ============================================================================
// TC-007: Switch State Transitions (SWE-REQ-020)
// ============================================================================

/**
 * @test TC-007-1: SwitchTransitions_AllCombinations
 * @req SWE-REQ-020
 * @function FUNC-017
 */
TEST(DeskController, Component_TC007_SwitchTransitions_AllCombinations) {
    DeskAppInputs_t inputs = {};
    DeskAppOutputs_t outputs = {};
    
    DeskApp_task_init(&inputs, &outputs);
    
    // Test OFF → UP → OFF
    inputs.switch_state = SWITCH_STATE_OFF;
    DeskApp_task(&inputs, &outputs);
    EXPECT_FALSE(outputs.motor_enable) << "Motor should be off";
    
    inputs.switch_state = SWITCH_STATE_UP;
    DeskApp_task(&inputs, &outputs);
    EXPECT_TRUE(outputs.motor_enable) << "Motor should be enabled";
    EXPECT_FALSE(outputs.motor_direction) << "Direction should be UP";
    
    inputs.switch_state = SWITCH_STATE_OFF;
    DeskApp_task(&inputs, &outputs);
    EXPECT_FALSE(outputs.motor_enable) << "Motor should stop";
    
    // Test OFF → DOWN → OFF
    inputs.switch_state = SWITCH_STATE_DOWN;
    DeskApp_task(&inputs, &outputs);
    EXPECT_TRUE(outputs.motor_enable) << "Motor should be enabled";
    EXPECT_TRUE(outputs.motor_direction) << "Direction should be DOWN";
    
    inputs.switch_state = SWITCH_STATE_OFF;
    DeskApp_task(&inputs, &outputs);
    EXPECT_FALSE(outputs.motor_enable) << "Motor should stop";
    
    // Test UP → OFF → DOWN
    inputs.switch_state = SWITCH_STATE_UP;
    DeskApp_task(&inputs, &outputs);
    EXPECT_TRUE(outputs.motor_enable) << "Motor UP";
    EXPECT_FALSE(outputs.motor_direction) << "Direction UP";
    
    inputs.switch_state = SWITCH_STATE_OFF;
    DeskApp_task(&inputs, &outputs);
    EXPECT_FALSE(outputs.motor_enable) << "Motor stop";
    
    inputs.switch_state = SWITCH_STATE_DOWN;
    DeskApp_task(&inputs, &outputs);
    EXPECT_TRUE(outputs.motor_enable) << "Motor DOWN";
    EXPECT_TRUE(outputs.motor_direction) << "Direction DOWN";
    
    // Test DOWN → OFF → UP (reverse direction)
    inputs.switch_state = SWITCH_STATE_DOWN;
    DeskApp_task(&inputs, &outputs);
    EXPECT_TRUE(outputs.motor_direction) << "Direction DOWN";
    
    inputs.switch_state = SWITCH_STATE_OFF;
    DeskApp_task(&inputs, &outputs);
    EXPECT_FALSE(outputs.motor_enable) << "Motor stop";
    
    inputs.switch_state = SWITCH_STATE_UP;
    DeskApp_task(&inputs, &outputs);
    EXPECT_FALSE(outputs.motor_direction) << "Direction changed to UP";
}

// ============================================================================
// TC-008: Null Pointer Handling (SWE-REQ-020)
// ============================================================================

/**
 * @test TC-008-1: NullInputs_ReturnsError
 * @req SWE-REQ-020 (defensive programming)
 * @function FUNC-017
 */
TEST(DeskController, Component_TC008_NullInputs_ReturnsError) {
    DeskAppOutputs_t outputs = {};
    
    DeskAppTask_Return_t ret = DeskApp_task(NULL, &outputs);
    
    EXPECT_EQ(ret, APP_TASK_ERROR) << "Should return ERROR for NULL inputs";
}

/**
 * @test TC-008-2: NullOutputs_ReturnsError
 * @req SWE-REQ-020
 * @function FUNC-017
 */
TEST(DeskController, Component_TC008_NullOutputs_ReturnsError) {
    DeskAppInputs_t inputs = {};
    inputs.switch_state = SWITCH_STATE_OFF;
    
    DeskAppTask_Return_t ret = DeskApp_task(&inputs, NULL);
    
    EXPECT_EQ(ret, APP_TASK_ERROR) << "Should return ERROR for NULL outputs";
}

// ============================================================================
// TC-013: Power-Off Recovery (SWE-REQ-009)
// ============================================================================

/**
 * @test TC-013: PowerCycle_ReinitializesClean
 * @req SWE-REQ-009, SWE-REQ-001
 * @function FUNC-016
 */
TEST(DeskController, Component_TC013_PowerCycle_ReinitializesClean) {
    DeskAppInputs_t inputs = {};
    DeskAppOutputs_t outputs = {};
    
    // Simulate system was commanding movement
    inputs.switch_state = SWITCH_STATE_UP;
    DeskApp_task_init(&inputs, &outputs);
    DeskApp_task(&inputs, &outputs);
    
    ASSERT_TRUE(outputs.motor_enable) << "Precondition: motor should be running";
    
    // Simulate power cycle by calling init again
    DeskApp_task_init(&inputs, &outputs);
    
    // Verify clean reinitialization
    EXPECT_FALSE(outputs.motor_enable) << "Motor should be disabled after power cycle";
    EXPECT_EQ(outputs.motor_pwm, 0) << "PWM should be zero after power cycle";
    
    // Verify system is operational after reset
    inputs.switch_state = SWITCH_STATE_DOWN;
    DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);
    EXPECT_EQ(ret, APP_TASK_SUCCESS) << "System should be functional after reset";
    EXPECT_TRUE(outputs.motor_enable) << "System should accept commands after reset";
}

// ============================================================================
// Additional Edge Case Tests
// ============================================================================

/**
 * @test EdgeCase: InitWithNullInputs_HandlesGracefully
 * @req SWE-REQ-001 (robustness)
 * @function FUNC-016
 */
TEST(DeskController, Component_EdgeCase_InitWithNullInputs_HandlesGracefully) {
    DeskAppOutputs_t outputs = {};
    outputs.motor_enable = true;  // Set to non-safe value
    
    // Init should handle NULL inputs gracefully
    DeskApp_task_init(NULL, &outputs);
    
    EXPECT_FALSE(outputs.motor_enable) << "Should still set safe defaults with NULL inputs";
    EXPECT_EQ(outputs.motor_pwm, 0) << "PWM should be safe even with NULL inputs";
}

/**
 * @test EdgeCase: InitWithNullOutputs_NoSegfault
 * @req SWE-REQ-001 (robustness)
 * @function FUNC-016
 */
TEST(DeskController, Component_EdgeCase_InitWithNullOutputs_NoSegfault) {
    DeskAppInputs_t inputs = {};
    
    // Should not crash
    DeskApp_task_init(&inputs, NULL);
    
    // If we get here, no segfault occurred
    SUCCEED();
}

/**
 * @test EdgeCase: RapidSwitchChanges_StableOutput
 * @req SWE-REQ-020 (state integrity)
 * @function FUNC-017
 */
TEST(DeskController, Component_EdgeCase_RapidSwitchChanges_StableOutput) {
    DeskAppInputs_t inputs = {};
    DeskAppOutputs_t outputs = {};
    
    DeskApp_task_init(&inputs, &outputs);
    
    // Rapidly cycle through all states
    for (int cycle = 0; cycle < 100; cycle++) {
        inputs.switch_state = SWITCH_STATE_UP;
        DeskApp_task(&inputs, &outputs);
        EXPECT_TRUE(outputs.motor_enable);
        EXPECT_FALSE(outputs.motor_direction);
        
        inputs.switch_state = SWITCH_STATE_OFF;
        DeskApp_task(&inputs, &outputs);
        EXPECT_FALSE(outputs.motor_enable);
        
        inputs.switch_state = SWITCH_STATE_DOWN;
        DeskApp_task(&inputs, &outputs);
        EXPECT_TRUE(outputs.motor_enable);
        EXPECT_TRUE(outputs.motor_direction);
        
        inputs.switch_state = SWITCH_STATE_OFF;
        DeskApp_task(&inputs, &outputs);
        EXPECT_FALSE(outputs.motor_enable);
    }
}

// ============================================================================
// Test Main
// ============================================================================

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
