# Mission Statement

**Document ID:** 01_MissionStatement  
**Version:** 1.1  
**Date:** January 26, 2026  
**Status:** Active / In Development  
**Compliance:** ISO 25119, ASPICE SYS.1

---

## Purpose

This document establishes the fundamental mission of the Automated Mechanical Desk Lift System using the To-By-While framework, providing clear direction for all subsequent system development activities. It serves as the baseline for traceability to system objectives, requirements, design, testing, and process compliance.

---

## Mission Statement (To-By-While Format)

### **TO:**
Replace manual crank operation of mechanical desks with a safe, reliable, and responsive motorized height adjustment system that improves user ergonomics and productivity through easy, intuitive controls.

### **BY:**
Implementing an Arduino UNO-based embedded control system that:
- Integrates a DC motor with L298N H-bridge motor driver
- Provides intuitive dual push-button (UP/DOWN) control interface
- Implements layered software architecture (HAL, Application, Main Loop) for modularity and testability
- Ensures safe operation through limit sensors, conflict detection, and hardware fail-safe mechanisms
- Follows ASPICE and ISO 25119 development processes
- Includes comprehensive testing at unit, component, and integration levels

### **WHILE:**
- Maintaining compatibility with existing mechanical desk structures and cranking shaft interfaces
- Ensuring user safety through multiple safety mechanisms and fail-safe design (limit sensors, emergency halt, conflict detection)
- Complying with safety-critical embedded system standards (ISO 25119, IEEE)
- Supporting extensibility for future enhancements (position presets, remote control, position feedback)
- Optimizing for resource-constrained microcontroller environments (Arduino UNO)
- Enabling comprehensive testing and validation through automated test suites (unit, component, integration)

---

## Mission Success Criteria

| ID | Criterion | Target | Verification Method |
|----|-----------|--------|---------------------|
| MSC-01 | Functional Operation | 100% reliable Up/Down movement (23/23 tests passing) | Automated Component/Integration Tests |
| MSC-02 | Safety Compliance | Zero unintended movements (verified by limit tests) | Component Tests (TC_SWReq005-006, TC_SWReq010) |
| MSC-03 | Response Time | UP/DOWN initiated ≤ 1.0 sec; motion halted ≤ 500 ms | Integration Tests with timing validation |
| MSC-04 | Conflict Detection | Simultaneous button press → motor stops | Component Test (TC_SWReq004) |
| MSC-05 | Development Quality | ASPICE Level 2 practices; 100% requirements traced | Traceability Matrix; Process Assessment |
| MSC-06 | Code Quality | Zero critical/error issues; style warnings noted | Static Analysis (cppcheck) |
| MSC-07 | Architecture Quality | Layered design (HAL, APP, Main); testable components | Component/Unit Tests (23 tests, 100% pass) |

---

## Stakeholders

| Stakeholder | Interest | Priority |
|-------------|----------|----------|
| End User | Safe, responsive desk height adjustment | Critical |
| Developer / Software Engineer | Clean architecture, testable code, maintainability | High |
| Safety Team | Compliance with safety standards (ISO 25119) | Critical |
| System Engineer | Clear traceability, complete requirements, quality metrics | High |
| QA / Verification Team | Comprehensive test coverage, defect detection | High |

---

## Key Constraints

1. **Hardware Constraint:** Arduino UNO microcontroller with limited SRAM (2 KB); compatible with L298N motor driver
2. **Safety Constraint:** Must prevent desk movement during fault conditions; limit sensors override button inputs
3. **Timing Constraint:** Control loop executes every 50 ms; button response ≤ 1 second
4. **Memory Constraint:** Optimize for 2 KB SRAM; avoid global variables beyond necessary state
5. **Standards Constraint:** ISO 25119, ASPICE Level 2 compliance required
6. **Testing Constraint:** All new code must have corresponding unit/component tests
