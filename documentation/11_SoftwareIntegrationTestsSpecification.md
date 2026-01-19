# Software Integration Test Specification (Application + HAL)

**Scope:** Integration-level tests exercising Application (DeskController) together with HAL (production HAL or HALMock).  
**Standards:** ISTQB-style test case format (ID, Title, Objective, Preconditions, Test Data, Steps, Expected Results, Traceability, Priority, Severity).

**Architecture Coverage:** These tests verify architectural components (ARCH-COMP-XXX), interfaces (ARCH-IF-XXX), states (ARCH-STATE-XXX), and state transitions (ARCH-TRANS-XXX) as defined in [Software Architecture](08_SoftwareArchitecture.md).

---

## Navigation

- [System Use Cases](05_SystemUseCases.md)
- [Software Requirements](07_SoftwareRequirements.md)
- [Software Architecture](08_SoftwareArchitecture.md)
- [Traceability Matrix](12_TraceabilityMatrix.md)

## Test Harness & Environment
- Host tests: g++/gtest with hal_mock (hal_mock/HALMock.*).
- Target tests (optional): Arduino hardware, deployed firmware, serial logging enabled.
- Common setup: HAL_Init(); DeskApp_task_init(&inputs,&outputs); ensure pins reset to known state; debounceDelay = 50 ms; default motor speed = 255.
- Execution note: Call HAL_Task() periodically in test loop to process blink timers.

---

## Architecture Verification Coverage

These integration tests verify the following architectural elements:

### Component Integration Points

1. **ARCH-COMP-001 ↔ ARCH-COMP-002 Integration**
   - **Verified By:** IT-002, IT-003, IT-004, IT-005, IT-007
   - **Tests:** Correct data flow via ARCH-IF-001 and ARCH-IF-002
   - **Properties:** Input/output struct integrity, HAL function calls

2. **ARCH-COMP-004 ↔ ARCH-COMP-001 Integration**
   - **Verified By:** IT-001, IT-006
   - **Tests:** Main loop orchestration, initialization sequence
   - **Properties:** Correct task invocation, timing compliance

3. **ARCH-COMP-001 ↔ ARCH-COMP-005 Integration**
   - **Verified By:** IT-002, IT-003, IT-004, IT-007, IT-008
   - **Tests:** State machine behavior
   - **Properties:** Valid state transitions, invariants maintained

### Interface Contract Verification

1. **ARCH-IF-001 (Task API) Verification**
   - **Test Method:** Pass known inputs, verify outputs
   - **Verified By:** IT-001 to IT-010
   - **Checks:** Input validation, output generation, timing

2. **ARCH-IF-002 (HAL API) Verification**
   - **Test Method:** Mock HAL, verify function call sequences
   - **Verified By:** IT-001 to IT-010
   - **Checks:** Correct HAL API usage, timing constraints

### State Machine Verification

1. **State Transitions (ARCH-TRANS-XXX)**
   - All 7 transitions verified
   - **Test Method:** Force specific inputs, verify state changes
   - **Coverage:**
     - ARCH-TRANS-001 (IDLE → MOVING_UP): IT-002
     - ARCH-TRANS-002 (IDLE → MOVING_DOWN): IT-003
     - ARCH-TRANS-003 (MOVING_UP → IDLE): IT-002
     - ARCH-TRANS-004 (MOVING_DOWN → IDLE): IT-003
     - ARCH-TRANS-005 (Any → ERROR): IT-004
     - ARCH-TRANS-006 (IDLE → ERROR): IT-007
     - ARCH-TRANS-007 (ERROR → IDLE): IT-006, IT-008

2. **State Invariants (ARCH-STATE-XXX)**
   - **ARCH-STATE-001 (IDLE):** Motor OFF, ready LED ON → IT-001, IT-002, IT-003
   - **ARCH-STATE-002 (MOVING_UP):** Motor UP, Up LED ON → IT-002
   - **ARCH-STATE-003 (MOVING_DOWN):** Motor DOWN, Down LED ON → IT-003
   - **ARCH-STATE-004 (ERROR):** Motor OFF, Error LED ON → IT-004, IT-007, IT-008, IT-009

### Timing Verification

1. **Response Time Requirements**
   - **SWE-REQ-013:** LED update < 50ms → IT-005
   - **SWE-REQ-019:** Emergency stop < 50ms → IT-004
   - **Test Method:** Timestamp input events and output responses

2. **Timeout Enforcement**
   - **SWE-REQ-018:** Movement timeout 30s → IT-002, IT-003
   - **Test Method:** Simulate 30s+ button press

---

## Integration Test Example for Testers

**Purpose:** Integration tests verify that multiple software components work correctly together as an integrated system, validating component interactions, interfaces, and complete workflows.

**What Integration Tests Verify:**
- Component interactions (DeskController ↔ HAL)
- Interface contracts (ARCH-IF-001: Task API, ARCH-IF-002: HAL API)
- Data flow between components
- State machine transitions across the system
- Complete end-to-end workflows
- Architectural requirements (ARCH-COMP-XXX, ARCH-STATE-XXX, ARCH-TRANS-XXX)

**Test Characteristics:**
- **Test Level:** Integration (System Level)
- **Scope:** Multiple components working together
- **Test Naming:** `Integration_IT###_DescriptiveName`
- **Framework:** Google Test (gtest)
- **Fixture:** Uses `IntegrationTestFixture` with SetUp/TearDown
- **Structure:** Multi-phase testing with clear state transitions

### Example Integration Test Case

Below is a complete example showing how to write an integration test that verifies upward movement workflow:

```cpp
/**
 * @test Integration_IT002_UpwardMovement_TransitionsAndControl
 * @req SWE-REQ-003, SWE-REQ-005, SWE-REQ-007, SWE-REQ-017, SWE-REQ-020
 * @usecase UC-02 (Raise Desk)
 * @architecture ARCH-COMP-001 (DeskController), ARCH-COMP-002 (HAL), ARCH-COMP-005 (State Machine)
 * @architecture ARCH-IF-001 (Task API), ARCH-IF-002 (HAL API)
 * @architecture ARCH-STATE-001 (IDLE), ARCH-STATE-002 (MOVING_UP)
 * @architecture ARCH-TRANS-001 (IDLE → MOVING_UP), ARCH-TRANS-003 (MOVING_UP → IDLE)
 * @priority High
 * @severity Major
 * 
 * Objective: Verify complete upward movement workflow integrating 
 *            DeskController and HAL components
 * 
 * Test Approach:
 * - Verify initial IDLE state
 * - Trigger state transition to MOVING_UP
 * - Verify sustained movement
 * - Trigger return to IDLE state
 * - Validate all state transitions and component interactions
 */
TEST_F(IntegrationTestFixture, Integration_IT002_UpwardMovement_TransitionsAndControl) {
    DeskAppTask_Return_t ret;
    
    // ========================================================================
    // PHASE 1: Verify Initial IDLE State (ARCH-STATE-001)
    // ========================================================================
    // Precondition: System initialized to safe IDLE state
    inputs.switch_state = SWITCH_STATE_OFF;
    
    ret = DeskApp_task(&inputs, &outputs);
    
    // Verify task execution successful
    EXPECT_EQ(ret, APP_TASK_SUCCESS) << "Task should succeed in IDLE state";
    
    // Verify IDLE state characteristics (motor disabled)
    VerifyMotorStopped();  // Helper: checks motor_enable=false, pwm=0
    
    // ========================================================================
    // PHASE 2: Trigger Upward Movement (ARCH-TRANS-001: IDLE → MOVING_UP)
    // ========================================================================
    // Action: User presses UP switch
    inputs.switch_state = SWITCH_STATE_UP;
    
    ret = DeskApp_task(&inputs, &outputs);
    
    // Verify task continues to succeed during movement
    EXPECT_EQ(ret, APP_TASK_SUCCESS) << "Task should succeed during upward movement";
    
    // Verify ARCH-STATE-002: MOVING_UP state characteristics
    VerifyMotorMovingUp();  // Helper: checks enable=true, direction=UP, pwm=255
    
    // Additional state-specific checks
    EXPECT_TRUE(outputs.motor_enable) << "Motor should be enabled for upward movement";
    EXPECT_FALSE(outputs.motor_direction) << "Direction should be UP (false)";
    EXPECT_EQ(outputs.motor_pwm, 255) << "PWM should be at full speed";
    
    // ========================================================================
    // PHASE 3: Verify Sustained Movement (Hold UP Switch)
    // ========================================================================
    // Action: Keep UP switch pressed (simulate sustained user input)
    // inputs.switch_state remains SWITCH_STATE_UP
    
    ret = DeskApp_task(&inputs, &outputs);
    
    // Verify system maintains MOVING_UP state
    EXPECT_EQ(ret, APP_TASK_SUCCESS) << "Task should continue succeeding";
    VerifyMotorMovingUp();  // Motor should still be moving up
    
    // ========================================================================
    // PHASE 4: Return to IDLE (ARCH-TRANS-003: MOVING_UP → IDLE)
    // ========================================================================
    // Action: User releases UP switch
    inputs.switch_state = SWITCH_STATE_OFF;
    
    ret = DeskApp_task(&inputs, &outputs);
    
    // Verify successful transition back to IDLE
    EXPECT_EQ(ret, APP_TASK_SUCCESS) << "Task should succeed when returning to IDLE";
    
    // Verify ARCH-STATE-001: IDLE state restored
    VerifyMotorStopped();  // Motor should be disabled again
    
    // Final verification: system ready for next command
    EXPECT_FALSE(outputs.motor_enable) << "Motor should be disabled in IDLE";
    EXPECT_EQ(outputs.motor_pwm, 0) << "PWM should be zero in IDLE";
}
```

### Test Structure Breakdown

**1. Test Documentation Header:**
```cpp
/**
 * @test Integration_IT002_...          // Unique test identifier
 * @req SWE-REQ-XXX                      // Requirements covered
 * @usecase UC-XX                        // Use case reference
 * @architecture ARCH-COMP-XXX           // Architecture elements verified
 * @priority High/Medium/Low             // Test priority
 * @severity Critical/Major/Minor        // Failure severity
 * 
 * Objective: Clear statement of what is being tested
 */
```

**2. Test Fixture:**
```cpp
TEST_F(IntegrationTestFixture, Integration_IT002_...) {
    // IntegrationTestFixture provides:
    // - DeskAppInputs_t inputs;
    // - DeskAppOutputs_t outputs;
    // - SetUp() for initialization
    // - Helper functions (VerifyMotorStopped, VerifyMotorMovingUp, etc.)
}
```

**3. Multi-Phase Structure:**
- **Phase 1:** Verify preconditions and initial state
- **Phase 2:** Trigger state change and verify transition
- **Phase 3:** Verify behavior during sustained state
- **Phase 4:** Trigger return transition and verify final state

**4. Assertions:**
```cpp
EXPECT_EQ(actual, expected) << "Descriptive failure message";
EXPECT_TRUE(condition) << "Explain what should be true";
EXPECT_FALSE(condition) << "Explain what should be false";
```

### Key Points for Writing Integration Tests

1. **Naming Convention:**
   - Prefix: `Integration_IT###_`
   - Format: `Integration_IT002_UpwardMovement_TransitionsAndControl`
   - Use descriptive names that explain the workflow being tested

2. **Comprehensive Documentation:**
   - Include all traceability tags (@test, @req, @usecase, @architecture)
   - Reference all architectural elements being verified
   - State objective clearly
   - Include priority and severity

3. **Multi-Phase Testing:**
   - Break complex workflows into logical phases
   - Use clear phase comments (PHASE 1, PHASE 2, etc.)
   - Each phase should have a clear purpose

4. **State Verification:**
   - Explicitly verify each state (ARCH-STATE-XXX)
   - Verify state transitions (ARCH-TRANS-XXX)
   - Check state invariants and characteristics

5. **Helper Functions:**
   - Use helper functions for common checks (VerifyMotorStopped, VerifyMotorMovingUp)
   - Keep test code readable and maintainable
   - Reduce duplication across tests

6. **Complete Workflows:**
   - Test entire end-to-end scenarios
   - Verify component interactions, not just individual components
   - Validate data flows correctly between components

7. **Architecture Focus:**
   - Verify interface contracts (ARCH-IF-001, ARCH-IF-002)
   - Test component integration (ARCH-COMP-001 ↔ ARCH-COMP-002)
   - Validate state machine behavior (ARCH-COMP-005)

8. **Descriptive Messages:**
   - Every assertion should have a clear failure message
   - Messages should explain what is expected and why
   - Makes debugging failed tests much easier

### Difference Between Test Levels

| Aspect | Integration Test | Component Test | Unit Test |
|--------|-----------------|----------------|-----------|
| **Scope** | Multiple components | Single component | Single function |
| **Focus** | Component interaction | Function logic | Basic behavior |
| **Prefix** | `Integration_IT###` | `Component_TC###` | `Unit_` |
| **Example** | DeskController + HAL | DeskController alone | Simple input→output |
| **Verifies** | Workflows, interfaces | Requirements, design | Minimal functionality |
| **Architecture** | ARCH-COMP, IF, STATE, TRANS | MODULE, FUNC | N/A |
| **Complexity** | High (multi-phase) | Medium (arrange-act-assert) | Low (simple checks) |

### Helper Functions Available in IntegrationTestFixture

```cpp
// Verify motor is completely stopped
void VerifyMotorStopped() {
    EXPECT_FALSE(outputs.motor_enable);
    EXPECT_EQ(outputs.motor_pwm, 0);
}

// Verify motor is moving upward
void VerifyMotorMovingUp() {
    EXPECT_TRUE(outputs.motor_enable);
    EXPECT_FALSE(outputs.motor_direction);  // UP = false
    EXPECT_EQ(outputs.motor_pwm, 255);
}

// Verify motor is moving downward
void VerifyMotorMovingDown() {
    EXPECT_TRUE(outputs.motor_enable);
    EXPECT_TRUE(outputs.motor_direction);   // DOWN = true
    EXPECT_EQ(outputs.motor_pwm, 255);
}
```

### Tips for Testers

1. **Start Simple:** Begin with the example above and modify for your test case
2. **Copy Pattern:** Use existing tests as templates for new tests
3. **Verify Incrementally:** Check each phase works before moving to next
4. **Clear Comments:** Explain what each phase does and why
5. **Run Frequently:** Execute tests often during development
6. **Use Helpers:** Leverage helper functions to keep tests readable
7. **Test Both Paths:** Verify both successful and failure scenarios
8. **Document Thoroughly:** Future testers (including yourself) will thank you

---

## Test Cases

### INT-01 — System Initialization
- **ID:** IT-001
- Objective: Verify system initializes correctly with HAL and DeskController in safe IDLE state.
- Preconditions: System powered off or uninitialized.
- Test Data: No buttons pressed, no limits active.
- Steps:
  1. Call HAL_init() to initialize hardware abstraction layer.
  2. Call DeskApp_task_init(&inputs, &outputs) to initialize application.
  3. Set inputs.btUPPressed=false, btDOWNPressed=false, upperLimitActive=false, lowerLimitActive=false.
  4. Call DeskApp_task(&inputs, &outputs).
  5. Apply outputs via HAL_ProcessAppState(APP_TASK_SUCCESS, &outputs); call HAL_Task().
- Expected Results:
  - DeskApp_task returns APP_TASK_SUCCESS.
  - Motor disabled: IN1=LOW, IN2=LOW, ENA=0.
  - All LEDs OFF: LED_LEFT_PIN=LOW, LED_RIGHT_PIN=LOW, ERROR_LED=LOW.
  - Outputs: moveUp=false, moveDown=false, error=false.
- **Architecture Verification:** ARCH-COMP-001 to 004; ARCH-IF-001 to 003; ARCH-STATE-001
- **Traceability:** UC-01, SWE-REQ-001, SWE-REQ-002
- Priority: High, Severity: Critical

### INT-02 — Up Button Commands Move Up
- **ID:** IT-002
- Objective: Verify pressing Up commands motor up and sets Up LED.
- Preconditions: upperLimitActive=false, lowerLimitActive=false, system initialized.
- Test Data: inputs.btUPPressed = true.
- Steps:
  1. Set inputs.btUPPressed=true; call DeskApp_task(&inputs,&outputs).
  2. Apply outputs via HAL_ProcessAppState(APP_TASK_SUCCESS,&outputs); call HAL_Task().
- Expected Results:
  - Motor direction pins: IN1=HIGH, IN2=LOW, ENA>0.
  - LED_LEFT_PIN = HIGH, LED_RIGHT_PIN = LOW, ERROR_LED = LOW.
- **Architecture Verification:** ARCH-COMP-001, ARCH-COMP-002; ARCH-IF-001, ARCH-IF-002; ARCH-STATE-001
- **Traceability:** UC-02, SWE-REQ-003, SWE-REQ-005
- Priority: High, Severity: Major

### INT-02 — Down Button Commands Move Down
- **ID:** IT-002
- Objective: Verify pressing Down commands motor down and sets Down LED.
- Preconditions: upperLimitActive=false, lowerLimitActive=false.
- Test Data: inputs.btDOWNPressed = true.
- Steps: same pattern as INT-01 with btDOWNPressed.
- Expected Results:
  - IN1=LOW, IN2=HIGH, ENA>0.
  - LED_RIGHT_PIN = HIGH, LED_LEFT_PIN = LOW, ERROR_LED = LOW.
- **Architecture Verification:** ARCH-COMP-001, ARCH-COMP-002; ARCH-IF-001, ARCH-IF-002; ARCH-TRANS-001, ARCH-TRANS-003
- **Traceability:** UC-02, SWE-REQ-003, SWE-REQ-005, SWE-REQ-007, SWE-REQ-017, SWE-REQ-018
- Priority: High, Severity: Major

### INT-03 — Simultaneous Buttons Prevent Movement (No Error)
- **ID:** IT-003
- Objective: Verify both buttons pressed stops movement and does not set fatal error.
- Preconditions: initialized.
- Test Data: inputs.btUPPressed=true, inputs.btDOWNPressed=true.
- Steps:
  1. Call DeskApp_task(&inputs,&outputs).
  2. Apply outputs via HAL_ProcessAppState(...); HAL_Task().
- Expected Results:
  - Motor stopped: IN1=LOW, IN2=LOW, ENA=0.
  - LEDs: LED_LEFT_PIN=LOW, LED_RIGHT_PIN=LOW, ERROR_LED=LOW.
  - DeskApp_task returns APP_TASK_SUCCESS.
- **Architecture Verification:** ARCH-COMP-001, ARCH-COMP-002, ARCH-COMP-005; ARCH-TRANS-002, ARCH-TRANS-004
- **Traceability:** UC-03, UC-07, UC-04, SWE-REQ-004, SWE-REQ-006, SWE-REQ-008, SWE-REQ-010
- Priority: High, Severity: Major

### INT-04 — Stop on Upper Limit During Move Up
- **ID:** IT-004
- Objective: Verify desk stops when upper limit becomes active during upward movement.
- Preconditions: Start with btUPPressed=true and upperLimitActive=false.
- Steps:
  1. Start move up (DeskApp_task / HAL_ProcessAppState).
  2. Set inputs.upperLimitActive=true while btUPPressed remains true; call DeskApp_task again; apply outputs.
- Expected Results:
  - After limit active: motor stopped (ENA=0), outputs.stop=true.
  - System enters DWELL then returns to IDLE after dwell timeout.
- **Architecture Verification:** ARCH-COMP-001, ARCH-COMP-005; ARCH-TRANS-005; ARCH-STATE-004
- **Traceability:** UC-02, UC-04, SWE-REQ-005, SWE-REQ-007, SWE-REQ-010, SWE-REQ-011, SWE-REQ-019
- Priority: High, Severity: Major

### INT-05 — Stop on Lower Limit During Move Down
- **ID:** IT-005
- Objective: Verify desk stops when lower limit becomes active during downward movement.
- Preconditions and steps analogous to INT-04 for down direction.
- Expected Results: Motor stopped, dwell behavior enforced.
- **Architecture Verification:** ARCH-COMP-001, ARCH-COMP-002; ARCH-IF-002; All states (LED verification)
- **Traceability:** UC-03, UC-05, SWE-REQ-006, SWE-REQ-008, SWE-REQ-012, SWE-REQ-013
- Priority: High, Severity: Major

### INT-06 — Debounce Behavior Prevents False Triggers
- **ID:** IT-006
- Objective: Verify short bounce on button input does not produce a movement command.
- Preconditions: HAL_debounceButton configured (debounceDelay = 50 ms).
- Test Data: Simulate raw readings: press->release->press with intervals < debounceDelay.
- Steps:
  1. Use HALMock to toggle pin states emulating bounce with timing < debounceDelay.
  2. Call HAL_debounceButton(...) each step and then DeskApp_task once stable.
- Expected Results:
  - No movement commanded until stable pressed state persists >= debounceDelay.
- **Architecture Verification:** ARCH-COMP-004, ARCH-COMP-001; ARCH-IF-001; ARCH-TRANS-007
- **Traceability:** UC-02, UC-03, UC-06, SWE-REQ-001, SWE-REQ-009, SWE-REQ-016, SWE-REQ-017
- Priority: Medium, Severity: Major

### INT-07 — Dwell Before Reversal
- **ID:** IT-007
- Objective: Verify system enforces dwell period after reaching a limit before allowing reverse direction.
- Preconditions: simulate reaching upper limit (APP_STATE_DWELL), dwell timeout = 300 ms.
- Steps:
  1. Reach upper limit -> system enters DWELL.
  2. While in dwell, press Down button -> call DeskApp_task.
  3. Wait dwell interval + small margin, call DeskApp_task again.
- Expected Results:
  - During dwell: movement commands inhibited (outputs.stop=true).
  - After dwell: Down movement allowed if button still pressed.
- **Architecture Verification:** ARCH-COMP-001, ARCH-COMP-005; ARCH-IF-001, ARCH-IF-002; ARCH-TRANS-006
- **Traceability:** UC-04, UC-07, SWE-REQ-010, SWE-REQ-014 (Dwell management)
- Priority: Medium, Severity: Major

### INT-08 — Power-Off During Movement (Resume Safe State)
- **ID:** IT-008
- Objective: Verify system stops motor on power loss and resumes to IDLE on reboot.
- Preconditions: Start moving up.
- Steps:
  1. Simulate power-off: call HAL_StopMotor(); reinitialize HAL_Init(); DeskApp_task_init().
  2. Verify motor pins inactive and outputs default to IDLE.
- Expected Results:
  - Motor stopped immediately on power-off.
  - On restart, system in IDLE, LEDs reflect ready; no movement without new input.
- **Architecture Verification:** ARCH-COMP-001, ARCH-COMP-004; ARCH-TRANS-007; ARCH-STATE-001, ARCH-STATE-004
- **Traceability:** UC-06, UC-08, SWE-REQ-001, SWE-REQ-009, SWE-REQ-015, SWE-REQ-016
- Priority: Medium, Severity: Major

### INT-09 — Application Error Indication & Recovery
- **ID:** IT-009
- Objective: Verify application sets error when both limits active and HAL reflects error LED; recovery requires safe conditions or power cycle.
- Preconditions: inputs.upperLimitActive=true and inputs.lowerLimitActive=true.
- Steps:
  1. Call DeskApp_task(&inputs,&outputs).
  2. Verify DeskApp_task returns APP_TASK_ERROR and outputs.error=true.
  3. Apply outputs to HAL_ProcessAppState(APP_TASK_ERROR,...).
- Expected Results:
  - Motor stopped, ERROR_LED asserted (blinking or steady per policy).
  - Recovery requires clearing invalid sensor state and/or reset: after clearing, DeskApp_task returns success.
- **Architecture Verification:** ARCH-COMP-001; ARCH-STATE-004 (ERROR state)
- **Traceability:** UC-08, SWE-REQ-011, SWE-REQ-015, SWE-REQ-016
- Priority: High, Severity: Critical

### INT-10 — LED Indicator Consistency
- **ID:** IT-010
- Objective: Verify LED states always reflect application outputs (moving/error/idle).
- Preconditions: Various inputs to generate IDLE, MOVING_UP, MOVING_DOWN, ERROR states.
- Steps:
  1. For each state, run DeskApp_task and HAL_ProcessAppState.
  2. Assert LED pin states as per I/O table.
- Expected Results:
  - LED mapping consistent with documentation (Up LED for MOVING_UP, Down LED for MOVING_DOWN, Error LED for ERROR).
- **Architecture Verification:** ARCH-COMP-002; ARCH-IF-002 (HAL LED functions)
- **Traceability:** UC-05, SWE-REQ-012, SWE-REQ-013, SWE-REQ-017
- Priority: Medium, Severity: Minor

---

## Test Data & Tools
- Use HALMock for deterministic pin/read/write behaviors.
- Use gtest for assertions and test lifecycle.
- Use parameterized tests where applicable (debounce timings, motor speed variations).

## Acceptance Criteria
- All high-priority tests (INT-01..INT-05, INT-09) must pass for release.
- Medium/Low priority tests provide additional verification and should pass in CI.
- All architectural elements (components, interfaces, states, transitions) must be verified by at least one integration test.

---

## Integration Test Summary Table

| Test ID | Name | Components | Interfaces | States/Transitions | Requirements | Use Cases | Priority |
|---------|------|------------|-----------|-------------------|-------------|-----------|----------|
| IT-001 | System Init | COMP-001 to 004 | IF-001 to 003 | STATE-001 | SWE-REQ-001, 002 | UC-01 | High |
| IT-002 | Upward Movement | COMP-001, 002, 005 | IF-001, IF-002 | TRANS-001, 003 | SWE-REQ-003, 005, 007, 017, 018 | UC-02 | High |
| IT-003 | Downward Movement | COMP-001, 002, 005 | IF-001, IF-002 | TRANS-002, 004 | SWE-REQ-004, 006, 008, 017, 018 | UC-03 | High |
| IT-004 | Emergency Stop | COMP-001, 005 | IF-001 | TRANS-005, STATE-004 | SWE-REQ-010, 011, 019 | UC-04 | High |
| IT-005 | LED Indicators | COMP-001, 002 | IF-002 | All states | SWE-REQ-012, 013 | UC-05 | High |
| IT-006 | Power Cycle | COMP-001, 004 | IF-001 | TRANS-007 | SWE-REQ-001, 009, 016 | UC-06 | Medium |
| IT-007 | Conflicting Inputs | COMP-001, 005 | IF-001, IF-002 | TRANS-006 | SWE-REQ-010, 014 | UC-07 | Medium |
| IT-008 | Error Recovery | COMP-001, 005 | IF-001 | STATE-004, TRANS-007 | SWE-REQ-011, 015, 016 | UC-08 | Medium |
| IT-009 | Dual Limit Error | COMP-001 | IF-001 | STATE-004 | SWE-REQ-011, 015 | UC-08 | High |
| IT-010 | LED Consistency | COMP-002 | IF-002 | All states | SWE-REQ-012, 013, 017 | UC-05 | Medium |

---

## Traceability Matrix (excerpt)

For complete traceability, see [Traceability Matrix](12_TraceabilityMatrix.md).

- IT-001 → UC-01 → SWE-REQ-001, SWE-REQ-002 → ARCH-COMP-001 to 004
- IT-002 → UC-02 → SWE-REQ-003, 005, 007, 017, 018 → ARCH-COMP-001, 002, 005; ARCH-TRANS-001, 003
- IT-003 → UC-03 → SWE-REQ-004, 006, 008, 017, 018 → ARCH-COMP-001, 002, 005; ARCH-TRANS-002, 004
- IT-004 → UC-04 → SWE-REQ-010, 011, 019 → ARCH-COMP-001, 005; ARCH-TRANS-005
- IT-005 → UC-05 → SWE-REQ-012, 013 → ARCH-COMP-001, 002; ARCH-IF-002
- IT-006 → UC-06 → SWE-REQ-001, 009, 016 → ARCH-COMP-004, 001; ARCH-TRANS-007
- IT-007 → UC-07 → SWE-REQ-010, 014 → ARCH-COMP-001, 005; ARCH-TRANS-006
- IT-008 → UC-08 → SWE-REQ-011, 015, 016 → ARCH-COMP-001, 005; ARCH-STATE-004
- IT-009 → UC-08 → SWE-REQ-011, 015 → ARCH-COMP-001; ARCH-STATE-004
- IT-010 → UC-05 → SWE-REQ-012, 013, 017 → ARCH-COMP-002; ARCH-IF-002

---

## Execution Notes (best practices)
- Keep tests deterministic: control timing via HALMock.millis() and avoid real delays in CI.
- Isolate each test: reset HAL and application state in test fixture SetUp/ TearDown.
- Fail-fast: assert preconditions early (e.g., DeskApp_task return codes).
- Maintain mapping to requirements and use cases in test metadata (comment header).
- Store test vectors and mock scenarios in /tests/resources for reuse.
