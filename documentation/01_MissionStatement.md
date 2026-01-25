# Mission Statement

**Document ID:** 01_MissionStatement  
**Version:** 1.0  
**Date:** January 19, 2026  
**Status:** Draft  
**Author:** System Engineering Team  
**Compliance:** ISO 25119, ASPICE SYS.1

---

## Purpose

This document establishes the fundamental mission of the Automated Mechanical Desk Lift System using the To-By-While framework, providing clear direction for all subsequent system development activities.

---

## Mission Statement (To-By-While Format)

### **TO:**
Replace the manual crank operation of an existing IKEA mechanical desk with a safe, reliable, and user-friendly motorized height adjustment system.

### **BY:**
Implementing an Arduino-based embedded control system that:
- Integrates a high-torque DC worm gear motor (31ZY-5840) with L298N H-bridge driver
- Provides intuitive single rocker switch control interface
- Incorporates hardware abstraction layers for modularity and testability
- Ensures safe operation through emergency stop, current monitoring, and soft-start mechanisms
- Follows ASPICE and ISO 25119 development processes

### **WHILE:**
- Maintaining the existing desk mechanical structure and cranking shaft interface
- Ensuring user safety through multiple safety mechanisms and fail-safe design
- Complying with safety-critical embedded system standards (ISO 25119, IEEE)
- Enabling future extensibility (height presets, app control, position sensing)
- Keeping costs reasonable for a home/office automation project
- Supporting comprehensive testing at unit, integration, and system levels

---

## Mission Success Criteria

| ID | Criterion | Target | Verification Method |
|----|-----------|--------|---------------------|
| MSC-01 | Functional Operation | 100% reliable Up/Down movement | System Test |
| MSC-02 | Safety Compliance | Zero unintended movements | Safety Analysis + Test |
| MSC-03 | User Experience | Operation with single rocker switch | Usability Test |
| MSC-04 | Reliability | 10,000+ movement cycles without failure | Endurance Test |
| MSC-05 | Development Quality | ASPICE Level 2+ compliance | Process Assessment |
| MSC-06 | Traceability | 100% requirements traced to tests | Traceability Matrix |

---

## Stakeholders

| Stakeholder | Interest | Priority |
|-------------|----------|----------|
| End User | Safe, easy desk height adjustment | Critical |
| Developer | Modular, testable, maintainable code | High |
| Safety Team | Compliance with safety standards | Critical |
| System Engineer | Clear requirements and traceability | High |

---

## Key Constraints

1. **Hardware Constraint:** Must use existing IKEA desk mechanical cranking shaft
2. **Safety Constraint:** Must prevent desk movement during fault conditions
3. **Cost Constraint:** Target Arduino UNO-class hardware
4. **Standards Constraint:** ISO 25119, ASPICE compliance required
5. **Usability Constraint:** Operation must be simpler than manual cranking

---

## Document History

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| 1.0 | 2026-01-19 | System Engineering Team | Initial mission statement |

---

## References

- [System Objectives](02_SystemObjectives.md)
- [Concept of Operations](03_ConOps.md)
- [System Context Diagram](04_SystemContextDiagram.md)
- [System Use Cases](05_SystemUseCases.md)
- [Software Requirements](07_SoftwareRequirements.md)

---

**Approval Status:** Pending Review

| Role | Name | Signature | Date |
|------|------|-----------|------|
| System Engineer | TBD | | |
| Safety Representative | TBD | | |
| Project Lead | TBD | | |
