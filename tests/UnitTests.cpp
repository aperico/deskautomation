#include <gtest/gtest.h>
#include "DeskController.h"
#include "DeskControllerTestInterface.h"

// Test fixture for white-box unit tests
class DeskControllerWhiteBoxTest : public ::testing::Test {
protected:
    DeskAppInputs_t inputs;
    DeskAppOutputs_t outputs;
    DeskControllerInternalState_t state;

    void SetUp() override {
        // Initialize inputs to safe defaults
        inputs.btUPPressed = false;
        inputs.btDOWNPressed = false;
        inputs.upperLimitActive = false;
        inputs.lowerLimitActive = false;

        // Initialize outputs
        outputs.moveUp = false;
        outputs.moveDown = false;
        outputs.stop = false;
        outputs.error = false;

        // Reset time
        DeskApp_set_time_ms(0);

        // Initialize to IDLE state
        DeskApp_task_init(&inputs, &outputs);
    }

    void SetState(int stateValue, unsigned long dwellMs = 0, unsigned long movementMs = 0) {
        state.currentState = static_cast<AppState_t>(stateValue);
        state.dwellStartMs = dwellMs;
        state.movementStartMs = movementMs;
        DeskApp_set_internal_state(&state);
    }

    void GetState() {
        DeskApp_get_internal_state(&state);
    }
};

// =============================================================================
// handle_idle() Tests
// =============================================================================

TEST_F(DeskControllerWhiteBoxTest, HandleIdle_NoButtonPressed_StaysIdleOutputsStop) {
    SetState(0); // APP_STATE_IDLE

    inputs.btUPPressed = false;
    inputs.btDOWNPressed = false;
    
    handle_idle_test(&inputs, &outputs);

    EXPECT_FALSE(outputs.moveUp);
    EXPECT_FALSE(outputs.moveDown);
    EXPECT_TRUE(outputs.stop);
    EXPECT_FALSE(outputs.error);
    
    GetState();
    EXPECT_EQ(0, state.currentState); // Still IDLE
}

TEST_F(DeskControllerWhiteBoxTest, HandleIdle_UpButtonPressed_NoUpperLimit_TransitionsToMoveUp) {
    SetState(0); // APP_STATE_IDLE
    DeskApp_set_time_ms(1000);

    inputs.btUPPressed = true;
    inputs.upperLimitActive = false;
    
    handle_idle_test(&inputs, &outputs);

    EXPECT_TRUE(outputs.moveUp);
    EXPECT_FALSE(outputs.moveDown);
    EXPECT_FALSE(outputs.stop);
    
    GetState();
    EXPECT_EQ(1, state.currentState); // APP_STATE_MOVE_UP
    EXPECT_EQ(1000, state.movementStartMs);
}

TEST_F(DeskControllerWhiteBoxTest, HandleIdle_UpButtonPressed_AtUpperLimit_OutputsStop) {
    SetState(0); // APP_STATE_IDLE

    inputs.btUPPressed = true;
    inputs.upperLimitActive = true;
    
    handle_idle_test(&inputs, &outputs);

    EXPECT_FALSE(outputs.moveUp);
    EXPECT_FALSE(outputs.moveDown);
    EXPECT_TRUE(outputs.stop);
    
    GetState();
    EXPECT_EQ(0, state.currentState); // Stays IDLE
}

TEST_F(DeskControllerWhiteBoxTest, HandleIdle_DownButtonPressed_NoLowerLimit_TransitionsToMoveDown) {
    SetState(0); // APP_STATE_IDLE
    DeskApp_set_time_ms(2000);

    inputs.btDOWNPressed = true;
    inputs.lowerLimitActive = false;
    
    handle_idle_test(&inputs, &outputs);

    EXPECT_FALSE(outputs.moveUp);
    EXPECT_TRUE(outputs.moveDown);
    EXPECT_FALSE(outputs.stop);
    
    GetState();
    EXPECT_EQ(2, state.currentState); // APP_STATE_MOVE_DOWN
    EXPECT_EQ(2000, state.movementStartMs);
}

TEST_F(DeskControllerWhiteBoxTest, HandleIdle_DownButtonPressed_AtLowerLimit_OutputsStop) {
    SetState(0); // APP_STATE_IDLE

    inputs.btDOWNPressed = true;
    inputs.lowerLimitActive = true;
    
    handle_idle_test(&inputs, &outputs);

    EXPECT_FALSE(outputs.moveUp);
    EXPECT_FALSE(outputs.moveDown);
    EXPECT_TRUE(outputs.stop);
    
    GetState();
    EXPECT_EQ(0, state.currentState); // Stays IDLE
}

TEST_F(DeskControllerWhiteBoxTest, HandleIdle_BothButtonsPressed_OutputsStop) {
    SetState(0); // APP_STATE_IDLE

    inputs.btUPPressed = true;
    inputs.btDOWNPressed = true;
    
    handle_idle_test(&inputs, &outputs);

    EXPECT_FALSE(outputs.moveUp);
    EXPECT_FALSE(outputs.moveDown);
    EXPECT_TRUE(outputs.stop);
    
    GetState();
    EXPECT_EQ(0, state.currentState); // Stays IDLE
}

// =============================================================================
// handle_move_up() Tests
// =============================================================================

TEST_F(DeskControllerWhiteBoxTest, HandleMoveUp_NormalMovement_ContinuesMoving) {
    DeskApp_set_time_ms(1000);
    SetState(1, 0, 500); // APP_STATE_MOVE_UP, started at 500ms

    inputs.btUPPressed = true;
    inputs.upperLimitActive = false;
    
    handle_move_up_test(&inputs, &outputs);

    EXPECT_TRUE(outputs.moveUp);
    EXPECT_FALSE(outputs.moveDown);
    EXPECT_FALSE(outputs.stop);
    
    GetState();
    EXPECT_EQ(1, state.currentState); // Still MOVE_UP
}

TEST_F(DeskControllerWhiteBoxTest, HandleMoveUp_ButtonReleased_StopsAndReturnsToIdle) {
    DeskApp_set_time_ms(1000);
    SetState(1, 0, 500); // APP_STATE_MOVE_UP

    inputs.btUPPressed = false; // Button released
    inputs.upperLimitActive = false;
    
    handle_move_up_test(&inputs, &outputs);

    EXPECT_FALSE(outputs.moveUp);
    EXPECT_FALSE(outputs.moveDown);
    EXPECT_TRUE(outputs.stop);
    
    GetState();
    EXPECT_EQ(0, state.currentState); // Back to IDLE
}

TEST_F(DeskControllerWhiteBoxTest, HandleMoveUp_UpperLimitReached_StopsAndTransitionsToDwell) {
    DeskApp_set_time_ms(2000);
    SetState(1, 0, 1000); // APP_STATE_MOVE_UP

    inputs.btUPPressed = true;
    inputs.upperLimitActive = true; // Limit reached
    
    handle_move_up_test(&inputs, &outputs);

    EXPECT_FALSE(outputs.moveUp);
    EXPECT_FALSE(outputs.moveDown);
    EXPECT_TRUE(outputs.stop);
    
    GetState();
    EXPECT_EQ(3, state.currentState); // APP_STATE_DWELL
    EXPECT_EQ(2000, state.dwellStartMs);
}

TEST_F(DeskControllerWhiteBoxTest, HandleMoveUp_TimeoutReached_StopsAndReturnsToIdle) {
    DeskApp_set_time_ms(31000);
    SetState(1, 0, 1000); // APP_STATE_MOVE_UP, started at 1000ms

    inputs.btUPPressed = true;
    inputs.upperLimitActive = false;
    
    handle_move_up_test(&inputs, &outputs);

    EXPECT_FALSE(outputs.moveUp);
    EXPECT_FALSE(outputs.moveDown);
    EXPECT_TRUE(outputs.stop);
    
    GetState();
    EXPECT_EQ(0, state.currentState); // Back to IDLE due to timeout
}

TEST_F(DeskControllerWhiteBoxTest, HandleMoveUp_TimeoutExactBoundary_StopsAndReturnsToIdle) {
    DeskApp_set_time_ms(30000);
    SetState(1, 0, 0); // APP_STATE_MOVE_UP, started at 0ms

    inputs.btUPPressed = true;
    inputs.upperLimitActive = false;
    
    handle_move_up_test(&inputs, &outputs);

    EXPECT_FALSE(outputs.moveUp);
    EXPECT_FALSE(outputs.moveDown);
    EXPECT_TRUE(outputs.stop);
    
    GetState();
    EXPECT_EQ(0, state.currentState); // Timeout at exact boundary
}

TEST_F(DeskControllerWhiteBoxTest, HandleMoveUp_JustBeforeTimeout_ContinuesMoving) {
    DeskApp_set_time_ms(29999);
    SetState(1, 0, 0); // APP_STATE_MOVE_UP, started at 0ms

    inputs.btUPPressed = true;
    inputs.upperLimitActive = false;
    
    handle_move_up_test(&inputs, &outputs);

    EXPECT_TRUE(outputs.moveUp);
    EXPECT_FALSE(outputs.stop);
    
    GetState();
    EXPECT_EQ(1, state.currentState); // Still moving
}

// =============================================================================
// handle_move_down() Tests
// =============================================================================

TEST_F(DeskControllerWhiteBoxTest, HandleMoveDown_NormalMovement_ContinuesMoving) {
    DeskApp_set_time_ms(1000);
    SetState(2, 0, 500); // APP_STATE_MOVE_DOWN, started at 500ms

    inputs.btDOWNPressed = true;
    inputs.lowerLimitActive = false;
    
    handle_move_down_test(&inputs, &outputs);

    EXPECT_FALSE(outputs.moveUp);
    EXPECT_TRUE(outputs.moveDown);
    EXPECT_FALSE(outputs.stop);
    
    GetState();
    EXPECT_EQ(2, state.currentState); // Still MOVE_DOWN
}

TEST_F(DeskControllerWhiteBoxTest, HandleMoveDown_ButtonReleased_StopsAndReturnsToIdle) {
    DeskApp_set_time_ms(1000);
    SetState(2, 0, 500); // APP_STATE_MOVE_DOWN

    inputs.btDOWNPressed = false; // Button released
    inputs.lowerLimitActive = false;
    
    handle_move_down_test(&inputs, &outputs);

    EXPECT_FALSE(outputs.moveUp);
    EXPECT_FALSE(outputs.moveDown);
    EXPECT_TRUE(outputs.stop);
    
    GetState();
    EXPECT_EQ(0, state.currentState); // Back to IDLE
}

TEST_F(DeskControllerWhiteBoxTest, HandleMoveDown_LowerLimitReached_StopsAndTransitionsToDwell) {
    DeskApp_set_time_ms(2000);
    SetState(2, 0, 1000); // APP_STATE_MOVE_DOWN

    inputs.btDOWNPressed = true;
    inputs.lowerLimitActive = true; // Limit reached
    
    handle_move_down_test(&inputs, &outputs);

    EXPECT_FALSE(outputs.moveUp);
    EXPECT_FALSE(outputs.moveDown);
    EXPECT_TRUE(outputs.stop);
    
    GetState();
    EXPECT_EQ(3, state.currentState); // APP_STATE_DWELL
    EXPECT_EQ(2000, state.dwellStartMs);
}

TEST_F(DeskControllerWhiteBoxTest, HandleMoveDown_TimeoutReached_StopsAndReturnsToIdle) {
    DeskApp_set_time_ms(31000);
    SetState(2, 0, 1000); // APP_STATE_MOVE_DOWN, started at 1000ms

    inputs.btDOWNPressed = true;
    inputs.lowerLimitActive = false;
    
    handle_move_down_test(&inputs, &outputs);

    EXPECT_FALSE(outputs.moveUp);
    EXPECT_FALSE(outputs.moveDown);
    EXPECT_TRUE(outputs.stop);
    
    GetState();
    EXPECT_EQ(0, state.currentState); // Back to IDLE due to timeout
}

TEST_F(DeskControllerWhiteBoxTest, HandleMoveDown_TimeoutExactBoundary_StopsAndReturnsToIdle) {
    DeskApp_set_time_ms(30000);
    SetState(2, 0, 0); // APP_STATE_MOVE_DOWN, started at 0ms

    inputs.btDOWNPressed = true;
    inputs.lowerLimitActive = false;
    
    handle_move_down_test(&inputs, &outputs);

    EXPECT_FALSE(outputs.moveUp);
    EXPECT_FALSE(outputs.moveDown);
    EXPECT_TRUE(outputs.stop);
    
    GetState();
    EXPECT_EQ(0, state.currentState); // Timeout at exact boundary
}

// =============================================================================
// handle_dwell() Tests
// =============================================================================

TEST_F(DeskControllerWhiteBoxTest, HandleDwell_DwellPeriodNotElapsed_StaysInDwell) {
    DeskApp_set_time_ms(1200);
    SetState(3, 1000, 0); // APP_STATE_DWELL, started at 1000ms

    handle_dwell_test(&inputs, &outputs);

    EXPECT_FALSE(outputs.moveUp);
    EXPECT_FALSE(outputs.moveDown);
    EXPECT_TRUE(outputs.stop);
    
    GetState();
    EXPECT_EQ(3, state.currentState); // Still in DWELL (200ms elapsed, need 300ms)
}

TEST_F(DeskControllerWhiteBoxTest, HandleDwell_DwellPeriodExactlyElapsed_TransitionsToIdle) {
    DeskApp_set_time_ms(1300);
    SetState(3, 1000, 0); // APP_STATE_DWELL, started at 1000ms

    handle_dwell_test(&inputs, &outputs);

    EXPECT_FALSE(outputs.moveUp);
    EXPECT_FALSE(outputs.moveDown);
    EXPECT_TRUE(outputs.stop);
    
    GetState();
    EXPECT_EQ(0, state.currentState); // Transitioned to IDLE (300ms elapsed)
}

TEST_F(DeskControllerWhiteBoxTest, HandleDwell_DwellPeriodExceeded_TransitionsToIdle) {
    DeskApp_set_time_ms(2000);
    SetState(3, 1000, 0); // APP_STATE_DWELL, started at 1000ms

    handle_dwell_test(&inputs, &outputs);

    EXPECT_FALSE(outputs.moveUp);
    EXPECT_FALSE(outputs.moveDown);
    EXPECT_TRUE(outputs.stop);
    
    GetState();
    EXPECT_EQ(0, state.currentState); // Transitioned to IDLE (1000ms elapsed)
}

TEST_F(DeskControllerWhiteBoxTest, HandleDwell_JustBeforeDwellComplete_StaysInDwell) {
    DeskApp_set_time_ms(1299);
    SetState(3, 1000, 0); // APP_STATE_DWELL, started at 1000ms

    handle_dwell_test(&inputs, &outputs);

    EXPECT_TRUE(outputs.stop);
    
    GetState();
    EXPECT_EQ(3, state.currentState); // Still in DWELL (299ms elapsed)
}

// =============================================================================
// Time Control Tests
// =============================================================================

TEST_F(DeskControllerWhiteBoxTest, TimeControl_SetAndGetTime) {
    DeskApp_set_time_ms(12345);
    EXPECT_EQ(12345, DeskApp_get_time_ms());

    DeskApp_set_time_ms(0);
    EXPECT_EQ(0, DeskApp_get_time_ms());
}

// =============================================================================
// Constants Validation Tests
// =============================================================================

TEST_F(DeskControllerWhiteBoxTest, Constants_DwellTimeIs300ms) {
    EXPECT_EQ(300UL, kDwellMs_test);
}

TEST_F(DeskControllerWhiteBoxTest, Constants_MovementTimeoutIs30Seconds) {
    EXPECT_EQ(30000UL, kMovementTimeoutMs_test);
}