# Hazard Analysis and Risk Assessment (HARA)

**Document ID:** HARA-Desk-001  
**Version:** 0.1  
**Date:** February 21, 2026  
**Status:** Draft - Not Started  
**Prepared By:** System Engineering Team  
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

- **HARA Workshop:** Not started
- **AgPLr Assignment:** Pending
- **Safety Goals:** Not applicable until AgPLr determination

---

## 4. Assumptions and Constraints

- Users remain in proximity to the desk during motion.
- Motion is only commanded by continuous button press.
- Upper and lower travel limits are enforced by limit sensors and mechanical stops.
- Maximum rated load is 20 kg.

These assumptions must be confirmed or revised during the HARA workshop.

---

## 5. Method

HARA will be performed using ISO 25119 severity (S), exposure (E), and controllability (C) classification. Hazardous events will be evaluated for AgPLr assignment.

---

## 6. Candidate Hazard Identification (Pre-Workshop)

The table below lists candidate hazards to seed the HARA workshop. Values are **TBD** until the safety review.

| Hazard ID | Hazardous Event | Operational Situation | Potential Harm | S / E / C | AgPLr | Status / Notes |
|-----------|-----------------|-----------------------|----------------|-----------|-------|----------------|
| HZ-001 | Unintended upward motion | User near desk during idle | Pinch or crush injury | TBD | TBD | Verify causes (stuck input, firmware fault) |
| HZ-002 | Unintended downward motion | User near desk during idle | Pinch or crush injury | TBD | TBD | Verify causes (stuck input, firmware fault) |
| HZ-003 | Failure to stop on button release | Normal operation | User injury, object damage | TBD | TBD | Relates to SysReq-003 |
| HZ-004 | Failure to stop at travel limits | End-of-stroke | Mechanical damage, user injury | TBD | TBD | Relates to SysReq-007 |
| HZ-005 | Conflicting inputs cause unexpected motion | User presses both buttons | Pinch or crush injury | TBD | TBD | Relates to SysReq-005 |
| HZ-006 | Electrical fault causes motor runaway | Powered state | Injury, equipment damage | TBD | TBD | Requires electrical fault analysis |

---

## 7. Outputs and Next Steps

1. Schedule HARA workshop with Functional Safety Engineer.
2. Complete S/E/C classification and AgPLr assignment for each hazard.
3. If AgPLr is assigned, create Safety Goals and Functional Safety Requirements.
4. Update traceability: HARA -> Safety Goals -> System Requirements -> Tests.

---

## 8. Approvals

- **Functional Safety Engineer:** Armando FuSaGuy - [Pending]  
- **System Engineer:** [Pending]  
- **Project Manager:** [Pending]
