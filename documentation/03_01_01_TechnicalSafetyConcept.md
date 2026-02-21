# Technical Safety Concept (TeSaCo)

**Document ID:** TeSaCo-Desk-001  
**System Name:** Automated Mechanical Desk Lift System  
**Date:** February 21, 2026  
**Version:** 1.0  
**Status:** Draft  
**Prepared By:** System Engineering Team  
**Functional Safety Engineer:** Armando FuSaGuy  
**Compliance:** ISO 25119-2

---

## 1. Purpose and Scope

This Technical Safety Concept (TeSaCo) defines the safety-related architecture and mechanisms that implement the Functional Safety Requirements (FSR) derived from the HARA. It covers safety functions, safety states, and monitoring concepts at the system level.

---

## 2. Inputs and References

- [02_02_HARA-complete.md](02_02_HARA-complete.md)
- [03_00_SystemRequirements.md](03_00_SystemRequirements.md)
- [03_01_SystemArchitecture.md](03_01_SystemArchitecture.md)
- [11_HardwareDocumentation.md](11_HardwareDocumentation.md)

---

## 3. Safety Goals and FSRs in Scope

| Safety Goal | Functional Safety Requirements |
|-------------|--------------------------------|
| SG-001 Prevent unintended motion without valid command | FSR-001 |
| SG-002 Stop within 500 ms of release | FSR-002 |
| SG-003 Prevent motion beyond travel limits | FSR-004 |
| SG-004 Prevent motion on conflicting inputs | FSR-003 |
| SG-005 Prevent/stop motor runaway | FSR-006 |
| SG-006 Prevent unintended motion after reset | FSR-005 |

---

## 4. Safety Architecture Overview

### 4.1 Safety Functions

| Safety Function ID | Description | Implements |
|--------------------|-------------|------------|
| SF-001 | Command STOP when no valid input present | FSR-001 |
| SF-002 | Stop on button release within 500 ms | FSR-002 |
| SF-003 | Block motion on conflicting inputs | FSR-003 |
| SF-004 | Stop on limit switch active | FSR-004 |
| SF-005 | Safe STOP after reset/brownout | FSR-005 |
| SF-006 | Detect stuck-on/runaway and enter fault STOP | FSR-006 |

### 4.2 Safety States

- **SAFE_STOP:** Motor disabled, outputs de-energized, error indicator optional.
- **IDLE:** Awaiting valid UP/DOWN command (no motion).
- **FAULT_STOP:** Motor disabled; fault indicator active; motion blocked until reset/clear.

### 4.3 Safety Interfaces

| Interface | Type | Safety Use |
|-----------|------|------------|
| UP/DOWN buttons | Digital inputs | Command validity, release detection, conflict detection |
| Upper/Lower limit switches | Digital inputs | End-of-travel stop |
| Motor driver EN/PWM | Digital/PWM outputs | Commanded STOP and motion control |
| Motor current sense (driver SENSE pins) | Analog input | Stuck-on/runaway detection |
| Error LED | Digital output | Fault indication |

---

## 5. Safety Mechanisms

### 5.1 Input Validation and Command Gating

- Debounced UP/DOWN inputs.
- Only one valid command allowed at a time.
- No command -> motor STOP (SF-001).

### 5.2 Stop on Release

- Button release triggers immediate STOP command.
- Timing requirement: <= 500 ms (SF-002).

### 5.3 Limit Enforcement

- Active limit switch overrides motion command.
- Both directions checked each cycle (SF-004).

### 5.4 Safe Restart and Brownout Handling

- On reset/brownout, system initializes to SAFE_STOP.
- Motion enabled only after a valid command (SF-005).

### 5.5 Fault Detection for Stuck-On/Runaway

- Detect mismatch between commanded STOP and observed motor current.
- If commanded STOP and motor current exceeds threshold for > 100 ms, enter FAULT_STOP and assert error indicator (SF-006).
- Thresholds and conversion constants are defined in [src/safety_config.h](../src/safety_config.h).

---

## 6. Diagnostic Architecture (DIA)

### 6.1 Diagnostics Overview

Diagnostics are implemented in the control ECU and use input validation, limit switch monitoring, and motor current sensing to detect unsafe conditions and transition to FAULT_STOP.

| Diagnostic ID | Condition | Detection Method | Reaction |
|--------------|-----------|------------------|----------|
| DIA-001 | Conflicting input | Both buttons active in same cycle | Command STOP, stay IDLE |
| DIA-002 | Limit violation | Limit switch active while command in same direction | Command STOP, block motion |
| DIA-003 | Stuck-on/runaway | STOP commanded but current > threshold for > 100 ms | Enter FAULT_STOP, assert error LED |
| DIA-004 | Reset/brownout | Reset event detected | Initialize SAFE_STOP |

### 6.2 Diagnostic Timing

- Diagnostic checks execute every control loop cycle (<= 50 ms).
- Fault reaction time for DIA-003 is <= 100 ms detection + command STOP.

---

## 7. Fault Handling Strategy

| Fault Condition | Detection Method | System Response |
|----------------|------------------|-----------------|
| Conflicting inputs | Both buttons active | Command STOP, stay IDLE |
| Limit switch active | Input asserted | Command STOP, block further motion in that direction |
| Driver stuck-on/runaway | Output mismatch or unexpected motion | Enter FAULT_STOP, assert error LED |
| Reset/brownout | Power/reset event | Initialize SAFE_STOP, await command |

---

## 8. Safety Timing Assumptions

- Control loop period <= 50 ms (software requirement).
- Application task cadence 250 ms (system requirement) does not block safety checks.
- STOP command asserted within 500 ms of release or detected fault.

---

## 9. Verification Links

| Safety Function | Verification Artifact |
|----------------|-----------------------|
| SF-001 | SYS-TC-013, SWReq-012 tests |
| SF-002 | SYS-TC-005, SWReq-003 tests |
| SF-003 | SYS-TC-007, SWReq-004 tests |
| SF-004 | SYS-TC-009/010, SWReq-005/006 tests |
| SF-005 | SYS-TC-014, SWReq-013 tests |
| SF-006 | SYS-TC-015, SWReq-014 tests |

---

## 10. Decisions

- Stuck-on/runaway detection uses L298N SENSE pins with a shunt resistor and ECU ADC measurement. Threshold and timing are defined in detailed design.
- FAULT_STOP is cleared only by explicit reset (power cycle or MCU reset). No automatic restart is allowed.

---

## 11. Approvals

- **Functional Safety Engineer:** Armando FuSaGuy - [Pending]  
- **System Engineer:** [Pending]  
- **Project Manager:** [Pending]
