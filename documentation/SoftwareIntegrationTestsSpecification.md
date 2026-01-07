# Software Integration Test Specification (Application + HAL)

Scope: integration-level tests exercising Application (DeskController) together with HAL (production HAL or HALMock).  
Standards: ISTQB-style test case format (ID, Title, Objective, Preconditions, Test Data, Steps, Expected Results, Traceability, Priority, Severity).

## Test Harness & Environment
- Host tests: g++/gtest with hal_mock (hal_mock/HALMock.*).
- Target tests (optional): Arduino hardware, deployed firmware, serial logging enabled.
- Common setup: HAL_Init(); DeskApp_task_init(&inputs,&outputs); ensure pins reset to known state; debounceDelay = 50 ms; default motor speed = 255.
- Execution note: Call HAL_Task() periodically in test loop to process blink timers.

---

## Test Cases

### INT-01 — Up Button Commands Move Up
- Objective: Verify pressing Up commands motor up and sets Up LED.
- Preconditions: upperLimitActive=false, lowerLimitActive=false, system initialized.
- Test Data: inputs.btUPPressed = true.
- Steps:
  1. Set inputs.btUPPressed=true; call DeskApp_task(&inputs,&outputs).
  2. Apply outputs via HAL_ProcessAppState(APP_TASK_SUCCESS,&outputs); call HAL_Task().
- Expected Results:
  - Motor direction pins: IN1=HIGH, IN2=LOW, ENA>0.
  - LED_LEFT_PIN = HIGH, LED_RIGHT_PIN = LOW, ERROR_LED = LOW.
- Traceability: UC-02
- Priority: High, Severity: Major

### INT-02 — Down Button Commands Move Down
- Objective: Verify pressing Down commands motor down and sets Down LED.
- Preconditions: upperLimitActive=false, lowerLimitActive=false.
- Test Data: inputs.btDOWNPressed = true.
- Steps: same pattern as INT-01 with btDOWNPressed.
- Expected Results:
  - IN1=LOW, IN2=HIGH, ENA>0.
  - LED_RIGHT_PIN = HIGH, LED_LEFT_PIN = LOW, ERROR_LED = LOW.
- Traceability: UC-03
- Priority: High, Severity: Major

### INT-03 — Simultaneous Buttons Prevent Movement (No Error)
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
- Traceability: UC-07, UC-04
- Priority: High, Severity: Major

### INT-04 — Stop on Upper Limit During Move Up
- Objective: Verify desk stops when upper limit becomes active during upward movement.
- Preconditions: Start with btUPPressed=true and upperLimitActive=false.
- Steps:
  1. Start move up (DeskApp_task / HAL_ProcessAppState).
  2. Set inputs.upperLimitActive=true while btUPPressed remains true; call DeskApp_task again; apply outputs.
- Expected Results:
  - After limit active: motor stopped (ENA=0), outputs.stop=true.
  - System enters DWELL then returns to IDLE after dwell timeout.
- Traceability: UC-02
- Priority: High, Severity: Major

### INT-05 — Stop on Lower Limit During Move Down
- Objective: Verify desk stops when lower limit becomes active during downward movement.
- Preconditions and steps analogous to INT-04 for down direction.
- Expected Results: Motor stopped, dwell behavior enforced.
- Traceability: UC-03
- Priority: High, Severity: Major

### INT-06 — Debounce Behavior Prevents False Triggers
- Objective: Verify short bounce on button input does not produce a movement command.
- Preconditions: HAL_debounceButton configured (debounceDelay = 50 ms).
- Test Data: Simulate raw readings: press->release->press with intervals < debounceDelay.
- Steps:
  1. Use HALMock to toggle pin states emulating bounce with timing < debounceDelay.
  2. Call HAL_debounceButton(...) each step and then DeskApp_task once stable.
- Expected Results:
  - No movement commanded until stable pressed state persists >= debounceDelay.
- Traceability: UC-02, UC-03
- Priority: Medium, Severity: Major

### INT-07 — Dwell Before Reversal
- Objective: Verify system enforces dwell period after reaching a limit before allowing reverse direction.
- Preconditions: simulate reaching upper limit (APP_STATE_DWELL), dwell timeout = 300 ms.
- Steps:
  1. Reach upper limit -> system enters DWELL.
  2. While in dwell, press Down button -> call DeskApp_task.
  3. Wait dwell interval + small margin, call DeskApp_task again.
- Expected Results:
  - During dwell: movement commands inhibited (outputs.stop=true).
  - After dwell: Down movement allowed if button still pressed.
- Traceability: Functional Decomposition (Dwell management)
- Priority: Medium, Severity: Major

### INT-08 — Power-Off During Movement (Resume Safe State)
- Objective: Verify system stops motor on power loss and resumes to IDLE on reboot.
- Preconditions: Start moving up.
- Steps:
  1. Simulate power-off: call HAL_StopMotor(); reinitialize HAL_Init(); DeskApp_task_init().
  2. Verify motor pins inactive and outputs default to IDLE.
- Expected Results:
  - Motor stopped immediately on power-off.
  - On restart, system in IDLE, LEDs reflect ready; no movement without new input.
- Traceability: UC-06
- Priority: Medium, Severity: Major

### INT-09 — Application Error Indication & Recovery
- Objective: Verify application sets error when both limits active and HAL reflects error LED; recovery requires safe conditions or power cycle.
- Preconditions: inputs.upperLimitActive=true and inputs.lowerLimitActive=true.
- Steps:
  1. Call DeskApp_task(&inputs,&outputs).
  2. Verify DeskApp_task returns APP_TASK_ERROR and outputs.error=true.
  3. Apply outputs to HAL_ProcessAppState(APP_TASK_ERROR,...).
- Expected Results:
  - Motor stopped, ERROR_LED asserted (blinking or steady per policy).
  - Recovery requires clearing invalid sensor state and/or reset: after clearing, DeskApp_task returns success.
- Traceability: UC-08
- Priority: High, Severity: Critical

### INT-10 — LED Indicator Consistency
- Objective: Verify LED states always reflect application outputs (moving/error/idle).
- Preconditions: Various inputs to generate IDLE, MOVING_UP, MOVING_DOWN, ERROR states.
- Steps:
  1. For each state, run DeskApp_task and HAL_ProcessAppState.
  2. Assert LED pin states as per I/O table.
- Expected Results:
  - LED mapping consistent with documentation (Up LED for MOVING_UP, Down LED for MOVING_DOWN, Error LED for ERROR).
- Traceability: UC-05
- Priority: Medium, Severity: Minor

---

## Test Data & Tools
- Use HALMock for deterministic pin/read/write behaviors.
- Use gtest for assertions and test lifecycle.
- Use parameterized tests where applicable (debounce timings, motor speed variations).

## Acceptance Criteria
- All high-priority tests (INT-01..INT-05, INT-09) must pass for release.
- Medium/Low priority tests provide additional verification and should pass in CI.

## Traceability Matrix (excerpt)

- INT-01 → [UC-02: Raise Desk](SystemUseCases.md#uc-02-raise-desk) → [Architecture: Functional / Use Case View](SoftwareArchitecture.md#functional--use-case-view)
- INT-02 → [UC-03: Lower Desk](SystemUseCases.md#uc-03-lower-desk) → [Architecture: Functional / Use Case View](SoftwareArchitecture.md#functional--use-case-view)
- INT-03 → [UC-07: Simultaneous Button Presses](SystemUseCases.md#uc-07-simultaneous-button-presses), [UC-04: Emergency Stop](SystemUseCases.md#uc-04-emergency-stop) → [Architecture: Safety & Emergency Stop](SoftwareArchitecture.md#architectural-views)
- INT-04 → [UC-02: Raise Desk](SystemUseCases.md#uc-02-raise-desk) → [Architecture: Motion Control / Dwell Management](SoftwareArchitecture.md#functional--use-case-view)
- INT-05 → [UC-03: Lower Desk](SystemUseCases.md#uc-03-lower-desk) → [Architecture: Motion Control / Dwell Management](SoftwareArchitecture.md#functional--use-case-view)
- INT-06 → [UC-02: Raise Desk](SystemUseCases.md#uc-02-raise-desk), [UC-03: Lower Desk](SystemUseCases.md#uc-03-lower-desk) → [Architecture: Input acquisition + debouncing](SoftwareArchitecture.md#functional-logical-decomposition)
- INT-07 → (Dwell behavior) → [Architecture: Dwell management](SoftwareArchitecture.md#functional--use-case-view)
- INT-08 → [UC-06: Power-Off During Movement](SystemUseCases.md#uc-06-power-off-during-movement) → [Architecture: Power handling](SoftwareArchitecture.md#physical-view)
- INT-09 → [UC-08: Error Indication & Recovery](SystemUseCases.md#uc-08-error-indication-and-recovery) → [Architecture: Safety & Fault Handling](SoftwareArchitecture.md#safety-and-fault-handling)
- INT-10 → [UC-05: Visual Feedback](SystemUseCases.md#uc-05-visual-feedback) → [Architecture: Indicators and HMI](SoftwareArchitecture.md#functional--use-case-view)

---

## Execution Notes (best practices)
- Keep tests deterministic: control timing via HALMock.millis() and avoid real delays in CI.
- Isolate each test: reset HAL and application state in test fixture SetUp/ TearDown.
- Fail-fast: assert preconditions early (e.g., DeskApp_task return codes).
- Maintain mapping to requirements and use cases in test metadata (comment header).
- Store test vectors and mock scenarios in /tests/resources for reuse.
