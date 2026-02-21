# System Feature Concept Review

**Feature Name:** Stuck-On and Runaway Motor Detection  
**Date:** February 21, 2026  
**Version:** 1.0  
**Review Type:** Initial

## Feature Overview

### Business Context
- **Customer Need:** Electrical fault mitigation for motor control systems.
- **Business Value:** Detects stuck-on motor or runaway current draw; prevents damage and injury.
- **Priority:** Critical

### Feature Description
The system monitors motor current during operation and detects electrical anomalies such as stuck-on or runaway faults. When the motor is commanded STOP but current remains elevated, the system latches into FAULT state and stops the motor.

### Use Cases

#### Primary Use Case
**Actor:** System  
**Scenario:**
1. Motor is commanded STOP (H-bridge disabled).
2. System monitors motor current via analog sensor.
3. If current > 150 mA for > 100 ms, fault is detected.
4. System latches into FAULT state.
5. Motor stops; user must reset to recover.

**Expected Outcome:** Fault detected and contained; no runaway or stuck-on state persists.

#### Alternative Use Cases
- Normal operation with varying load current (< 150 mA when STOP).
- Brief current spike due to back-EMF during rapid deceleration (< 100 ms) does not trigger fault.

## Technical Approach

### Proposed Solution
Motor current is monitored via the L298N motor driver's SENSE pins connected to Arduino UNO analog input A0. A 0.5 Ω shunt resistor converts motor current to voltage for ADC measurement. DeskApp implements fault detection logic in the application state machine.

### Architecture Diagram
```mermaid
graph LR
    Motor[Motor] --> Shunt["Shunt (0.5Ω)"]
    Shunt --> SENSE["L298N SENSE"]
    SENSE --> ADC["Arduino A0"]
    ADC --> HAL["HAL_readMotorCurrent()"]
    HAL --> DeskApp["DeskApp Fault Logic"]
    DeskApp --> FAULT["FAULT State"]
    FAULT --> LED["LED/Output"]
```

### Key Design Decisions

1. **Decision:** Monitor current pin directly (not current-limiting resistor).
   - **Rationale:** Direct current measurement more sensitive to stuck-on condition.

2. **Decision:** 150 mA threshold for stuck-on detection.
   - **Rationale:** Balances normal operational noise (~100 mA) against motor stall current (~200+ mA).
   - **Configuration:** See src/safety_config.h.

3. **Decision:** 100 ms fault detection time.
   - **Rationale:** Filters transient spikes (back-EMF) while catching sustained faults quickly.
   - **Configuration:** See src/safety_config.h.

4. **Decision:** Fault state is latched (requires reset to recover).
   - **Rationale:** Safety-critical; prevents automatic recovery that could mask ongoing fault.

## Requirements Summary

### Functional Requirements
| Req ID | Description | Priority |
|--------|-------------|----------|
| SysReq-012 | Detect stuck-on and runaway motor faults | Critical |
| FSR-006 | Fault latch on elevated current when STOP commanded | Critical |
| SWReq-014 | Implement current-sense fault detection in DeskApp | Critical |

### Non-Functional Requirements
- **Performance:** Fault detection within 100 ms + application task cycle (250 ms max delay).
- **Reliability:** ADC baseline variation < ±5 mA when motor at STOP.
- **Safety:** Fault state is latched and unrecoverable without reset.

## Components & Integration

### Hardware
- **Sensor:** L298N SENSE pins (motor current feedback).
- **Connection:** Analog pin A0 on Arduino UNO with 0.5 Ω shunt.
- **ADC Reference:** 5V (AREF).

### Software
- **HAL API:** `HAL_readMotorCurrent()` → returns current in milliamps (0–10000 mA).
- **Conversion Formula:** `I_mA = (ADC_COUNT * ADC_REF_MV) / (1023 * SHUNT_MILLIOHMS)`
- **Constants:** Defined in src/safety_config.h (MOTOR_SENSE_THRESHOLD_MA, MOTOR_SENSE_FAULT_TIME_MS, ADC_REF_MV, SHUNT_MILLIOHMS).

## Impact Analysis

### Impact on Existing Systems
- **Motor Controller:** No change; H-bridge control unaffected.
- **DeskApp:** State machine now includes FAULT latching on current detection.
- **Main Loop:** Calls HAL_readMotorCurrent() and passes to DeskApp inputs.

### Dependencies
- **Hardware:** L298N SENSE pin routing; shunt resistor placement.
- **Firmware:** HAL abstraction; DeskApp state machine.
- **Testing:** Needs adjustable load to simulate fault conditions.

### Risks
| Risk | Impact | Probability | Mitigation |
|------|--------|-------------|------------|
| ADC noise causes false positives | High | Medium | 100 ms fault window + tuning; digital filtering if needed |
| Shunt resistor tolerances affect calibration | Medium | Low | Trim constant during calibration; document variance |
| High current spikes on start-up trigger false fault | Medium | Medium | Ensure STOP before fault check; use motor ramp controller |

## Verification Approach

### Test Strategy
- **Unit Testing:** ADC→mA conversion boundary tests (0 ADC → 0 mA, 1023 ADC → ~10000 mA).
- **Component Testing:** Fault latching with simulated current above/below threshold (TC-SWReq-014-001).
- **Integration Testing:** Motor driver with actual load; simulate current spike and verify FAULT state (SYS-TC-015).

### Acceptance Criteria
- [ ] Motor current accurately measured in milliamps (±5 mA baseline error).
- [ ] Fault latches when current > 150 mA for > 100 ms with STOP commanded.
- [ ] Fault does NOT trigger on transient spikes < 100 ms or current < 150 mA.
- [ ] System remains in FAULT state until reset.

## Safety and Security Considerations

### Safety Analysis
- HARA completed; hazard confirmed in [02_02_HARA-complete.md](../02_02_HARA-complete.md).
- Safety Goal: SG-005 (Detect and respond to electrical faults).
- Diagnostic Coverage: Motor current monitoring provides diagnostic for stuck-on hazard.
- **Thresholds (Safety-Critical):**
  - MOTOR_SENSE_THRESHOLD_MA = 150 mA (stuck-on detection)
  - MOTOR_SENSE_FAULT_TIME_MS = 100 ms (noise filtering)
  - See src/safety_config.h for all safety constants.

### Security Analysis
- Not applicable; no network or authentication involved.

## Open Questions and Decisions Needed

### Questions
1. **Q:** What is the expected motor current in normal operation?
   - **Owner:** Electrical Engineer
   - **Target Date:** [Pending]

2. **Q:** How should the user reset after a fault is detected?
   - **Owner:** Product Owner
   - **Target Date:** [Pending]

3. **Q:** Should fault logs be maintained for diagnostics?
   - **Owner:** System Engineer
   - **Target Date:** [Pending]

### Decisions Required
- [ ] **Decision:** Shunt resistor value and tolerance - **Owner:** Electrical Engineer - **Date:** [Pending]
- [ ] **Decision:** Fault threshold and timeout tuning - **Owner:** Electrical Engineer - **Date:** [Pending]
- [ ] **Decision:** Reset mechanism (button, power cycle, etc.) - **Owner:** Product Owner - **Date:** [Pending]

## Review Feedback

### Reviewer Comments
| Reviewer | Role | Comment | Status | Resolution |
|----------|------|---------|--------|------------|
| [Name] | [Role] | [TBD] | Open | |

### Action Items from Review
| Action | Owner | Due Date | Status |
|--------|-------|----------|--------|
| Verify shunt resistor tolerance and calibration | Electrical Engineer | [Pending] | Not Started |
| Simulate motor stall and verify fault detection | Test Engineer | [Pending] | Not Started |
| Measure baseline ADC noise in lab environment | Test Engineer | [Pending] | Not Started |

## Approval

### Review Outcome
- ☐ **Approved**
- ☐ **Approved with Conditions**
- ☐ **Not Approved**

### Sign-Off
- **System Engineer:** [Pending]  
- **Technical Lead:** [Pending]  
- **Product Owner:** [Pending]  
- **Safety Engineer (if applicable):** Armando FuSaGuy - [Pending]

## Next Steps
1. Verify shunt calibration and ADC baseline in lab environment.
2. Execute SYS-TC-015 with motor load simulation.
3. Confirm current-sense thresholds and fault timing in src/safety_config.h.
4. Confirm safety goal mappings (SG-005 → FSR-006 → SysReq-012) in the RTM.
