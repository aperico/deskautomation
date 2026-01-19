# System Requirements

**Document Version:** 1.0  
**Last Updated:** January 19, 2026  
**Status:** Draft  
**Author:** Development Team  
**Reviewers:** System Engineering, Safety Team  
**Compliance:** IEEE 29148-2018, ISO 26262, ISO 25119


## Document History

| Version | Date | Author | Status | Changes |
|---------|------|--------|--------|---------|
| 1.0 | Jan 19, 2026 | Dev Team | Draft | Initial v1.0 system requirements, removed v2.0 features |

---

## Overview

This document specifies the system-level requirements for the Automated Mechanical Desk Lift system. These requirements derive from the [System Use Cases](05_SystemUseCases.md) and define the functional, safety, and performance characteristics of the system. This version (1.0) focuses on the **v1.0 first draft design**, which implements core desk height control via a 3-position rocker switch with no visual feedback.

**Design Philosophy:** Minimal viable first draft focusing on reliable, safe desk height control using mechanical spring-return safety for the rocker switch input.

---

## Navigation

- [System Use Cases](05_SystemUseCases.md)
- [Software Requirements](07_SoftwareRequirements.md)
- [Software Architecture](08_SoftwareArchitecture.md)
- [Traceability Matrix](12_TraceabilityMatrix.md)

---

## System-Level Requirements Summary

| Req ID | Name | Category | Priority | Allocation | v1.0 Status |
|--------|------|----------|----------|-----------|------------|
| SYS-REQ-001 | Desk Height Control Range | Functional | Mandatory | Hardware/Software | Approved |
| SYS-REQ-002 | User Input Interface | Functional | Mandatory | Hardware | Approved |
| SYS-REQ-003 | Movement Safety Interlocks | Safety | Mandatory | Software | Approved |
| SYS-REQ-004 | Emergency Stop Mechanism | Safety | Mandatory | Hardware/Software | Approved |
| SYS-REQ-005 | Power Supply Requirements | Functional | Mandatory | Hardware | Approved |
| SYS-REQ-006 | Motor Control | Functional | Mandatory | Hardware/Software | Approved |
| SYS-REQ-007 | Fault Detection and Recovery | Safety | Mandatory | Software | Approved |
| SYS-REQ-008 | Operational State Management | Functional | Mandatory | Software | Approved |
| SYS-REQ-009 | Motor Current Monitoring | Safety | Mandatory | Hardware/Software | Approved |
| SYS-REQ-010 | Soft-Start Motor Control | Functional | Mandatory | Software | Approved |
| SYS-REQ-011 | User Feedback (v1.0) | Functional | High | Hardware | Future (v2.0+) |

---

## Detailed System Requirements

### SYS-REQ-001: Desk Height Control Range

**Category:** Functional  
**Priority:** Mandatory  
**Allocation:** Hardware / Software

**Description:**

The system shall support continuous desk height adjustment within a configurable range suitable for typical office desk applications (approximately 700–1200 mm height variation, subject to mechanical desk design).

**Specific Requirements:**
- System shall allow desk to move smoothly between configured lower and upper limits
- Movement shall be continuous and reversible (can raise or lower at any time)
- System shall detect when upper and lower limits are reached and stop automatically
- Limit detection shall occur within 100ms of physical limit contact

**Rationale:**
User comfort and ergonomic requirements for standing desk applications.

**Acceptance Criteria:**
- Desk raises and lowers smoothly within configurable range
- Automatic limit detection prevents mechanical over-travel
- User can manually verify final positions

**Dependencies:**
- Hardware: Desk mechanical structure, motor shaft coupling
- Software: SWE-REQ-005, SWE-REQ-006 (Movement Execution)

---

### SYS-REQ-002: User Input Interface

**Category:** Functional  
**Priority:** Mandatory  
**Allocation:** Hardware

**Description:**

The system shall provide a single 3-position rocker switch as the user input interface for desk height control. The rocker switch shall support three distinct positions: UP, OFF (center/neutral), and DOWN, with spring-return to the OFF position.

**Specific Requirements:**
- Rocker switch shall have three mechanical positions: UP, OFF, DOWN
- Switch shall automatically return to OFF position via spring mechanism when released
- Switch shall be rated for minimum 100,000 mechanical cycles
- Switch debounce time shall be 50ms (software-configurable)
- Switch inputs shall be read via digital GPIO pins on the ECU
- No LED visual feedback in v1.0; visual feedback is planned for v2.0+

**Rationale:**
Mechanical spring-return provides inherent safety (automatic stop on release). Single switch simplifies design and reduces complexity vs. two separate buttons. Eliminates accidental simultaneous command issues.

**Acceptance Criteria:**
- Rocker switch mechanical operation verified for 100,000 cycles without degradation
- Spring-return mechanism reliably returns to OFF position
- Software debouncing prevents spurious state transitions
- Conflicting simultaneous UP/DOWN inputs detected and handled safely

**Dependencies:**
- Hardware: Rocker switch component, spring mechanism
- Software: SWE-REQ-017 (Switch Debouncing), SWE-REQ-014 (Conflicting Input Handling)

---

### SYS-REQ-003: Movement Safety Interlocks

**Category:** Safety  
**Priority:** Mandatory  
**Allocation:** Software

**Description:**

The system shall implement software interlocks to prevent unsafe or conflicting movement commands, ensuring that desk movement is always under controlled conditions.

**Specific Requirements:**
- System shall prevent UP movement when desk is at upper limit (mechanical stop + electronic validation)
- System shall prevent DOWN movement when desk is at lower limit (mechanical stop + electronic validation)
- System shall immediately halt movement if conflicting inputs are detected (simultaneous UP and DOWN activation)
- System shall enforce a dwell period (minimum 10ms) between direction changes to allow mechanical settling
- All safety interlocks shall have < 50ms response time

**Rationale:**
Prevents mechanical damage, excessive stress on motor and drive train, and ensures user safety.

**Acceptance Criteria:**
- Upper and lower limit switches tested and working correctly
- Conflicting input detection tested; motor stops within 50ms
- Direction changes don't cause mechanical shock or damage
- No unintended movement due to noise or transient inputs

**Dependencies:**
- Hardware: Limit switches (if used), motor driver
- Software: SWE-REQ-003, SWE-REQ-004, SWE-REQ-014, SWE-REQ-018 (Timeout)

---

### SYS-REQ-004: Emergency Stop Mechanism

**Category:** Safety  
**Priority:** Mandatory  
**Allocation:** Hardware / Software

**Description:**

The system shall provide a reliable emergency stop mechanism that immediately halts desk movement in all circumstances. The primary emergency stop is spring-return to the OFF (center) position on the rocker switch; secondary emergency stops include timeout and fault detection.

**Specific Requirements:**
- **Primary:** Rocker switch spring-return to OFF position shall immediately de-energize motor within 50ms
- **Secondary:** System timeout (> 30s continuous movement) shall trigger automatic motor stop
- **Tertiary:** Fault detection (stall, overcurrent, invalid state) shall trigger automatic motor stop and ERROR state
- Emergency stop shall be non-maskable (cannot be overridden by software logic)
- Power cycle shall always reset system safely

**Rationale:**
Multiple layers of emergency protection ensure safety in fault conditions or user error.

**Acceptance Criteria:**
- Motor stops within 50ms when rocker switch returns to OFF
- Timeout triggering tested at 30-second threshold
- Fault detection tested and responding correctly
- Power cycle safely resets system to IDLE state

**Dependencies:**
- Hardware: Motor driver enable control, power supply control
- Software: SWE-REQ-010, SWE-REQ-011, SWE-REQ-018 (Timeout)

---

### SYS-REQ-005: Power Supply Requirements

**Category:** Functional  
**Priority:** Mandatory  
**Allocation:** Hardware

**Description:**

The system shall operate from an external DC power supply with specifications compatible with the 31ZY-5840 motor and BTS7960/IBT-2 driver.

**Specific Requirements:**
- Input voltage: 12V or 24V DC (user-selectable via jumper or configuration)
- Minimum current capacity: 5A at 12V or 2.5A at 24V (motor peak current)
- Power supply must include over-current protection
- External power supply should include a 12V to 5V buck converter (or equivalent) to power Arduino ECU
- Arduino ECU powered via 5V regulated supply (500mA minimum)
- Motor driver (BTS7960/IBT-2) powered directly from external supply (not through Arduino 5V)

**Rationale:**
Ensures adequate power for motor and control electronics without overloading Arduino power supply.

**Acceptance Criteria:**
- System operates reliably at min/max voltage specifications
- Current consumption measured and verified
- Thermal performance acceptable (no overheating)
- Power supply protection functions verified

**Dependencies:**
- Hardware: Power supply, buck converter, wiring

---

### SYS-REQ-006: Motor Control

**Category:** Functional  
**Priority:** Mandatory  
**Allocation:** Hardware / Software

**Description:**

The system shall provide controlled bidirectional motor operation via the BTS7960/IBT-2 H-bridge driver, with support for PWM speed control and direction reversal.

**Specific Requirements:**
- Motor direction control via R_EN and L_EN enable pins (must both be HIGH or both be LOW; never opposite states)
- Motor speed control via PWM (pulse width modulation) on IN1 and IN2 pins
- PWM frequency: 1kHz minimum (typical 5–10kHz)
- PWM duty cycle adjustable from 0–100%
- Motor shall have soft-start ramp (SWE-REQ-010) to reduce mechanical shock
- Motor shall be monitored for stall via current sense (SYS-REQ-009)

**Rationale:**
PWM control enables smooth speed ramping and soft-start. H-bridge allows bidirectional motion control.

**Acceptance Criteria:**
- Motor starts smoothly under load via soft-start ramp
- Direction reversal works reliably without mechanical shock
- PWM speed control adjusts motor speed smoothly
- Current sense accurately reports motor load

**Dependencies:**
- Hardware: BTS7960/IBT-2 motor driver, motor, PWM output pins
- Software: SWE-REQ-022 (Soft-Start), SWE-REQ-021 (Current Monitoring)

---

### SYS-REQ-007: Fault Detection and Recovery

**Category:** Safety  
**Priority:** Mandatory  
**Allocation:** Software

**Description:**

The system shall detect runtime faults and enter a safe ERROR state, with clear indication to the user and recovery via power cycle.

**Specific Requirements:**
- System shall detect movement timeout (> 30s without state change)
- System shall detect motor stall (via current sense thresholds)
- System shall detect conflicting inputs (simultaneous UP and DOWN)
- System shall detect invalid state transitions
- Upon fault detection, system shall:
  - Immediately transition to ERROR state
  - De-energize motor within 50ms
  - Log fault type (if diagnostic capability available)
  - Ignore all user inputs until power cycle
- Fault recovery requires power cycle (power OFF → power ON)

**Rationale:**
Ensures system reaches safe state in all fault conditions. Power cycle prevents false recovery and gives user time to assess situation.

**Acceptance Criteria:**
- All fault types detected correctly
- Motor stops within 50ms of fault detection
- User cannot accidentally resume operation without power cycle
- Fault indication is clear (lack of response to input)

**Dependencies:**
- Software: SWE-REQ-015, SWE-REQ-016 (Error Detection/Recovery)

---

### SYS-REQ-008: Operational State Management

**Category:** Functional  
**Priority:** Mandatory  
**Allocation:** Software

**Description:**

The system shall maintain discrete operational states and perform valid state transitions based on user input and system conditions.

**Specific Requirements:**
- System shall support four primary states:
  - **IDLE:** System ready, motor de-energized, waiting for user input
  - **MOVING_UP:** Motor energized in UP direction, desk ascending
  - **MOVING_DOWN:** Motor energized in DOWN direction, desk descending
  - **ERROR:** Fault detected, motor de-energized, awaiting power cycle
- State transitions shall be:
  - IDLE → MOVING_UP (when rocker UP position detected)
  - IDLE → MOVING_DOWN (when rocker DOWN position detected)
  - MOVING_UP → IDLE (when rocker returns to OFF or limit reached)
  - MOVING_DOWN → IDLE (when rocker returns to OFF or limit reached)
  - Any state → ERROR (on fault detection)
  - ERROR → IDLE (on power cycle, if no fault remains)
- State transition time shall be < 50ms

**Rationale:**
Clear state model ensures predictable, safe system behavior.

**Acceptance Criteria:**
- All valid state transitions tested
- Invalid transitions prevented
- State held stable during operation
- Transition timing verified

**Dependencies:**
- Software: Core control loop (HAL, state machine)

---

### SYS-REQ-009: Motor Current Monitoring

**Category:** Safety  
**Priority:** Mandatory  
**Allocation:** Hardware / Software

**Description:**

The system shall continuously monitor motor current via the BTS7960/IBT-2 current sense outputs (R_IS and L_IS) to detect stall, jam, or overload conditions.

**Specific Requirements:**
- Current sense inputs on analog pins A0 (R_IS) and A1 (L_IS)
- Sampling rate: minimum 100Hz during motor operation
- Stall threshold: shall be calibrated based on motor load (typical 400–600 ADC counts, subject to calibration)
- Overcurrent threshold: shall be set below motor driver hardware limit (subject to calibration)
- Upon stall or overcurrent detection: motor stops within 50ms, system enters ERROR state
- Fault logging shall record which condition triggered (stall vs. overcurrent)

**Rationale:**
Early stall detection prevents motor damage, excessive power draw, and thermal stress.

**Acceptance Criteria:**
- Current monitoring active during all movements
- Stall detection tested with mechanical jam simulation
- Overcurrent detection tested at calibrated threshold
- No false positives under normal operation

**Dependencies:**
- Hardware: R_IS/L_IS pins, analog-to-digital converter
- Software: SWE-REQ-021 (Current Monitoring), SWE-REQ-015 (Error Detection)

---

### SYS-REQ-010: Soft-Start Motor Control

**Category:** Functional  
**Priority:** Mandatory  
**Allocation:** Software

**Description:**

The system shall apply a PWM soft-start ramp when initiating motor movement to gradually increase power and reduce mechanical shock to the worm gear motor.

**Specific Requirements:**
- Soft-start ramp duration: 200–500ms (configurable)
- Initial PWM duty cycle: 20–30% (configurable)
- Final PWM duty cycle: 100% (or target speed)
- Ramp shall be linear or proportional over time
- If movement stops during ramp (button released, limit reached, error), PWM shall immediately set to 0
- Ramp shall be applied to both UP and DOWN movements

**Rationale:**
Reduces mechanical shock, increases motor and gearbox longevity, improves safety.

**Acceptance Criteria:**
- Motor accelerates smoothly under load
- No abrupt starts or mechanical shock observed
- Ramp timing verified with motor response
- Ramp configurable without code recompilation

**Dependencies:**
- Software: SWE-REQ-022 (PWM Soft-Start), motor control functions

---

### SYS-REQ-011: User Feedback (v1.0 vs. v2.0+)

**Category:** Functional  
**Priority:** High  
**Allocation:** Hardware (v2.0+)  
**v1.0 Status:** Future Feature

**Description (v1.0 - First Draft):**

In version 1.0, the system provides **no direct visual feedback** via LEDs. User feedback is implicit:
- User moves rocker switch UP → desk moves UP (immediate physical feedback)
- User moves rocker switch DOWN → desk moves DOWN (immediate physical feedback)
- User releases switch → desk stops (spring-return mechanism is tactile feedback)
- On fault: desk stops moving; user observes lack of response to rocker input

**Rationale (v1.0):**
Simplified first draft eliminates need for LED hardware and associated software. Mechanical feedback (desk motion, spring-return tactility) provides adequate user indication for core functionality. Vision feedback is planned for v2.0+.

**Description (v2.0+ - Future Enhancement):**

Version 2.0+ shall add LED-based visual feedback to enhance user awareness of system state:
- **Ready Indicator (Green LED):** System powered ON, in IDLE state, ready for commands
- **UP Indicator (Blue LED):** System in MOVING_UP state
- **DOWN Indicator (Yellow LED):** System in MOVING_DOWN state  
- **Error Indicator (Red LED):** System in ERROR state, awaiting recovery

**Future Acceptance Criteria (v2.0+):**
- LED indicators match system state within 50ms
- LED test pattern on power-up (all LEDs flash 500ms)
- Sufficient brightness for office lighting conditions
- Low power consumption (LEDs draw < 100mA total)

**Acceptance Criteria (v1.0):**
- ✅ System operates without LED hardware
- ✅ User can infer state from desk motion and rocker spring-return
- ✅ No functional loss vs. LED feedback

**Dependencies (v2.0+):**
- Hardware: LEDs, resistors, driver transistors (if required)
- Software: SWE-REQ-012, SWE-REQ-013 (LED Control)

---

## Hardware Allocation Summary

| Component | Function | Specification |
|-----------|----------|---------------|
| Arduino UNO | ECU (Engine Control Unit) | ATmega328P, 5V, 32KB Flash |
| BTS7960/IBT-2 | Motor H-bridge driver | 5–27V input, 43A peak, current sense outputs |
| 31ZY-5840 | Worm gear motor | 12/24V DC, 10 RPM, torque-optimized |
| Rocker Switch | User input interface | 3-position, spring-return to OFF, 100K+ cycles |
| Power Supply (external) | Primary power | 12V or 24V DC, 5A+ capacity, regulated |
| Buck Converter (5V) | ECU power | 12/24V → 5V, 500mA minimum |
| Limit Switches (if used) | Upper/lower bounds | Electronic detection of desk limits |
| Current Sense (R_IS/L_IS) | Motor monitoring | Built into BTS7960/IBT-2 driver |

---

## Software Allocation Summary

| Module | Function | Derived From |
|--------|----------|--------------|
| System Initialization | Power-on setup, self-test | SYS-REQ-001, SYS-REQ-005 |
| Input Acquisition | Rocker switch reading, debouncing | SYS-REQ-002, SWE-REQ-017 |
| State Machine | Core control logic, transitions | SYS-REQ-008, SWE-REQ-020 |
| Motor Control | Direction, PWM, soft-start | SYS-REQ-006, SYS-REQ-010, SWE-REQ-022 |
| Safety Interlocks | Conflict detection, limits, timeout | SYS-REQ-003, SYS-REQ-004, SWE-REQ-018 |
| Current Monitoring | Stall/overcurrent detection | SYS-REQ-009, SWE-REQ-021 |
| Error Handling | Fault detection, ERROR state | SYS-REQ-007, SWE-REQ-015, SWE-REQ-016 |

---

## Traceability

**System Requirements ↔ Use Cases:**
- SYS-REQ-001 ← UC-02, UC-03 (Raise/Lower Desk)
- SYS-REQ-002 ← UC-01, UC-02, UC-03 (Power-On, Raise, Lower)
- SYS-REQ-003 ← UC-04, UC-07, UC-08 (Emergency Stop, Conflicts, Error)
- SYS-REQ-004 ← UC-04 (Emergency Stop)
- SYS-REQ-005 ← UC-01 (Power-On)
- SYS-REQ-006 ← UC-02, UC-03, UC-10 (Raise, Lower, Soft-Start)
- SYS-REQ-007 ← UC-08 (Error Detection & Recovery)
- SYS-REQ-008 ← All use cases (State management across operations)
- SYS-REQ-009 ← UC-09 (Motor Current Monitoring)
- SYS-REQ-010 ← UC-10 (PWM Soft-Start)
- SYS-REQ-011 ← UC-05 (Visual Feedback - removed in v1.0, planned for v2.0+)

**System Requirements ↔ Software Requirements:**
- SYS-REQ-002 → SWE-REQ-003, SWE-REQ-004, SWE-REQ-017
- SYS-REQ-003 → SWE-REQ-010, SWE-REQ-014, SWE-REQ-018
- SYS-REQ-004 → SWE-REQ-010, SWE-REQ-011
- SYS-REQ-006 → SWE-REQ-005, SWE-REQ-006, SWE-REQ-022
- SYS-REQ-007 → SWE-REQ-015, SWE-REQ-016
- SYS-REQ-009 → SWE-REQ-021

---

## Design Phases

### Phase 1 (v1.0) - Current Design
- ✅ 3-position rocker switch with spring-return
- ✅ No LED visual feedback (implicit feedback via desk motion)
- ✅ Basic stall protection (current monitoring)
- ✅ Safety interlocks and emergency stop

### Phase 2 (v2.0) - Planned Enhancements
- ⏳ LED visual feedback (4 LEDs: Ready, UP, DOWN, Error)
- ⏳ Position memory (optional, requires limit switches or position encoder)
- ⏳ Mobile app control (Bluetooth module)
- ⏳ Preset height positions (stored in EEPROM)

### Phase 3 (v3.0+) - Future Enhancements
- ⏳ Collision detection (load cell feedback)
- ⏳ Voice command support
- ⏳ Integration with smart home systems

---

## Compliance

This system is designed to comply with:
- **IEEE 29148-2018:** Application and Management of Systems and Software Engineering
- **ISO 26262:** Functional Safety for Electrical/Electronic Systems
- **ISO 25119:** Safety of Machinery - Safety-Related Control Systems for Power-driven Desk Risers

Specific safety considerations for powered furniture and automated desk lifts are addressed in design documentation and test specifications.

---

## Appendix: Future Features (v2.0+)

### LED Visual Feedback System (Planned for v2.0)

When implemented, the system shall support four LED indicators:

| LED | Color | Meaning | Driven By |
|-----|-------|---------|-----------|
| READY | Green | System powered, IDLE state, ready for input | P2 (GPIO) |
| UP | Blue | System in MOVING_UP state | P3 (GPIO) |
| DOWN | Yellow | System in MOVING_DOWN state | P4 (GPIO) |
| ERROR | Red | System in ERROR state, awaiting recovery | P5 (GPIO) |

LED control shall be implemented via GPIO pins with appropriate current-limiting resistors (assuming Arduino GPIO max ~40mA per pin). If higher current is required, external transistor drivers shall be added.

**LED Test Mode (v2.0):**
On power-up, before entering IDLE state, all LEDs shall flash in sequence for 500ms as a functional self-test.

---

