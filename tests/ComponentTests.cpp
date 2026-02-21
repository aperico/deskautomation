#include <gtest/gtest.h>
#include "desk_app.h"
#include "desk_types.h"

// ============================================================================
// TEST CASE SPECIFICATION: Application Layer Component Tests
// ============================================================================
// PURPOSE: Verify software requirements (SWReq-001 through SWReq-011) for 
//          desk automation control logic independent of hardware (HAL).
//
// SCOPE: Tests the desk_app component which implements:
//   - Button input processing (SWReq-001, SWReq-002, SWReq-003, SWReq-009)
//   - State machine (SWReq-007, SWReq-010)
//   - Safety logic: conflicting inputs, limit protection (SWReq-004, SWReq-005, SWReq-006)
//   - Control loop management (SWReq-008, SWReq-011)
//
// METHOD: Unit testing with mock hardware abstraction layer (HAL)
//   - Inputs: AppInput_t structure (buttons, limits, faults, timestamps)
//   - Outputs: AppOutput_t structure (motor commands, LED status, fault flags)
//   - No actual GPIO, timers, or motor drivers exercised
// ============================================================================

class DeskAppComponentTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // Precondition: Initialize application to known state (IDLE)
        APP_Init();
    }
};

// ============================================================================
// TEST CASE: TC-SWReq-007-001 - Initial State is IDLE
// ============================================================================
// Requirement ID: SWReq-007 (State Machine with IDLE, MOVING_UP, MOVING_DOWN states)
// Requirement ID: SWReq-010 (Operational state maintenance)
//
// Test Objective:
//   Verify that the application initializes to IDLE state and responds with
//   motor stop command and idle LED status.
//
// Preconditions:
//   - APP_Init() has been called
//   - No inputs asserted
//
// Test Steps:
//   1. Create zero-initialized AppInput_t with timestamp_ms = 0
//   2. Call APP_Task() with empty inputs
//   3. Verify output state
//
// Expected Results:
//   - Motor command: MOTOR_STOP
//   - Motor speed: 0
//   - LED status: LED_IDLE
//   - Application in IDLE state
//
// Rationale:
//   - Ensures safe initial state on startup
//   - Prevents unintended motor activation
//   - Satisfies SysReq-007 (limit protection requires known initial state)
// ============================================================================
TEST_F(DeskAppComponentTest, TC_SWReq007_001_InitialStateIsIdle)
{
    AppInput_t inputs = {0};
    inputs.timestamp_ms = 0U;
    
    AppOutput_t outputs;
    APP_Task(&inputs, &outputs);
    
    // Expected: Safe idle state
    EXPECT_EQ(outputs.motor_cmd, MOTOR_STOP) 
        << "Initial motor command must be STOP";
    EXPECT_EQ(outputs.motor_speed, 0U) 
        << "Initial motor speed must be zero";
    EXPECT_EQ(outputs.led_bt_up, LED_OFF) 
        << "UP button LED must be OFF in IDLE";
    EXPECT_EQ(outputs.led_bt_down, LED_OFF) 
        << "DOWN button LED must be OFF in IDLE";
    EXPECT_EQ(outputs.led_error, LED_OFF) 
        << "Error LED must be OFF in IDLE";
    EXPECT_EQ(APP_GetState(), APP_STATE_IDLE) 
        << "Application must be in IDLE state";
}

// ============================================================================
// TEST CASE: TC-SWReq-001-001 - UP Button Commands Motor UP
// ============================================================================
// Requirement ID: SWReq-001 (UP button press → motor upward direction)
// Requirement ID: SWReq-007 (State transition from IDLE to MOVING_UP)
// Requirement ID: SWReq-010 (State history maintained)
//
// Test Objective:
//   Verify that pressing UP button triggers motor upward movement command
//   and transitions to MOVING_UP state.
//
// Preconditions:
//   - Application in IDLE state (from SetUp)
//   - DOWN button not pressed
//   - Limit sensors not triggered
//
// Test Steps:
//   1. Create AppInput_t with button_up=true, others false
//   2. Call APP_Task()
//   3. Verify motor command and state transition
//
// Expected Results:
//   - Motor command: MOTOR_UP
//   - Motor speed: > 0 (non-zero)
//   - LED status: LED_ACTIVE
//   - State: MOVING_UP
//
// Rationale:
//   - Ensures responsive UP button handling
//   - Meets SysReq-002 response time requirement
//   - Provides user feedback via LED
// ============================================================================
TEST_F(DeskAppComponentTest, TC_SWReq001_001_UpButtonCommandsMotorUp)
{
    AppInput_t inputs = {0};
    inputs.button_up = true;
    inputs.button_down = false;
    inputs.limit_upper = false;
    inputs.limit_lower = false;
    inputs.timestamp_ms = 0U;
    
    AppOutput_t outputs;
    APP_Task(&inputs, &outputs);
    
    // Expected: Motor UP command with non-zero speed
    EXPECT_EQ(outputs.motor_cmd, MOTOR_UP) 
        << "UP button must command MOTOR_UP";
    EXPECT_GT(outputs.motor_speed, 0U) 
        << "Motor speed must be non-zero when moving up";
    EXPECT_EQ(outputs.led_bt_up, LED_ON) 
        << "UP button LED must be ON when moving up";
    EXPECT_EQ(outputs.led_bt_down, LED_OFF) 
        << "DOWN button LED must be OFF";
    EXPECT_EQ(outputs.led_error, LED_OFF) 
        << "Error LED must be OFF during normal operation";
    EXPECT_EQ(APP_GetState(), APP_STATE_MOVING_UP) 
        << "State must transition to MOVING_UP";
}

// ============================================================================
// TEST CASE: TC-SWReq-002-001 - DOWN Button Commands Motor DOWN
// ============================================================================
// Requirement ID: SWReq-002 (DOWN button press → motor downward direction)
// Requirement ID: SWReq-007 (State transition from IDLE to MOVING_DOWN)
// Requirement ID: SWReq-010 (State history maintained)
//
// Test Objective:
//   Verify that pressing DOWN button triggers motor downward movement command
//   and transitions to MOVING_DOWN state.
//
// Preconditions:
//   - Application in IDLE state
//   - UP button not pressed
//   - Limit sensors not triggered
//
// Test Steps:
//   1. Create AppInput_t with button_down=true, others false
//   2. Call APP_Task()
//   3. Verify motor command and state transition
//
// Expected Results:
//   - Motor command: MOTOR_DOWN
//   - Motor speed: > 0 (non-zero)
//   - LED status: LED_ACTIVE
//   - State: MOVING_DOWN
//
// Rationale:
//   - Ensures responsive DOWN button handling
//   - Meets SysReq-002 response time requirement
//   - Symmetric with UP button functionality
// ============================================================================
TEST_F(DeskAppComponentTest, TC_SWReq002_001_DownButtonCommandsMotorDown)
{
    AppInput_t inputs = {0};
    inputs.button_up = false;
    inputs.button_down = true;
    inputs.limit_upper = false;
    inputs.limit_lower = false;
    inputs.timestamp_ms = 0U;
    
    AppOutput_t outputs;
    APP_Task(&inputs, &outputs);
    
    // Expected: Motor DOWN command with non-zero speed
    EXPECT_EQ(outputs.motor_cmd, MOTOR_DOWN) 
        << "DOWN button must command MOTOR_DOWN";
    EXPECT_GT(outputs.motor_speed, 0U) 
        << "Motor speed must be non-zero when moving down";
    EXPECT_EQ(outputs.led_bt_up, LED_OFF) 
        << "UP button LED must be OFF";
    EXPECT_EQ(outputs.led_bt_down, LED_ON) 
        << "DOWN button LED must be ON when moving down";
    EXPECT_EQ(outputs.led_error, LED_OFF) 
        << "Error LED must be OFF during normal operation";
    EXPECT_EQ(APP_GetState(), APP_STATE_MOVING_DOWN) 
        << "State must transition to MOVING_DOWN";
}

// ============================================================================
// TEST CASE: TC-SWReq-014-001 - Current Sense Fault Latches Error
// ============================================================================
// Requirement ID: SWReq-014 (Stuck-on/runaway detection)
//
// Test Objective:
//   Verify that excessive motor current while STOP is commanded latches fault.
//
// Preconditions:
//   - Application in IDLE state
//   - No buttons pressed
//
// Test Steps:
//   1. Provide motor_current_ma above threshold at t=0
//   2. Call APP_Task() -> should not fault yet (timer starts)
//   3. Provide same current at t=150ms
//   4. Call APP_Task() -> fault should latch
//
// Expected Results:
//   - Fault output asserted
//   - Error LED ON
//   - State transitions to FAULT
// ============================================================================
TEST_F(DeskAppComponentTest, TC_SWReq014_001_CurrentSenseFaultLatches)
{
    AppInput_t inputs = {0};
    inputs.motor_current_ma = 200U;
    inputs.timestamp_ms = 0U;

    AppOutput_t outputs;
    APP_Task(&inputs, &outputs);
    EXPECT_FALSE(outputs.fault_out)
        << "Fault should not latch on first sample";

    inputs.timestamp_ms = 150U;
    APP_Task(&inputs, &outputs);

    EXPECT_TRUE(outputs.fault_out)
        << "Fault must latch after current exceeds threshold for >100ms";
    EXPECT_EQ(outputs.led_error, LED_ON)
        << "Error LED must be ON during fault";
    EXPECT_EQ(APP_GetState(), APP_STATE_FAULT)
        << "State must transition to FAULT";
}

// ============================================================================
// TEST CASE: TC-SWReq-005-001 - Upper Limit Stops Upward Movement
// ============================================================================
// Requirement ID: SWReq-005 (Upper limit override: stop when limit active)
// Requirement ID: SysReq-007 (Limit protection)
//
// Test Objective:
//   SAFETY-CRITICAL: Verify that upper limit sensor prevents upward movement
//   even when UP button is pressed. This protects mechanical travel limits.
//
// Preconditions:
//   - Application in IDLE state
//   - Upper limit sensor is active (true)
//   - UP button pressed
//
// Test Steps:
//   1. Create AppInput_t with button_up=true, limit_upper=true
//   2. Call APP_Task()
//   3. Verify motor is commanded to STOP
//
// Expected Results:
//   - Motor command: MOTOR_STOP
//   - Motor speed: 0
//   - State: IDLE (no transition to MOVING_UP)
//
// Rationale:
//   - SAFETY-CRITICAL requirement for preventing over-travel
//   - Software must override user input when hardware limit reached
//   - Meets SysReq-007 mechanical protection requirement
// ============================================================================
TEST_F(DeskAppComponentTest, TC_SWReq005_001_UpperLimitStopsUpwardMovement)
{
    AppInput_t inputs = {0};
    inputs.button_up = true;
    inputs.limit_upper = true;   // SAFETY-CRITICAL: Limit is active
    inputs.limit_lower = false;
    inputs.timestamp_ms = 0U;
    
    AppOutput_t outputs;
    APP_Task(&inputs, &outputs);
    
    // SAFETY-CRITICAL: Must prevent upward movement
    EXPECT_EQ(outputs.motor_cmd, MOTOR_STOP) 
        << "SAFETY: Upper limit must override UP button and force STOP";
    EXPECT_EQ(outputs.motor_speed, 0U) 
        << "Motor speed must be zero at limit";
    EXPECT_EQ(APP_GetState(), APP_STATE_IDLE) 
        << "Must remain in IDLE state at upper limit";
}

// ============================================================================
// TEST CASE: TC-SWReq-006-001 - Lower Limit Stops Downward Movement
// ============================================================================
// Requirement ID: SWReq-006 (Lower limit override: stop when limit active)
// Requirement ID: SysReq-007 (Limit protection)
//
// Test Objective:
//   SAFETY-CRITICAL: Verify that lower limit sensor prevents downward movement
//   even when DOWN button is pressed. This protects mechanical travel limits.
//
// Preconditions:
//   - Application in IDLE state
//   - Lower limit sensor is active (true)
//   - DOWN button pressed
//
// Test Steps:
//   1. Create AppInput_t with button_down=true, limit_lower=true
//   2. Call APP_Task()
//   3. Verify motor is commanded to STOP
//
// Expected Results:
//   - Motor command: MOTOR_STOP
//   - Motor speed: 0
//   - State: IDLE (no transition to MOVING_DOWN)
//
// Rationale:
//   - SAFETY-CRITICAL requirement for preventing over-travel
//   - Software must override user input when hardware limit reached
//   - Symmetric with upper limit protection (SWReq-005)
//   - Meets SysReq-007 mechanical protection requirement
// ============================================================================
TEST_F(DeskAppComponentTest, TC_SWReq006_001_LowerLimitStopsDownwardMovement)
{
    AppInput_t inputs = {0};
    inputs.button_down = true;
    inputs.limit_upper = false;
    inputs.limit_lower = true;   // SAFETY-CRITICAL: Limit is active
    inputs.timestamp_ms = 0U;
    
    AppOutput_t outputs;
    APP_Task(&inputs, &outputs);
    
    // SAFETY-CRITICAL: Must prevent downward movement
    EXPECT_EQ(outputs.motor_cmd, MOTOR_STOP) 
        << "SAFETY: Lower limit must override DOWN button and force STOP";
    EXPECT_EQ(outputs.motor_speed, 0U) 
        << "Motor speed must be zero at limit";
    EXPECT_EQ(APP_GetState(), APP_STATE_IDLE) 
        << "Must remain in IDLE state at lower limit";
}

// ============================================================================
// TEST CASE: TC-SWReq-004-001 - Conflicting Buttons Force STOP
// ============================================================================
// Requirement ID: SWReq-004 (Conflicting inputs: ignore when both pressed)
// Requirement ID: SysReq-005 (Conflicting input handling)
//
// Test Objective:
//   SAFETY-CRITICAL: Verify that simultaneous UP and DOWN button presses
//   result in motor stop, preventing undefined behavior.
//
// Preconditions:
//   - Application in IDLE state
//   - Both UP and DOWN buttons are pressed simultaneously
//   - No limit sensors active
//
// Test Steps:
//   1. Create AppInput_t with button_up=true, button_down=true
//   2. Call APP_Task()
//   3. Verify motor is commanded to STOP
//
// Expected Results:
//   - Motor command: MOTOR_STOP
//   - Motor speed: 0
//   - State: IDLE (no state transition)
//
// Rationale:
//   - SAFETY-CRITICAL for handling unexpected user input combinations
//   - Prevents mechanical stress from conflicting motor commands
//   - Ensures deterministic behavior (fail-safe to STOP)
//   - Meets SysReq-005 requirement for input error handling
// ============================================================================
TEST_F(DeskAppComponentTest, TC_SWReq004_001_ConflictingButtonsForceStop)
{
    AppInput_t inputs = {0};
    inputs.button_up = true;      // CONFLICTING
    inputs.button_down = true;    // CONFLICTING
    inputs.limit_upper = false;
    inputs.limit_lower = false;
    inputs.timestamp_ms = 0U;
    
    AppOutput_t outputs;
    APP_Task(&inputs, &outputs);
    
    // SAFETY-CRITICAL: Simultaneous button press is a fault condition
    EXPECT_EQ(outputs.motor_cmd, MOTOR_STOP) 
        << "SAFETY: Conflicting button inputs must force STOP";
    EXPECT_EQ(outputs.motor_speed, 0U) 
        << "Motor speed must be zero with conflicting inputs";
    EXPECT_TRUE(outputs.fault_out)
        << "SAFETY: Simultaneous button press must trigger fault";
    EXPECT_EQ(APP_GetState(), APP_STATE_FAULT) 
        << "Must transition to FAULT state on conflicting button inputs";
    EXPECT_EQ(outputs.led_error, LED_ON)
        << "Error LED must be ON to indicate fault condition";
}

// ============================================================================
// TEST CASE: TC-SWReq-004-002 - Simultaneous Button Press During Motion
// ============================================================================
// Requirement ID: SWReq-004 (Conflicting inputs trigger fault)
// Requirement ID: SysReq-010 (Fault detection and response)
//
// Test Objective:
//   Verify that simultaneous button press while in MOVING state triggers
//   fault condition for safety.
//
// Preconditions:
//   - Application initially in IDLE state
//
// Test Steps:
//   1. Transition to MOVING_UP by pressing UP button
//   2. While moving, press both buttons simultaneously
//   3. Verify system enters FAULT state with motor stopped
//
// Expected Results:
//   - Motor command = MOTOR_STOP
//   - Motor speed = 0
//   - State transitions to APP_STATE_FAULT
//   - Fault output flag is set
//   - Error LED is illuminated

TEST_F(DeskAppComponentTest, TC_SWReq004_002_ConflictingButtonsDuringMotion)
{
    // Step 1: Transition to MOVING_UP
    AppInput_t inputs = {0};
    inputs.button_up = true;
    inputs.limit_upper = false;
    inputs.limit_lower = false;
    inputs.timestamp_ms = 0U;
    
    AppOutput_t outputs;
    APP_Task(&inputs, &outputs);
    
    // Verify in MOVING_UP state
    EXPECT_EQ(APP_GetState(), APP_STATE_MOVING_UP)
        << "Setup: Should be in MOVING_UP state";
    
    // Step 2: Simultaneously press both buttons while moving
    inputs.button_up = true;
    inputs.button_down = true;  // Now press DOWN while UP already moving
    inputs.timestamp_ms = 10U;
    
    APP_Task(&inputs, &outputs);
    
    // Step 3: Verify fault state
    EXPECT_EQ(outputs.motor_cmd, MOTOR_STOP) 
        << "SAFETY: Motor must stop when conflicting buttons pressed";
    EXPECT_EQ(outputs.motor_speed, 0U) 
        << "Motor speed must be zero during fault";
    EXPECT_TRUE(outputs.fault_out)
        << "SAFETY: Simultaneous button press during motion must trigger fault";
    EXPECT_EQ(APP_GetState(), APP_STATE_FAULT) 
        << "Must transition to FAULT state immediately";
    EXPECT_EQ(outputs.led_error, LED_ON)
        << "Error LED must be ON to indicate fault condition";
}

// ============================================================================
// TEST CASE: TC-SWReq-003-001 - Button Release Stops Motor
// ============================================================================
// Requirement ID: SWReq-003 (Button release → motor deactivation)
// Requirement ID: SysReq-003 (Motion halt requirement)
//
// Test Objective:
//   Verify that releasing button transitions motor to STOP and returns to
//   IDLE state. Tests the complete UP→MOVING_UP→STOP→IDLE sequence.
//
// Preconditions:
//   - Application initially in IDLE state
//
// Test Steps:
//   1. Create AppInput_t with button_up=true (simulate pressed)
//   2. Call APP_Task() → verify MOVING_UP state
//   3. Create AppInput_t with button_up=false (simulate released)
//   4. Call APP_Task() with new timestamp
//   5. Verify transition back to IDLE with STOP command
//
// Expected Results (Step 5):
//   - Motor command: MOTOR_STOP
//   - Motor speed: 0
//   - State: IDLE (transitioned from MOVING_UP)
//
// Rationale:
//   - Ensures responsive halt when user releases button
//   - Meets SysReq-003 motion halt timing requirement
//   - Critical for safe operation and user control
// ============================================================================
TEST_F(DeskAppComponentTest, TC_SWReq003_001_ButtonReleaseStopsMotor)
{
    // Step 1-2: Establish MOVING_UP state by pressing UP button
    AppInput_t inputs1 = {0};
    inputs1.button_up = true;
    inputs1.timestamp_ms = 0U;
    
    AppOutput_t outputs1;
    APP_Task(&inputs1, &outputs1);
    EXPECT_EQ(outputs1.motor_cmd, MOTOR_UP);
    EXPECT_EQ(APP_GetState(), APP_STATE_MOVING_UP) 
        << "Precondition: Should be in MOVING_UP state";
    
    // Step 3-4: Release button (button_up=false) with new timestamp
    AppInput_t inputs2 = {0};
    inputs2.button_up = false;
    inputs2.button_down = false;
    inputs2.timestamp_ms = 100U;  // Simulate time passing
    
    AppOutput_t outputs2;
    APP_Task(&inputs2, &outputs2);
    
    // Step 5: Verify transition back to IDLE with STOP
    EXPECT_EQ(outputs2.motor_cmd, MOTOR_STOP) 
        << "Button release must command STOP";
    EXPECT_EQ(outputs2.motor_speed, 0U) 
        << "Motor speed must be zero after button release";
    EXPECT_EQ(APP_GetState(), APP_STATE_IDLE) 
        << "Must return to IDLE state on button release";
}

// ============================================================================
// TEST CASE: TC-SWReq-010-001 - Fault Input Triggers Error State
// ============================================================================
// Requirement ID: SWReq-010 (State maintenance including FAULT state)
// Requirement ID: SysReq-008 (Reliability: fault detection and reporting)
//
// Test Objective:
//   Verify that external fault input (from motor controller or hardware)
//   immediately transitions to FAULT state, stops motor, and sets fault
//   output flag for upstream notification.
//
// Preconditions:
//   - Application in IDLE state
//   - fault_in flag set to true (indicating external fault condition)
//
// Test Steps:
//   1. Create AppInput_t with fault_in=true (external fault detected)
//   2. Call APP_Task()
//   3. Verify fault handling response
//
// Expected Results:
//   - Motor command: MOTOR_STOP
//   - Fault output: true (propagate fault upstream)
//   - LED status: LED_ERROR
//   - State: FAULT
//
// Rationale:
//   - Ensures rapid response to hardware faults
//   - Provides fault indication to external systems
//   - Prevents continued operation with failing hardware
//   - Supports SysReq-008 reliability requirement
// ============================================================================
TEST_F(DeskAppComponentTest, TC_SWReq010_001_FaultInputTriggersErrorState)
{
    AppInput_t inputs = {0};
    inputs.button_up = true;       // Button pressed, but...
    inputs.fault_in = true;        // ...external fault detected
    inputs.timestamp_ms = 0U;
    
    AppOutput_t outputs;
    APP_Task(&inputs, &outputs);
    
    // SAFETY: Fault must override any user input
    EXPECT_EQ(outputs.motor_cmd, MOTOR_STOP) 
        << "Fault must force motor STOP regardless of user input";
    EXPECT_TRUE(outputs.fault_out) 
        << "Fault output flag must be set to propagate fault";
    EXPECT_EQ(outputs.led_bt_up, LED_OFF) 
        << "Button LEDs must be OFF during fault";
    EXPECT_EQ(outputs.led_bt_down, LED_OFF) 
        << "Button LEDs must be OFF during fault";
    EXPECT_EQ(outputs.led_error, LED_ON) 
        << "Error LED must be ON to indicate fault state";
    EXPECT_EQ(APP_GetState(), APP_STATE_FAULT) 
        << "Must transition to FAULT state";
}

// ============================================================================
// TEST CASE: TC-SWReq-010-002 - Dual Limit Switch Fault Detection
// ============================================================================
// Requirement ID: SWReq-010 (Fault detection and response)
// Requirement ID: SysReq-007 (Limit switch protection)
//
// Test Objective:
//   Verify that simultaneous activation of both limit switches triggers
//   a fault condition. This detects hardware malfunction (e.g., sensor failure).
//
// Preconditions:
//   - Application initially in IDLE state
//
// Test Steps:
//   1. Attempt to move UP while both limit switches are active
//   2. Verify system detects hardware fault
//   3. Confirm motor stops and error LED activates
//
// Expected Results:
//   - Motor command = MOTOR_STOP
//   - Motor speed = 0
//   - Fault output = true
//   - State = APP_STATE_FAULT
//   - Error LED = ON

TEST_F(DeskAppComponentTest, TC_SWReq010_002_DualLimitSwitchFault)
{
    // Step 1: Trigger dual limit fault
    AppInput_t inputs = {0};
    inputs.button_up = true;          // User presses UP
    inputs.button_down = false;
    inputs.limit_upper = true;        // BOTH limits active simultaneously
    inputs.limit_lower = true;        // (hardware fault condition)
    inputs.timestamp_ms = 0U;
    
    AppOutput_t outputs;
    APP_Task(&inputs, &outputs);
    
    // Step 2: Verify fault state
    EXPECT_EQ(outputs.motor_cmd, MOTOR_STOP) 
        << "Motor must stop when both limits are triggered";
    EXPECT_EQ(outputs.motor_speed, 0U) 
        << "Motor speed must be zero";
    EXPECT_TRUE(outputs.fault_out)
        << "SAFETY: Both limit switches active must trigger fault";
    EXPECT_EQ(outputs.led_bt_up, LED_OFF) 
        << "Movement LEDs must be OFF during fault";
    EXPECT_EQ(outputs.led_bt_down, LED_OFF) 
        << "Movement LEDs must be OFF during fault";
    EXPECT_EQ(outputs.led_error, LED_ON) 
        << "Error LED must be ON to indicate hardware fault";
    EXPECT_EQ(APP_GetState(), APP_STATE_FAULT) 
        << "Must transition to FAULT state on dual limit activation";
    
    // Step 3: Clear dual limit fault (transient fault - auto-clears)
    inputs.limit_upper = false;      // Only one limit active now
    inputs.limit_lower = true;       // Fault condition resolved
    inputs.timestamp_ms = 10U;
    
    APP_Task(&inputs, &outputs);
    
    // Step 4: Verify automatic recovery from transient fault
    EXPECT_EQ(outputs.motor_cmd, MOTOR_STOP) 
        << "Motor remains stopped";
    EXPECT_FALSE(outputs.fault_out)
        << "Dual limit fault must auto-clear when condition resolves";
    EXPECT_EQ(outputs.led_error, LED_OFF) 
        << "Error LED must turn OFF when fault clears";
    EXPECT_EQ(APP_GetState(), APP_STATE_IDLE) 
        << "Must automatically recover to IDLE when dual limit fault clears";
}

// ============================================================================
// TEST CASE: TC-SWReq-010-003 - Fault Recovery When Buttons Released
// ============================================================================
// Requirement ID: SWReq-010 (Fault state recovery)
// Requirement ID: SysReq-008 (Reliability: fault recovery mechanism)
//
// Test Objective:
//   Verify that application recovers from FAULT state when both buttons
//   are released and no other fault conditions remain active.
//
// Preconditions:
//   - Application initially in IDLE state
//
// Test Steps:
//   1. Trigger fault condition (both buttons pressed)
//   2. Verify system in FAULT state
//   3. Release both buttons (button_up=false, button_down=false)
//   4. Call APP_Task() again
//   5. Verify recovery to IDLE state with all LEDs off
//
// Expected Results:
//   - Motor command = MOTOR_STOP
//   - Fault output = false (fault cleared)
//   - All LEDs OFF
//   - State = APP_STATE_IDLE (recovered)
//
// Rationale:
//   - Allows operator to recover from fault by releasing buttons
//   - Prevents system lockup in fault state
//   - Supports user-friendly fault recovery
// ============================================================================

TEST_F(DeskAppComponentTest, TC_SWReq010_003_FaultRecoveryWhenButtonsReleased)
{
    // Step 1: Trigger fault with both buttons pressed
    AppInput_t inputs = {0};
    inputs.button_up = true;
    inputs.button_down = true;    // Simultaneous press = fault
    inputs.limit_upper = false;
    inputs.limit_lower = false;
    inputs.timestamp_ms = 0U;
    
    AppOutput_t outputs;
    APP_Task(&inputs, &outputs);
    
    // Step 2: Verify in FAULT state
    EXPECT_EQ(APP_GetState(), APP_STATE_FAULT)
        << "Setup: Should be in FAULT state";
    EXPECT_TRUE(outputs.fault_out)
        << "Setup: Fault output should be active";
    
    // Step 3-4: Release both buttons
    inputs.button_up = false;
    inputs.button_down = false;   // Both released
    inputs.timestamp_ms = 10U;
    
    APP_Task(&inputs, &outputs);
    
    // Step 5: Verify recovery to IDLE
    EXPECT_EQ(outputs.motor_cmd, MOTOR_STOP) 
        << "Motor must remain stopped";
    EXPECT_EQ(outputs.motor_speed, 0U) 
        << "Motor speed must be zero";
    EXPECT_FALSE(outputs.fault_out)
        << "Fault output must be cleared";
    EXPECT_EQ(outputs.led_bt_up, LED_OFF) 
        << "UP button LED must be OFF";
    EXPECT_EQ(outputs.led_bt_down, LED_OFF) 
        << "DOWN button LED must be OFF";
    EXPECT_EQ(outputs.led_error, LED_OFF) 
        << "Error LED must be OFF after recovery";
    EXPECT_EQ(APP_GetState(), APP_STATE_IDLE) 
        << "Must recover to IDLE state when buttons released";
}

// ============================================================================
// TEST CASE: TC-SWReq-011-001 - Periodic Control Loop Execution
// ============================================================================
// Requirement ID: SWReq-011 (Non-blocking scheduler: APP_Task() at 250ms ±10ms)
// Requirement ID: SWReq-008 (Control loop cycle time ≤ 50ms)
//
// Test Objective:
//   Verify that multiple sequential APP_Task() calls with different
//   timestamps can track timing and respond appropriately. This tests
//   the application's ability to work with a periodic scheduler.
//
// Preconditions:
//   - Application in IDLE state
//
// Test Steps:
//   1. Call APP_Task() with timestamp_ms=0
//   2. Call APP_Task() with timestamp_ms=100 (100ms elapsed)
//   3. Call APP_Task() with timestamp_ms=200 (200ms elapsed)
//   4. Verify consistent behavior across all calls
//
// Expected Results:
//   - APP_Task() completes without error on each call
//   - Outputs remain consistent (IDLE state, MOTOR_STOP)
//   - No infinite loops or blocking calls
//
// Rationale:
//   - Verifies application doesn't require blocking delays
//   - Confirms compatibility with scheduler-based architecture
//   - Ensures deterministic execution in periodic mode
//   - Meets SysReq-009 scheduler integration requirement
// ============================================================================
TEST_F(DeskAppComponentTest, TC_SWReq011_001_PeriodicControlLoopExecution)
{
    AppOutput_t outputs;
    
    // First execution at t=0
    AppInput_t inputs1 = {0};
    inputs1.timestamp_ms = 0U;
    
    APP_Task(&inputs1, &outputs);
    EXPECT_EQ(outputs.motor_cmd, MOTOR_STOP);
    
    // Second execution at t=100ms (simulate scheduler period)
    AppInput_t inputs2 = {0};
    inputs2.timestamp_ms = 100U;
    
    APP_Task(&inputs2, &outputs);
    EXPECT_EQ(outputs.motor_cmd, MOTOR_STOP) 
        << "Output must remain consistent at second period";
    
    // Third execution at t=200ms
    AppInput_t inputs3 = {0};
    inputs3.timestamp_ms = 200U;
    
    APP_Task(&inputs3, &outputs);
    EXPECT_EQ(outputs.motor_cmd, MOTOR_STOP) 
        << "Output must remain consistent at third period";
    
    // All calls should complete without blocking
    // (Test framework timeout would catch infinite loops)
}

