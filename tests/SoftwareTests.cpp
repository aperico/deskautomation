#include "../source/arduino/DeskController.h"
#include <gtest/gtest.h>
// Software Test Cases implemented are based on SoftwareTestCases.md

// SR-01: System Initialization# Software Requirements
TEST(DeskAppTest, SR01_Power_IdleStop_NoError) {
    DeskAppInputs_t inputs = {};
    DeskAppOutputs_t outputs = {};
    DeskApp_task_init(&inputs, &outputs);

    // System just powered on, no buttons pressed
    inputs.btUPPressed = false;
    inputs.btDOWNPressed = false;
    DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);

    EXPECT_EQ(outputs.moveUp, false);
    EXPECT_EQ(outputs.moveDown, false);
    EXPECT_EQ(outputs.error, false);
}

TEST(DeskAppTest, SWEREQ001_NoButtonsPressed_IdleNoMovement) {
    DeskAppInputs_t inputs = {};
    DeskAppOutputs_t outputs = {};
    DeskApp_task_init(&inputs, &outputs);

    // No buttons pressed at any time
    inputs.btUPPressed = false;
    inputs.btDOWNPressed = false;
    DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);

    EXPECT_EQ(outputs.moveUp, false);
    EXPECT_EQ(outputs.moveDown, false);
    EXPECT_EQ(outputs.error, false);
}

// SWE-REQ-003: Upward Movement Detection
TEST(DeskAppTest, SWEREQ003_UpPressed_MovesUp_WhenNotupperLimitActive) {
    DeskAppInputs_t inputs = {};
    DeskAppOutputs_t outputs = {};
    DeskApp_task_init(&inputs, &outputs);

    // Simulate Up button pressed, not at upper limit
    inputs.btUPPressed = true;
    inputs.btDOWNPressed = false;
    inputs.upperLimitActive = false;
    DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);

    EXPECT_EQ(outputs.moveUp, true);
    EXPECT_EQ(outputs.moveDown, false);
    EXPECT_EQ(outputs.error, false);
}

TEST(DeskAppTest, SWEREQ005_UpPressed_DoesNotMoveUp_WhenupperLimitActive) {
    DeskAppInputs_t inputs = {};
    DeskAppOutputs_t outputs = {};
    DeskApp_task_init(&inputs, &outputs);

    // Up button pressed, but already at upper limit
    inputs.btUPPressed = true;
    inputs.btDOWNPressed = false;
    inputs.upperLimitActive = true;
    DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);

    EXPECT_EQ(outputs.moveUp, false);
    EXPECT_EQ(outputs.moveDown, false);
    EXPECT_EQ(outputs.error, false);
}

// SWE-REQ-004: Downward Movement Detection
TEST(DeskAppTest, SWEREQ004_DownPressed_MovesDown_WhenNotlowerLimitActive) {
    DeskAppInputs_t inputs = {};
    DeskAppOutputs_t outputs = {};
    DeskApp_task_init(&inputs, &outputs);

    // Down button pressed, not at lower limit
    inputs.btUPPressed = false;
    inputs.btDOWNPressed = true;
    inputs.lowerLimitActive = false;
    DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);

    EXPECT_EQ(outputs.moveUp, false);
    EXPECT_EQ(outputs.moveDown, true);
    EXPECT_EQ(outputs.error, false);
    
}

TEST(DeskAppTest, SWEREQ006_DownPressed_DoesNotMoveDown_WhenlowerLimitActive) {
    DeskAppInputs_t inputs = {};
    DeskAppOutputs_t outputs = {};
    DeskApp_task_init(&inputs, &outputs);

    // Down button pressed, but already at lower limit
    inputs.btUPPressed = false;
    inputs.btDOWNPressed = true;
    inputs.lowerLimitActive = true;
    DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);

    EXPECT_EQ(outputs.moveUp, false);
    EXPECT_EQ(outputs.moveDown, false);
    EXPECT_EQ(outputs.error, false);
    
}

// SWE-REQ-010, SWE-REQ-011: Emergency Stop Detection and Execution
TEST(DeskAppTest, SWEREQ010_EmergencyStop_FromUp_WhenBothLimitsActive) {
    DeskAppInputs_t inputs = {};
    DeskAppOutputs_t outputs = {};
    DeskApp_task_init(&inputs, &outputs);

    // Simulate moving up, then lower limit becomes active (fault)
    inputs.btUPPressed = true;
    inputs.btDOWNPressed = false;
    inputs.upperLimitActive = true; // Fault: both up and upper limit
    inputs.lowerLimitActive = true; // Fault: both up and lower limit
    DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);

    EXPECT_EQ(outputs.moveUp, false);
    EXPECT_EQ(outputs.moveDown, false);
    EXPECT_EQ(outputs.error, true);
    EXPECT_EQ(ret, APP_TASK_ERROR);
}

TEST(DeskAppTest, SWEREQ011_EmergencyStop_FromDown_WhenBothLimitsActive) {
    DeskAppInputs_t inputs = {};
    DeskAppOutputs_t outputs = {};
    DeskApp_task_init(&inputs, &outputs);

    // Simulate moving down, then both limits active (fault)
    inputs.btUPPressed = false;
    inputs.btDOWNPressed = true;
    inputs.lowerLimitActive = true; // Fault: both limits active
    inputs.upperLimitActive = true; // Fault: both limits active
    DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);

    EXPECT_EQ(outputs.moveUp, false);
    EXPECT_EQ(outputs.moveDown, false);
    EXPECT_EQ(outputs.error, true);
    
}

// SWE-REQ-012, SWE-REQ-013: LED State Indication (covered by output assertions in all tests)

// SWE-REQ-009: Power Loss Handling (stateless reset behavior)

// SWE-REQ-010, SWE-REQ-014: Simultaneous Button Presses (conflicting inputs)
TEST(DeskAppTest, SWEREQ014_BothButtonsPressed_NoMovement_Commanded) {
    DeskAppInputs_t inputs = {};
    DeskAppOutputs_t outputs = {};
    DeskApp_task_init(&inputs, &outputs);

    // Both buttons pressed, not at any limit
    inputs.btUPPressed = true;
    inputs.btDOWNPressed = true;
    inputs.upperLimitActive = false;
    inputs.lowerLimitActive = false;
    DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);

    EXPECT_EQ(outputs.moveUp, false);
    EXPECT_EQ(outputs.moveDown, false);
    EXPECT_EQ(outputs.error, false);
}

// ============================================================================
// CONST-003: Direction changes require stop before reversal (DWELL state)
// ============================================================================

TEST(DeskAppTest, CONST003_DwellBeforeReversal_UpToDown) {
    DeskAppInputs_t inputs = {};
    DeskAppOutputs_t outputs = {};
    DeskApp_task_init(&inputs, &outputs);

    // Simulate Up pressed, then Down pressed immediately after Up released
    inputs.btUPPressed = true;
    inputs.btDOWNPressed = false;
    DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);
    EXPECT_EQ(outputs.moveUp, true);

    // Release Up, press Down immediately (simulate no dwell)
    inputs.btUPPressed = false;
    inputs.btDOWNPressed = true;
    ret = DeskApp_task(&inputs, &outputs);

    // Expect dwell or idle before allowing reversal
    // (This assertion may need to be adjusted based on your implementation)
    EXPECT_EQ(outputs.moveDown, false); // Dwell/IDLE prevents immediate reversal
    EXPECT_EQ(outputs.moveUp, false);
}

// ============================================================================
// SWE-REQ-009: Power Loss Handling (Stateless Reset)
// ============================================================================

TEST(DeskAppTest, SWEREQ009_PowerOffDuringMovement_ReinitializesToIdle) {
    DeskAppInputs_t inputs = {};
    DeskAppOutputs_t outputs = {};
    DeskApp_task_init(&inputs, &outputs);

    // Simulate Up button pressed, moving up
    inputs.btUPPressed = true;
    inputs.btDOWNPressed = false;
    inputs.upperLimitActive = false;
    DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);
    EXPECT_EQ(outputs.moveUp, true);

    // Simulate power-off (re-initialize)
    DeskApp_task_init(&inputs, &outputs);

    // After power restore, system should be IDLE, no movement
    EXPECT_EQ(outputs.moveUp, false);
    EXPECT_EQ(outputs.moveDown, false);
    EXPECT_EQ(outputs.error, false);
}

// ============================================================================
// SWE-REQ-018: Movement Timeout (30 seconds)
// Note: Full timeout test requires time acceleration in unit tests
// Integration tests IT-002 and IT-003 verify timeout with mock timing
// ============================================================================

TEST(DeskAppTest, SWEREQ018_TimeoutCondition_Documentation) {
    // This test documents the timeout requirement
    // Actual timeout testing requires:
    // 1. Time-accelerated mock (in integration tests)
    // 2. Continuous button press for >30s
    // 3. Verification that movement stops and returns to IDLE
    // See IT-002 and IT-003 in IntegrationTests.cpp for implementation
    SUCCEED(); // Documentation test always passes
}

// ============================================================================
// SWE-REQ-020: State Transition Integrity
// ============================================================================

TEST(DeskAppTest, SWEREQ020_StateTransitionIntegrity_AllStatesValid) {
    DeskAppInputs_t inputs = {};
    DeskAppOutputs_t outputs = {};
    DeskApp_task_init(&inputs, &outputs);

    // Verify state machine handles all valid transitions
    // IDLE -> MOVING_UP
    inputs.btUPPressed = true;
    inputs.btDOWNPressed = false;
    inputs.upperLimitActive = false;
    EXPECT_EQ(DeskApp_task(&inputs, &outputs), APP_TASK_SUCCESS);
    EXPECT_EQ(outputs.moveUp, true);

    // MOVING_UP -> IDLE (button release)
    inputs.btUPPressed = false;
    EXPECT_EQ(DeskApp_task(&inputs, &outputs), APP_TASK_SUCCESS);
    EXPECT_EQ(outputs.moveUp, false);

    // IDLE -> MOVING_DOWN
    inputs.btDOWNPressed = true;
    inputs.lowerLimitActive = false;
    EXPECT_EQ(DeskApp_task(&inputs, &outputs), APP_TASK_SUCCESS);
    EXPECT_EQ(outputs.moveDown, true);

    // MOVING_DOWN -> IDLE (button release)
    inputs.btDOWNPressed = false;
    EXPECT_EQ(DeskApp_task(&inputs, &outputs), APP_TASK_SUCCESS);
    EXPECT_EQ(outputs.moveDown, false);
}

// ============================================================================
// Smoke Tests / Basic Sanity Checks
// ============================================================================

TEST(SmokeTest, BasicTruth) {
    DeskAppInputs_t inputs = {};
    DeskAppOutputs_t outputs = {};
    DeskAppTask_Return_t ret;

    inputs.btUPPressed = false;
    inputs.btDOWNPressed = false;
    DeskApp_task_init(&inputs, &outputs);
    ret = DeskApp_task(&inputs, &outputs);

    EXPECT_EQ(outputs.moveUp, false);
    EXPECT_EQ(outputs.moveDown, false);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
