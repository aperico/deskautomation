# Requirements Traceability Matrix (RTM)

**Project Name:** Automated Mechanical Desk Lift System  
**Date:** February 21, 2026  
**Version:** 1.0  

---

## Purpose

This matrix ensures traceability from mission and objectives through system requirements and verification artifacts, including functional safety.

---

## 1. Mission -> Objectives

| Mission Element | Objective ID | Objective Summary |
|----------------|--------------|-------------------|
| Safe, reliable motorized adjustment | OBJ-001 | Safe height adjustment range |
| Responsive control | OBJ-002 | Responsive control initiation |
| Immediate halt on release | OBJ-003 | Immediate motion halt |
| Reliable operation | OBJ-004 | Reliable full-stroke operation |
| User comfort | OBJ-005 | Smooth motion control |

---

## 2. Objectives -> System Requirements

| Objective ID | System Requirements |
|--------------|---------------------|
| OBJ-001 | SysReq-001, SysReq-007 |
| OBJ-002 | SysReq-002, SysReq-009 |
| OBJ-003 | SysReq-003, SysReq-005, SysReq-009 |
| OBJ-004 | SysReq-004, SysReq-008 |
| OBJ-005 | SysReq-006 |

---

## 3. System Requirements -> System Tests

| System Requirement | System Test Cases | Verification Status |
|-------------------|-------------------|---------------------|
| SysReq-001 | SYS-TC-001, SYS-TC-002 | Planned |
| SysReq-002 | SYS-TC-003, SYS-TC-004 | Planned |
| SysReq-003 | SYS-TC-005 | Planned |
| SysReq-004 | SYS-TC-002, SYS-TC-006 | Planned |
| SysReq-005 | SYS-TC-007 | Planned |
| SysReq-006 | SYS-TC-008 | Planned |
| SysReq-007 | SYS-TC-009, SYS-TC-010 | Planned |
| SysReq-008 | SYS-TC-011 | Planned |
| SysReq-009 | SYS-TC-012 | Planned |

---

## 4. System Requirements -> Software Requirements

| System Requirement | Software Requirements |
|-------------------|-----------------------|
| SysReq-002 | SWReq-001, SWReq-002, SWReq-007, SWReq-008, SWReq-009 |
| SysReq-003 | SWReq-003, SWReq-007, SWReq-008 |
| SysReq-005 | SWReq-004 |
| SysReq-007 | SWReq-005, SWReq-006 |
| SysReq-008 | SWReq-010 |
| SysReq-009 | SWReq-011, SWReq-008 |

---

## 5. Functional Safety Traceability (Draft)

| HARA Hazard | Safety Goal | Safety Requirement | System Requirement | Test Case | Status |
|-------------|-------------|--------------------|--------------------|-----------|--------|
| HZ-001 | SG-001 | FSR-001 | SysReq-010 | SYS-TC-007 (no motion without valid command) | Draft |
| HZ-002 | SG-001 | FSR-001 | SysReq-010 | SYS-TC-007 (no motion without valid command) | Draft |
| HZ-003 | SG-002 | FSR-002 | SysReq-003 | SYS-TC-005 | Draft |
| HZ-004 | SG-003 | FSR-004 | SysReq-007 | SYS-TC-009, SYS-TC-010 | Draft |
| HZ-005 | SG-004 | FSR-003 | SysReq-005 | SYS-TC-007 | Draft |
| HZ-006 | SG-005 | FSR-006 | SysReq-012 | TBD (fault injection) | Draft |
| HZ-007 | SG-001 | FSR-001 | SysReq-010 | SYS-TC-007 (no motion without valid command) | Draft |
| HZ-008 | SG-006 | FSR-005 | SysReq-011 | TBD (reset/brownout test) | Draft |
| HZ-009 | SG-005 | FSR-006 | SysReq-012 | TBD (fault injection) | Draft |

---

## 6. References

- [01_MissionStatement.md](01_MissionStatement.md)
- [01_SystemObjectives.md](01_SystemObjectives.md)
- [02_ConceptOfOperations.md](02_ConceptOfOperations.md)
- [02_02_HARA-complete.md](02_02_HARA-complete.md)
- [03_00_SystemRequirements.md](03_00_SystemRequirements.md)
- [04_SoftwareRequirements.md](04_SoftwareRequirements.md)
- [10_SystemTestSpecification.md](10_SystemTestSpecification.md)
