# Hazard Analysis and Risk Assessment (HARA)

**Document ID:** HARA-Desk-001  
**Version:** 0.1  
**Date:** February 21, 2026  
**Status:** Updated - AgPLr Confirmed  
**Functional Safety Engineer:** Armando FuSaGuy  
**Compliance:** ISO 25119-2 (Clause 5.3)

---

## 1. Purpose

This document captures the Hazard Analysis and Risk Assessment (HARA) for the Automated Mechanical Desk Lift System. HARA determines whether any hazardous situations require an AgPLr and establishes the safety basis for subsequent functional safety artifacts.

---

## 2. Scope and System of Interest

**System of Interest:** Automated Mechanical Desk Lift System (hardware + firmware + user controls).  
**Environment:** Indoor office use, supervised operation by a user at the desk.  
**Operating States:** Power on, idle, moving up, moving down, fault/stop.

**Out of Scope:** External building electrical safety, furniture structural integrity beyond defined load, and maintenance procedures.

---

## 3. HARA Status

- **HARA Workshop:** Completed
- **AgPLr Assignment:** Confirmed
- **Safety Goals:** Established

---

## 4. Assumptions and Constraints

- Users remain in proximity to the desk during motion.
- Users may also be not in proximity (bystanders possible).
- Motion is only commanded by continuous button press.
- Upper and lower travel limits are enforced by limit switches only.
- Maximum rated load is 20 kg.

These assumptions must be confirmed or revised during the HARA workshop.

---

## 5. Method

HARA will be performed using ISO 25119 severity (S), exposure (E), and controllability (C) classification. Hazardous events will be evaluated for AgPLr assignment.

**Scale:** ISO 25119 S0-S3, E1-E4, C1-C3.

### 5.1 AgPLr Mapping (Confirmed)

The following mapping is used for AgPLr assignment:

- Risk index $R = 2S + E + C$ (with S0-S3, E1-E4, C1-C3).
- $S0$ implies QM (no AgPLr).
- $R \le 6$ -> AgPLr a
- $R = 7$ or $8$ -> AgPLr b
- $R = 9$ or $10$ -> AgPLr c
- $R \ge 11$ -> AgPLr d

AgPLr values below are **confirmed** by the Functional Safety Engineer.

---

## 6. Candidate Hazard Identification (Pre-Workshop)

The table below lists candidate hazards to seed the HARA workshop. Values are **TBD** until the safety review.

| Hazard ID | Hazardous Event | Operational Situation | Potential Harm | S / E / C | AgPLr | Status / Notes |
|-----------|-----------------|-----------------------|----------------|-----------|-------|----------------|
| HZ-001 | Unintended upward motion | User near desk during idle | Pinch or crush injury | S1 / E1 / C3 | AgPLr a | Verify causes (stuck input, firmware fault) |
| HZ-002 | Unintended downward motion | User near desk during idle | Pinch or crush injury | S1 / E3 / C3 | AgPLr b | Verify causes (stuck input, firmware fault) |
| HZ-003 | Failure to stop on button release | Normal operation | User injury, object damage | S1 / E3 / C3 | AgPLr b | Relates to SysReq-003 |
| HZ-004 | Failure to stop at travel limits | End-of-stroke | Mechanical damage, user injury | S1 / E2 / C2 | AgPLr a | Relates to SysReq-007; limit switch failure |
| HZ-005 | Conflicting inputs cause unexpected motion | User presses both buttons | Pinch or crush injury | S1 / E2 / C2 | AgPLr a | Relates to SysReq-005 |
| HZ-006 | Electrical fault causes motor runaway | Powered state | Injury, equipment damage | S2 / E1 / C3 | AgPLr b | Requires electrical fault analysis |
| HZ-007 | Unintended motion while user not in proximity | User away from desk | Bystander injury, property damage | S1 / E1 / C2 | AgPLr a | Consider unattended motion scenarios |
| HZ-008 | Brownout reset leads to unintended motion on restart | Power fluctuation | User injury, equipment damage | S1 / E1 / C3 | AgPLr a | Add power integrity review |
| HZ-009 | Motor driver stuck-on output | Powered state | Injury, equipment damage | S2 / E1 / C3 | AgPLr b | Add driver fault detection concept |

---

## 7. Safety Goals

Safety Goals are derived from hazards with AgPLr assigned.

| Safety Goal ID | Safety Goal Statement | Related Hazards | AgPLr Target |
|---------------|-----------------------|----------------|--------------|
| SG-001 | Prevent any unintended motion when no valid user command is present. | HZ-001, HZ-002, HZ-007 | b |
| SG-002 | Ensure motion stops within 500 ms of button release. | HZ-003 | b |
| SG-003 | Prevent motion beyond upper and lower travel limits. | HZ-004 | a |
| SG-004 | Prevent motion on conflicting UP/DOWN inputs. | HZ-005 | a |
| SG-005 | Prevent or safely stop motor runaway due to electrical faults or stuck-on driver outputs. | HZ-006, HZ-009 | b |
| SG-006 | Prevent unintended motion following brownout or reset events. | HZ-008 | a |

---

## 8. Functional Safety Requirements

Functional Safety Requirements (FSR) are derived from Safety Goals.

| FSR ID | Requirement Statement | Safety Goal | AgPLr Target | Verification |
|--------|-----------------------|-------------|--------------|--------------|
| FSR-001 | The system shall command motor STOP when no valid UP/DOWN command is present. | SG-001 | b | Unit + system test: verify motor remains STOP with no input |
| FSR-002 | The system shall require continuous button press to maintain motion; release shall stop motion within 500 ms. | SG-002 | b | System test: measure stop time <= 500 ms |
| FSR-003 | The system shall prevent motion when both UP and DOWN buttons are pressed simultaneously. | SG-004 | a | Unit + system test: conflict input -> STOP |
| FSR-004 | The system shall stop motion when an upper or lower limit switch is active, regardless of user command. | SG-003 | a | System test: limit active -> STOP |
| FSR-005 | The system shall enter a safe STOP state after power reset or brownout before accepting motion commands. | SG-006 | a | Integration test: reset -> STOP state |
| FSR-006 | The system shall detect motor driver stuck-on or runaway behavior and command STOP with fault indication. | SG-005 | b | Integration test: injected fault -> STOP + fault LED |

---


## 10. Approvals

- **Functional Safety Engineer:** Armando FuSaGuy - [Pending]  
- **System Engineer:** [Pending]  
- **Project Manager:** [Pending]
