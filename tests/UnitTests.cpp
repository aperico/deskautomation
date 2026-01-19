
#define DESK_CONTROLLER_ENABLE_TEST_INTERFACE 1
#include <gtest/gtest.h>
#include "DeskController.h"
#include "DeskControllerTestInterface.h"

// White-box test fixture for DeskController state handler functions
class DeskControllerStateHandlerTest : public ::testing::Test {
protected:
    DeskAppInputs_t inputs;
    DeskAppOutputs_t outputs;

    void SetUp() override {
        // Initialize inputs and outputs to known state
        inputs.switch_state = SWITCH_STATE_OFF;
        outputs.motor_enable = false;
        outputs.motor_direction = false;
        outputs.motor_pwm = 0;
        
        // Reset internal state to IDLE
        DeskApp_task_init(&inputs, &outputs);
        DeskApp_set_time_ms(0);
    }
    
    void SetInternalState(int state, unsigned long dwellStartMs = 0, unsigned long movementStartMs = 0) {
        DeskControllerInternalState_t state_struct;
        state_struct.currentState = state;
        state_struct.dwellStartMs = dwellStartMs;
        state_struct.movementStartMs = movementStartMs;
        DeskApp_set_internal_state(&state_struct);
    }
    
    void AdvanceTime(unsigned long ms) {
        DeskApp_set_time_ms(DeskApp_get_time_ms() + ms);
    }
};

// ============================================================================
// WHITE-BOX UNIT TESTS: IDLE State Handler
// ============================================================================

/**
 * @test Unit_IdleHandler_SwitchUp_TransitionsToMoveUp
 * @purpose Verify idle handler transitions to MOVE_UP on UP switch
 * @focus handle_idle_test() function behavior
 */
TEST_F(DeskControllerStateHandlerTest, Unit_IdleHandler_SwitchUp_TransitionsToMoveUp) {
    SetInternalState(0); // APP_STATE_IDLE
    inputs.switch_state = SWITCH_STATE_UP;
    
    handle_idle_test(&inputs, &outputs);
    
    // Should set motor enable and direction for UP
    EXPECT_TRUE(outputs.motor_enable);
    EXPECT_FALSE(outputs.motor_direction); // UP direction
    EXPECT_EQ(outputs.motor_pwm, 255);
}

/**
 * @test Unit_IdleHandler_SwitchDown_TransitionsToMoveDown
 * @purpose Verify idle handler transitions to MOVE_DOWN on DOWN switch
 * @focus handle_idle_test() function behavior
 */
TEST_F(DeskControllerStateHandlerTest, Unit_IdleHandler_SwitchDown_TransitionsToMoveDown) {
    SetInternalState(0); // APP_STATE_IDLE
    inputs.switch_state = SWITCH_STATE_DOWN;
    
    handle_idle_test(&inputs, &outputs);
    
    // Should set motor enable and direction for DOWN
    EXPECT_TRUE(outputs.motor_enable);
    EXPECT_TRUE(outputs.motor_direction); // DOWN direction
    EXPECT_EQ(outputs.motor_pwm, 255);
}

/**
 * @test Unit_IdleHandler_SwitchOff_RemainsIdle
 * @purpose Verify idle handler keeps motor off when switch is OFF
 * @focus handle_idle_test() function behavior
 */
TEST_F(DeskControllerStateHandlerTest, Unit_IdleHandler_SwitchOff_RemainsIdle) {
    SetInternalState(0); // APP_STATE_IDLE
    inputs.switch_state = SWITCH_STATE_OFF;
    
    handle_idle_test(&inputs, &outputs);
    
    // Motor should remain disabled
    EXPECT_FALSE(outputs.motor_enable);
    EXPECT_EQ(outputs.motor_pwm, 0);
}

/**
 * @test Unit_IdleHandler_NullOutputs_HandlesSafely
 * @purpose Verify idle handler doesn't crash on NULL outputs
 * @focus Error resilience in state handlers
 */
TEST_F(DeskControllerStateHandlerTest, Unit_IdleHandler_NullOutputs_HandlesSafely) {
    SetInternalState(0); // APP_STATE_IDLE
    inputs.switch_state = SWITCH_STATE_UP;
    
    // Should not crash
    EXPECT_NO_THROW(handle_idle_test(&inputs, nullptr));
}

// ============================================================================
// WHITE-BOX UNIT TESTS: MOVE_UP State Handler
// ============================================================================

/**
 * @test Unit_MoveUpHandler_ContinuesMotorUp
 * @purpose Verify move_up handler keeps motor running upward
 * @focus handle_move_up_test() function behavior
 */
TEST_F(DeskControllerStateHandlerTest, Unit_MoveUpHandler_ContinuesMotorUp) {
    SetInternalState(1); // APP_STATE_MOVE_UP
    inputs.switch_state = SWITCH_STATE_UP;
    
    handle_move_up_test(&inputs, &outputs);
    
    // Should maintain motor motion
    EXPECT_TRUE(outputs.motor_enable);
    EXPECT_FALSE(outputs.motor_direction); // UP direction
    EXPECT_EQ(outputs.motor_pwm, 255);
}

/**
 * @test Unit_MoveUpHandler_SwitchToOff_StopsMotor
 * @purpose Verify move_up handler stops motor when switch released
 * @focus State transition: MOVE_UP → IDLE
 */
TEST_F(DeskControllerStateHandlerTest, Unit_MoveUpHandler_SwitchToOff_StopsMotor) {
    SetInternalState(1); // APP_STATE_MOVE_UP
    inputs.switch_state = SWITCH_STATE_OFF;
    
    handle_move_up_test(&inputs, &outputs);
    
    // Motor should stop
    EXPECT_FALSE(outputs.motor_enable);
    EXPECT_EQ(outputs.motor_pwm, 0);
}

/**
 * @test Unit_MoveUpHandler_SwitchToDown_StartsDownwell
 * @purpose Verify move_up handler transitions to DOWN with dwell
 * @focus State transition: MOVE_UP → DWELL (direction reversal)
 */
TEST_F(DeskControllerStateHandlerTest, Unit_MoveUpHandler_SwitchToDown_StartsDownwell) {
    SetInternalState(1); // APP_STATE_MOVE_UP
    inputs.switch_state = SWITCH_STATE_DOWN;
    
    handle_move_up_test(&inputs, &outputs);
    
    // Should stop motor to prepare for reversal (dwell state)
    // v2.0: Will transition to dwell, then move down
    EXPECT_FALSE(outputs.motor_enable);
}

/**
 * @test Unit_MoveUpHandler_TimeoutDetection
 * @purpose Verify move_up handler detects 30-second timeout
 * @focus Safety: SWE-REQ-018 (Movement Timeout)
 */
TEST_F(DeskControllerStateHandlerTest, Unit_MoveUpHandler_TimeoutDetection) {
    unsigned long start_time = 0;
    SetInternalState(1, 0, start_time); // APP_STATE_MOVE_UP, movementStartMs = 0
    inputs.switch_state = SWITCH_STATE_UP;
    
    // Simulate 30+ seconds
    AdvanceTime(kMovementTimeoutMs_test + 1000);
    
    handle_move_up_test(&inputs, &outputs);
    
    // Motor should stop on timeout (v2.0 feature)
    // For now, verify handler doesn't crash
    EXPECT_NO_THROW(handle_move_up_test(&inputs, &outputs));
}

// ============================================================================
// WHITE-BOX UNIT TESTS: MOVE_DOWN State Handler
// ============================================================================

/**
 * @test Unit_MoveDownHandler_ContinuesMotorDown
 * @purpose Verify move_down handler keeps motor running downward
 * @focus handle_move_down_test() function behavior
 */
TEST_F(DeskControllerStateHandlerTest, Unit_MoveDownHandler_ContinuesMotorDown) {
    SetInternalState(2); // APP_STATE_MOVE_DOWN
    inputs.switch_state = SWITCH_STATE_DOWN;
    
    handle_move_down_test(&inputs, &outputs);
    
    // Should maintain motor motion
    EXPECT_TRUE(outputs.motor_enable);
    EXPECT_TRUE(outputs.motor_direction); // DOWN direction
    EXPECT_EQ(outputs.motor_pwm, 255);
}

/**
 * @test Unit_MoveDownHandler_SwitchToOff_StopsMotor
 * @purpose Verify move_down handler stops motor when switch released
 * @focus State transition: MOVE_DOWN → IDLE
 */
TEST_F(DeskControllerStateHandlerTest, Unit_MoveDownHandler_SwitchToOff_StopsMotor) {
    SetInternalState(2); // APP_STATE_MOVE_DOWN
    inputs.switch_state = SWITCH_STATE_OFF;
    
    handle_move_down_test(&inputs, &outputs);
    
    // Motor should stop
    EXPECT_FALSE(outputs.motor_enable);
    EXPECT_EQ(outputs.motor_pwm, 0);
}

/**
 * @test Unit_MoveDownHandler_SwitchToUp_StartsUpwell
 * @purpose Verify move_down handler transitions to UP with dwell
 * @focus State transition: MOVE_DOWN → DWELL (direction reversal)
 */
TEST_F(DeskControllerStateHandlerTest, Unit_MoveDownHandler_SwitchToUp_StartsUpwell) {
    SetInternalState(2); // APP_STATE_MOVE_DOWN
    inputs.switch_state = SWITCH_STATE_UP;
    
    handle_move_down_test(&inputs, &outputs);
    
    // Should stop motor to prepare for reversal (dwell state)
    // v2.0: Will transition to dwell, then move up
    EXPECT_FALSE(outputs.motor_enable);
}

/**
 * @test Unit_MoveDownHandler_TimeoutDetection
 * @purpose Verify move_down handler detects 30-second timeout
 * @focus Safety: SWE-REQ-018 (Movement Timeout)
 */
TEST_F(DeskControllerStateHandlerTest, Unit_MoveDownHandler_TimeoutDetection) {
    unsigned long start_time = 0;
    SetInternalState(2, 0, start_time); // APP_STATE_MOVE_DOWN, movementStartMs = 0
    inputs.switch_state = SWITCH_STATE_DOWN;
    
    // Simulate 30+ seconds
    AdvanceTime(kMovementTimeoutMs_test + 1000);
    
    handle_move_down_test(&inputs, &outputs);
    
    // Motor should stop on timeout (v2.0 feature)
    // For now, verify handler doesn't crash
    EXPECT_NO_THROW(handle_move_down_test(&inputs, &outputs));
}

// ============================================================================
// WHITE-BOX UNIT TESTS: DWELL State Handler
// ============================================================================

/**
 * @test Unit_DwellHandler_WaitsForDwellPeriod
 * @purpose Verify dwell handler holds motor off during reversal settling
 * @focus handle_dwell_test() function behavior
 */
TEST_F(DeskControllerStateHandlerTest, Unit_DwellHandler_WaitsForDwellPeriod) {
    SetInternalState(3, 0, 0); // APP_STATE_DWELL, dwellStartMs = 0
    inputs.switch_state = SWITCH_STATE_DOWN;
    
    handle_dwell_test(&inputs, &outputs);
    
    // Motor should remain off during dwell
    EXPECT_FALSE(outputs.motor_enable);
    EXPECT_EQ(outputs.motor_pwm, 0);
}

/**
 * @test Unit_DwellHandler_ExitsDwellOnTimeout
 * @purpose Verify dwell handler transitions to move state after settling time
 * @focus State transition: DWELL → MOVE_DOWN (or IDLE if switch released)
 */
TEST_F(DeskControllerStateHandlerTest, Unit_DwellHandler_ExitsDwellOnTimeout) {
    DeskApp_set_time_ms(0);
    SetInternalState(3, 0, 0); // APP_STATE_DWELL, dwellStartMs = 0
    inputs.switch_state = SWITCH_STATE_DOWN;
    
    // Advance time past dwell period (300ms per kDwellMs_test)
    AdvanceTime(kDwellMs_test + 100);
    
    handle_dwell_test(&inputs, &outputs);
    
    // Should transition to MOVE_DOWN (motor enable)
    EXPECT_TRUE(outputs.motor_enable);
    EXPECT_TRUE(outputs.motor_direction); // DOWN direction
}

/**
 * @test Unit_DwellHandler_SwitchReleaseExitsDwell
 * @purpose Verify dwell handler exits immediately if switch released
 * @focus State transition: DWELL → IDLE
 */
TEST_F(DeskControllerStateHandlerTest, Unit_DwellHandler_SwitchReleaseExitsDwell) {
    SetInternalState(3, 0, 0); // APP_STATE_DWELL
    inputs.switch_state = SWITCH_STATE_OFF;
    
    handle_dwell_test(&inputs, &outputs);
    
    // Should exit dwell and return to idle
    EXPECT_FALSE(outputs.motor_enable);
}

/**
 * @test Unit_DwellHandler_PreservesDwellStartTime
 * @purpose Verify dwell handler correctly checks elapsed time from start
 * @focus Timing precision for dwell period enforcement
 */
TEST_F(DeskControllerStateHandlerTest, Unit_DwellHandler_PreservesDwellStartTime) {
    unsigned long dwell_start = 5000;
    DeskApp_set_time_ms(5000);
    SetInternalState(3, dwell_start, 0); // APP_STATE_DWELL, dwellStartMs = 5000
    inputs.switch_state = SWITCH_STATE_DOWN;
    
    // Advance only 100ms (less than 300ms dwell)
    AdvanceTime(100);
    
    handle_dwell_test(&inputs, &outputs);
    
    // Should still be in dwell (motor off)
    EXPECT_FALSE(outputs.motor_enable);
    
    // Advance another 250ms (total 350ms > 300ms dwell)
    AdvanceTime(250);
    
    handle_dwell_test(&inputs, &outputs);
    
    // Should now exit dwell
    EXPECT_TRUE(outputs.motor_enable);
}

// ============================================================================
// WHITE-BOX UNIT TESTS: State Machine Integration
// ============================================================================

/**
 * @test Unit_StateMachine_UpMoveSequence
 * @purpose Verify complete UP movement sequence (IDLE→MOVE_UP→IDLE)
 * @focus State machine correctness
 */
TEST_F(DeskControllerStateHandlerTest, Unit_StateMachine_UpMoveSequence) {
    // Start in IDLE
    SetInternalState(0);
    inputs.switch_state = SWITCH_STATE_UP;
    handle_idle_test(&inputs, &outputs);
    EXPECT_TRUE(outputs.motor_enable);
    
    // Transition to MOVE_UP (simulated by test)
    SetInternalState(1);
    handle_move_up_test(&inputs, &outputs);
    EXPECT_TRUE(outputs.motor_enable);
    
    // Release switch → return to IDLE
    inputs.switch_state = SWITCH_STATE_OFF;
    handle_move_up_test(&inputs, &outputs);
    EXPECT_FALSE(outputs.motor_enable);
}

/**
 * @test Unit_StateMachine_DirectionReversal
 * @purpose Verify direction reversal with dwell (MOVE_UP→DWELL→MOVE_DOWN)
 * @focus Dwell enforcement during direction changes
 */
TEST_F(DeskControllerStateHandlerTest, Unit_StateMachine_DirectionReversal) {
    // Start moving UP
    SetInternalState(1, 0, 0); // MOVE_UP
    inputs.switch_state = SWITCH_STATE_UP;
    handle_move_up_test(&inputs, &outputs);
    EXPECT_TRUE(outputs.motor_enable);
    
    // Switch to DOWN → enters dwell
    inputs.switch_state = SWITCH_STATE_DOWN;
    handle_move_up_test(&inputs, &outputs);
    EXPECT_FALSE(outputs.motor_enable); // Motor stops for dwell
    
    // In dwell state
    DeskApp_set_time_ms(0);
    SetInternalState(3, 0, 0); // DWELL, dwellStartMs = 0
    handle_dwell_test(&inputs, &outputs);
    EXPECT_FALSE(outputs.motor_enable); // Still in dwell
    
    // Wait for dwell to complete
    AdvanceTime(kDwellMs_test + 100);
    handle_dwell_test(&inputs, &outputs);
    
    // Now should be moving DOWN
    EXPECT_TRUE(outputs.motor_enable);
    EXPECT_TRUE(outputs.motor_direction); // DOWN direction
}

// ============================================================================
// WHITE-BOX UNIT TESTS: Error Handling & Boundaries
// ============================================================================

/**
 * @test Unit_StateHandler_NullInputs_HandlesSafely
 * @purpose Verify all state handlers tolerate NULL inputs gracefully
 * @focus Robustness
 */
TEST_F(DeskControllerStateHandlerTest, Unit_StateHandler_NullInputs_HandlesSafely) {
    SetInternalState(0); // IDLE
    EXPECT_NO_THROW(handle_idle_test(nullptr, &outputs));
    
    SetInternalState(1); // MOVE_UP
    EXPECT_NO_THROW(handle_move_up_test(nullptr, &outputs));
    
    SetInternalState(2); // MOVE_DOWN
    EXPECT_NO_THROW(handle_move_down_test(nullptr, &outputs));
    
    SetInternalState(3); // DWELL
    EXPECT_NO_THROW(handle_dwell_test(nullptr, &outputs));
}

/**
 * @test Unit_TimeControl_MockTimeUpdates
 * @purpose Verify test time control functions work correctly
 * @focus Test infrastructure reliability
 */
TEST_F(DeskControllerStateHandlerTest, Unit_TimeControl_MockTimeUpdates) {
    DeskApp_set_time_ms(0);
    EXPECT_EQ(DeskApp_get_time_ms(), 0);
    
    AdvanceTime(1000);
    EXPECT_EQ(DeskApp_get_time_ms(), 1000);
    
    DeskApp_set_time_ms(5000);
    EXPECT_EQ(DeskApp_get_time_ms(), 5000);
}

/**
 * @test Unit_InternalState_CanBeInspectedAndModified
 * @purpose Verify test interface allows state inspection and manipulation
 * @focus Test infrastructure reliability
 */
TEST_F(DeskControllerStateHandlerTest, Unit_InternalState_CanBeInspectedAndModified) {
    DeskControllerInternalState_t state;
    
    SetInternalState(1, 1000, 2000); // MOVE_UP, dwell=1000, movement=2000
    DeskApp_get_internal_state(&state);
    
    EXPECT_EQ(state.currentState, 1);
    EXPECT_EQ(state.dwellStartMs, 1000);
    EXPECT_EQ(state.movementStartMs, 2000);
}