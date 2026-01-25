# System Requirements

## Overview

This document defines functional system requirements derived from system objectives and operational scenarios. These requirements specify **what** the system shall do at the system level, serving as the basis for software and hardware requirements decomposition.

---

## Traceability

**Derived from:**
- [00_SystemMissionStatement.md](00_SystemMissionStatement.md)
- [01_SystemObjectives.md](01_SystemObjectives.md)
- [02_ConceptOfOperations.md](02_ConceptOfOperations.md)

---

## Requirements Specification

| REQ-ID | Statement | Rationale | Verification | Derived From |
|--------|-----------|-----------|--------------|--------------|
| **SysReq-001** | The system shall adjust desk height across the full range of 30 cm to 120 cm while supporting a load up to 20 kg. | Ensures ergonomic positioning for diverse user heights and typical office equipment loads. Full 90 cm stroke enables both sitting and standing work postures. | Physical measurement of height at minimum, maximum, and intermediate positions under 20 kg load; mechanical stress testing to confirm no binding or failure. | OBJ-001, Scenario 4 |
| **SysReq-002** | The system shall initiate desk motion within 1.0 second of UP or DOWN button press. | Provides responsive, intuitive user experience aligned with mission of "controls that respond instantly." Delay > 1 sec perceived as sluggish. | Timing measurement from button press electrical signal to motor activation signal; tested at minimum, mid-range, and maximum desk heights. | OBJ-002, Scenarios 1 & 2 |
| **SysReq-003** | The system shall halt all desk motion within 500 ms of button release. | Critical safety requirement ensuring user can immediately stop motion upon detecting hazards or reaching desired height. | Timing measurement from button release electrical signal to motor deactivation; measurement of coasting distance (shall be ≤ 1 cm). | OBJ-003, Scenario 3 |
| **SysReq-004** | The system shall complete a full 90 cm stroke (30 cm to 120 cm or reverse) within 30 seconds under 20 kg load. | Balances user convenience (reasonably fast adjustment) with motor/mechanical constraints and smooth operation. Excessive speed risks instability. | Performance measurement: full-stroke travel time under rated load; verify ≤ 30 sec for both upward and downward directions. | OBJ-004, Scenario 4 |
| **SysReq-005** | The system shall stop all motion when both UP and DOWN buttons are pressed simultaneously. | Safety requirement preventing undefined behavior or motor stress from conflicting commands. Ensures predictable, safe response to user error. | Functional test: simultaneously press both buttons and verify motor remains inactive; release one button and verify normal operation resumes. | OBJ-003, Scenario 5 |
| **SysReq-006** | The system shall maintain smooth, continuous motion without perceptible jerking or sudden speed changes during height adjustment. | Prevents load disturbance (monitor shake, spilled drinks) and enhances user comfort. Aligns with mission focus on health and comfort. | Vibration measurement with accelerometer on desktop surface during motion (acceptance: < 0.5 g peak acceleration); qualitative user observation for jerking. | OBJ-005, Scenarios 1 & 2 |
| **SysReq-007** | The system shall automatically stop motor activation at mechanical travel limits (30 cm and 120 cm) even if button remains held. | Protects mechanical system from over-travel damage and excessive motor stress. Ensures system longevity and prevents user-induced failures. | Functional test: hold UP button beyond upper limit and DOWN button beyond lower limit; verify automatic motor cutoff with no mechanical binding or damage. | OBJ-001, Scenario 4 |
| **SysReq-008** | The system shall operate reliably for a minimum of 10,000 full-stroke adjustment cycles without performance degradation. | Supports product lifetime expectation for daily office use (e.g., 5 adjustments/day × 5 years = ~9,000 cycles). Ensures reliability claim in mission. | Endurance testing: perform 10,000 automated full-stroke cycles under 20 kg load; verify response time, stroke time, and halt time remain within specifications. | OBJ-004 |

| **SysReq-009** | The system shall employ a non-blocking main loop with a time-based scheduler that executes application control logic at 4 Hz (every 250 ms). | Ensures predictable timing and responsiveness while avoiding blocking delays that can hinder safety-critical reactions and I/O handling. | Inspection of firmware architecture; timing measurement with instrumentation to verify application logic is invoked at 250 ms intervals over ≥ 60 s; confirm no use of blocking delays in the main loop. | OBJ-002, OBJ-003 |

---

## Requirements Traceability Matrix

### Upstream Traceability (Objectives → Requirements)

| Objective | Derived System Requirements |
|-----------|----------------------------|
| OBJ-001: Safe Height Adjustment Range | SysReq-001, SysReq-007 |
| OBJ-002: Responsive Control Initiation | SysReq-002 |
| OBJ-003: Immediate Motion Halt | SysReq-003, SysReq-005 |
| OBJ-004: Reliable Full-Stroke Operation | SysReq-004, SysReq-008 |
| OBJ-005: Smooth Motion Control | SysReq-006 |

### Scenario Coverage

| Scenario | Validated Requirements |
|----------|------------------------|
| Scenario 1: Normal Desk Raise | SysReq-001, SysReq-002, SysReq-004, SysReq-006 |
| Scenario 2: Normal Desk Lower | SysReq-001, SysReq-002, SysReq-004, SysReq-006 |
| Scenario 3: Motion Halt | SysReq-003 |
| Scenario 4: Full-Stroke Adjustment | SysReq-001, SysReq-004, SysReq-007 |
| Scenario 5: Simultaneous Button Press | SysReq-005 |

---

## Verification Summary

| Verification Method | Requirements Verified | Notes |
|---------------------|----------------------|-------|
| Physical Measurement | SysReq-001, SysReq-004 | Height, load, timing measurements |
| Timing Measurement | SysReq-002, SysReq-003, SysReq-004 | Electrical signal instrumentation |
| Functional Testing | SysReq-005, SysReq-007 | Button combination tests, limit tests |
| Vibration Analysis | SysReq-006 | Accelerometer on desktop surface |
| Endurance Testing | SysReq-008 | Automated cyclic testing |

---

## Requirements Characteristics

All system requirements comply with INCOSE quality criteria:

- ✅ **Necessary**: Each requirement addresses a distinct system capability
- ✅ **Unambiguous**: Uses "shall" with quantitative acceptance criteria
- ✅ **Verifiable**: Objective test methods specified for each requirement
- ✅ **Complete**: Covers all system objectives and operational scenarios
- ✅ **Consistent**: No conflicting requirements
- ✅ **Traceable**: Clear links to upstream objectives and downstream verification

---

**Document Version:** 1.1  
**Date:** January 25, 2026  
**Status:** Updated  
