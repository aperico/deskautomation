# Traceability Matrix

This matrix provides complete bi-directional traceability across all project artifacts for the Automated Mechanical Desk Lift system. It ensures alignment with ASPICE standards and enables impact analysis for changes.

**Traceability Chain:** Use Cases (UC-XX) → Requirements (SWE-REQ-XXX) → Architecture (ARCH-COMP-XXX, ARCH-IF-XXX, ARCH-STATE-XXX) → Test Cases (TC-XXX, IT-XXX)

---

## Navigation

- [System Use Cases](SystemUseCases.md)
- [Software Requirements](SoftwareRequirements.md)
- [Software Architecture](SoftwareArchitecture.md)
- [Software Test Cases Specification](SoftwareTestCasesSpecification.md)
- [Software Integration Tests Specification](SoftwareIntegrationTestsSpecification.md)

---

## Element ID Conventions

- **Use Cases:** UC-01 through UC-08
- **Requirements:** SWE-REQ-001 through SWE-REQ-020
- **Architecture Components:** ARCH-COMP-001 through ARCH-COMP-005
- **Architecture Interfaces:** ARCH-IF-001 through ARCH-IF-003
- **Architecture States:** ARCH-STATE-001 through ARCH-STATE-004
- **Architecture Transitions:** ARCH-TRANS-001 through ARCH-TRANS-007
- **Unit Test Cases:** TC-001 through TC-017
- **Integration Test Cases:** IT-001 through IT-010

Refer to [Software Requirements](SoftwareRequirements.md) for full requirement text and [Software Architecture](SoftwareArchitecture.md) for architectural element details.

---

## Use Cases → Software Requirements

| Use Case | Description | Software Requirement(s) |
|----------|-------------|--------------------------|
| UC-01 | Power the Desk Control System | SWE-REQ-001, SWE-REQ-002, SWE-REQ-009 |
| UC-02 | Raise Desk | SWE-REQ-003, SWE-REQ-005, SWE-REQ-007, SWE-REQ-012, SWE-REQ-017, SWE-REQ-018, SWE-REQ-020 |
| UC-03 | Lower Desk | SWE-REQ-004, SWE-REQ-006, SWE-REQ-008, SWE-REQ-012, SWE-REQ-017, SWE-REQ-018, SWE-REQ-020 |
| UC-04 | Emergency Stop | SWE-REQ-010, SWE-REQ-011, SWE-REQ-019, SWE-REQ-020 |
| UC-05 | Visual Feedback | SWE-REQ-012, SWE-REQ-013, SWE-REQ-020 |
| UC-06 | Power-Off During Movement | SWE-REQ-001, SWE-REQ-009, SWE-REQ-016, SWE-REQ-020 |
| UC-07 | Simultaneous Button Presses | SWE-REQ-010, SWE-REQ-014, SWE-REQ-017, SWE-REQ-020 |
| UC-08 | Error Indication and Recovery | SWE-REQ-011, SWE-REQ-015, SWE-REQ-016, SWE-REQ-018, SWE-REQ-020 |


---

## Software Requirements → Architecture → Test Cases

This table provides complete forward traceability from requirements through architectural components to test verification.

| Requirement | Category | Primary Component | Supporting Components | Unit Tests | Integration Tests |
|------------|----------|------------------|---------------------|-----------|------------------|
| SWE-REQ-001 | Functional | ARCH-COMP-004 | ARCH-COMP-002, ARCH-COMP-003 | TC-001 | IT-001, IT-006, IT-008 |
| SWE-REQ-002 | Functional | ARCH-COMP-001 | ARCH-COMP-002 | TC-002 | IT-001 |
| SWE-REQ-003 | Functional | ARCH-COMP-002 | ARCH-COMP-001 | TC-003 | IT-002 |
| SWE-REQ-004 | Functional | ARCH-COMP-002 | ARCH-COMP-001 | TC-005 | IT-003 |
| SWE-REQ-005 | Functional | ARCH-COMP-001 | ARCH-COMP-002, ARCH-COMP-005 | TC-003, TC-004 | IT-002 |
| SWE-REQ-006 | Functional | ARCH-COMP-001 | ARCH-COMP-002, ARCH-COMP-005 | TC-005, TC-006 | IT-003, IT-005 |
| SWE-REQ-007 | Functional | ARCH-COMP-001 | ARCH-COMP-002, ARCH-COMP-005 | TC-003, TC-004, TC-009 | IT-002 |
| SWE-REQ-008 | Functional | ARCH-COMP-001 | ARCH-COMP-002, ARCH-COMP-005 | TC-005, TC-006, TC-010 | IT-003, IT-005 |
| SWE-REQ-009 | Functional | ARCH-COMP-004 | ARCH-COMP-001 | TC-013 | IT-006, IT-008 |
| SWE-REQ-010 | Safety | ARCH-COMP-001 | ARCH-COMP-002 | TC-011, TC-012, TC-014 | IT-004, IT-007 |
| SWE-REQ-011 | Safety | ARCH-COMP-001 | ARCH-COMP-002, ARCH-COMP-005 | TC-011, TC-012, TC-015 | IT-004, IT-009 |
| SWE-REQ-012 | Functional | ARCH-COMP-001 | ARCH-COMP-002 | TC-007, TC-008 | IT-005, IT-010 |
| SWE-REQ-013 | Performance | ARCH-COMP-001 | ARCH-COMP-002 | TC-007, TC-008 | IT-005, IT-010 |
| SWE-REQ-014 | Safety | ARCH-COMP-001 | ARCH-COMP-002, ARCH-COMP-005 | TC-014, TC-015 | IT-007 |
| SWE-REQ-015 | Safety | ARCH-COMP-001 | ARCH-COMP-005 | TC-016, TC-017 | IT-008, IT-009 |
| SWE-REQ-016 | Safety | ARCH-COMP-001 | ARCH-COMP-004, ARCH-COMP-005 | TC-016, TC-017 | IT-006, IT-008 |
| SWE-REQ-017 | Functional | ARCH-COMP-002 | - | TC-003 to TC-006 | IT-002, IT-003, IT-006, IT-010 |
| SWE-REQ-018 | Safety | ARCH-COMP-001 | ARCH-COMP-005 | TC-009, TC-010 | IT-002, IT-003 |
| SWE-REQ-019 | Performance | ARCH-COMP-001 | ARCH-COMP-002, ARCH-COMP-005 | TC-011, TC-012, TC-015 | IT-004 |
| SWE-REQ-020 | Functional | ARCH-COMP-005 | ARCH-COMP-001 | All | All |

---

## Architecture Components → Integration Tests

This table shows which integration tests verify each architectural component and its interfaces.

| Component/Interface | Type | Verified By Integration Tests | Requirements Coverage |
|-------------------|------|----------------------------|----------------------|
| ARCH-COMP-001 | Component | IT-001 to IT-010 | SWE-REQ-002, 005-020 |
| ARCH-COMP-002 | Component | IT-001 to IT-010 | SWE-REQ-001, 003, 004, 012, 013, 017 |
| ARCH-COMP-003 | Component | IT-001 | SWE-REQ-001 |
| ARCH-COMP-004 | Component | IT-001, IT-006, IT-008 | SWE-REQ-001, 009, 016 |
| ARCH-COMP-005 | Component | IT-002 to IT-008 | SWE-REQ-005-008, 010, 011, 014-016, 018, 020 |
| ARCH-IF-001 | Interface | IT-001 to IT-010 | All functional requirements |
| ARCH-IF-002 | Interface | IT-001 to IT-010 | SWE-REQ-001, 003, 004, 012, 013, 017, 019 |
| ARCH-IF-003 | Interface | IT-001 | SWE-REQ-001 |
| ARCH-STATE-001 (IDLE) | State | IT-001, IT-002, IT-003, IT-006, IT-008 | SWE-REQ-001, 002, 009, 016 |
| ARCH-STATE-002 (MOVING_UP) | State | IT-002, IT-004 | SWE-REQ-005, 007 |
| ARCH-STATE-003 (MOVING_DOWN) | State | IT-003, IT-004 | SWE-REQ-006, 008 |
| ARCH-STATE-004 (ERROR) | State | IT-004, IT-007, IT-008, IT-009 | SWE-REQ-011, 014, 015, 016 |
| ARCH-TRANS-001 | Transition | IT-002 | SWE-REQ-005 |
| ARCH-TRANS-002 | Transition | IT-003 | SWE-REQ-006 |
| ARCH-TRANS-003 | Transition | IT-002 | SWE-REQ-007, 018 |
| ARCH-TRANS-004 | Transition | IT-003 | SWE-REQ-008, 018 |
| ARCH-TRANS-005 | Transition | IT-004 | SWE-REQ-010, 011, 019 |
| ARCH-TRANS-006 | Transition | IT-007 | SWE-REQ-014 |
| ARCH-TRANS-007 | Transition | IT-006, IT-008 | SWE-REQ-016 |

---

## Backward Traceability: Tests → Requirements → Use Cases

This section enables impact analysis when tests fail.

### Unit Test Coverage

| Unit Test | Verifies Requirements | Traces to Use Cases |
|-----------|---------------------|--------------------|
| TC-001 | SWE-REQ-001 | UC-01 |
| TC-002 | SWE-REQ-002 | UC-01 |
| TC-003 | SWE-REQ-003, 005, 017 | UC-02 |
| TC-004 | SWE-REQ-005 | UC-02 |
| TC-005 | SWE-REQ-004, 006 | UC-03 |
| TC-006 | SWE-REQ-006 | UC-03 |
| TC-007 | SWE-REQ-012 | UC-05 |
| TC-008 | SWE-REQ-012, 013 | UC-05 |
| TC-009 | SWE-REQ-007, 018 | UC-02 |
| TC-010 | SWE-REQ-008, 018 | UC-03 |
| TC-011 | SWE-REQ-010, 011, 019 | UC-04 |
| TC-012 | SWE-REQ-010, 011, 019 | UC-04 |
| TC-013 | SWE-REQ-009 | UC-01, UC-06 |
| TC-014 | SWE-REQ-010, 014 | UC-04, UC-07 |
| TC-015 | SWE-REQ-011, 014, 019 | UC-04, UC-07 |
| TC-016 | SWE-REQ-015, 016 | UC-08 |
| TC-017 | SWE-REQ-015, 016 | UC-08 |

### Integration Test Coverage

| Integration Test | Description | Verifies Requirements | Verifies Architecture | Traces to Use Cases |
|-----------------|-------------|---------------------|----------------------|--------------------|
| IT-001 | System Initialization | SWE-REQ-001, 002 | ARCH-COMP-001 to 004, ARCH-IF-001 to 003, ARCH-STATE-001 | UC-01 |
| IT-002 | Upward Movement | SWE-REQ-003, 005, 007, 017, 018 | ARCH-COMP-001, 002, 005; ARCH-IF-001, IF-002; ARCH-TRANS-001, 003 | UC-02 |
| IT-003 | Downward Movement | SWE-REQ-004, 006, 008, 017, 018 | ARCH-COMP-001, 002, 005; ARCH-IF-001, IF-002; ARCH-TRANS-002, 004 | UC-03 |
| IT-004 | Emergency Stop (Both Buttons) | SWE-REQ-010, 011, 019 | ARCH-COMP-001, 005; ARCH-TRANS-005, ARCH-STATE-004 | UC-04, UC-07 |
| IT-005 | Lower Limit Stop | SWE-REQ-006, 008, 012, 013 | ARCH-COMP-001, 002; ARCH-IF-002; All states | UC-03, UC-05 |
| IT-006 | Debounce Behavior | SWE-REQ-001, 009, 016, 017 | ARCH-COMP-004, 001; ARCH-IF-001; ARCH-TRANS-007 | UC-02, UC-03, UC-06 |
| IT-007 | Dwell Before Reversal | SWE-REQ-010, 014 | ARCH-COMP-001, 005; ARCH-IF-001, IF-002; ARCH-TRANS-006 | UC-04, UC-07 |
| IT-008 | Power-Off Recovery | SWE-REQ-001, 009, 015, 016 | ARCH-COMP-001, 004; ARCH-TRANS-007; ARCH-STATE-001, 004 | UC-06, UC-08 |
| IT-009 | Dual Limit Error | SWE-REQ-011, 015 | ARCH-COMP-001; ARCH-STATE-004 | UC-08 |
| IT-010 | LED Consistency | SWE-REQ-012, 013, 017 | ARCH-COMP-002; ARCH-IF-002; All states | UC-05 |

---

## Traceability Metrics

### Coverage Statistics

| Metric | Value | Target | Status |
|--------|-------|--------|--------|
| Use Cases with Requirements | 8/8 (100%) | 100% | ✓ Pass |
| Requirements with Architecture | 20/20 (100%) | 100% | ✓ Pass |
| Requirements with Unit Tests | 20/20 (100%) | 100% | ✓ Pass |
| Requirements with Integration Tests | 20/20 (100%) | 100% | ✓ Pass |
| Architecture Components with Tests | 5/5 (100%) | 100% | ✓ Pass |
| States with Integration Tests | 4/4 (100%) | 100% | ✓ Pass |
| Transitions with Integration Tests | 7/7 (100%) | 100% | ✓ Pass |

### Orphan Analysis

- **Orphan Requirements:** 0 (all requirements trace to use cases)
- **Orphan Tests:** 0 (all tests trace to requirements)
- **Orphan Architecture:** 0 (all components implement requirements)

### Gap Analysis

- **Requirements without Tests:** None
- **Architecture without Tests:** None
- **Use Cases without Requirements:** None

**Conclusion:** Complete bi-directional traceability achieved across all artifacts.

---

## Notes

- This traceability matrix is maintained manually and should be updated whenever:
  - New use cases, requirements, architecture elements, or tests are added
  - Existing elements are modified or removed
  - Test results reveal gaps in coverage
- For detailed test specifications, see:
  - [Software Test Cases Specification](SoftwareTestCasesSpecification.md) (unit tests)
  - [Software Integration Tests Specification](SoftwareIntegrationTestsSpecification.md) (integration tests)
- For architectural details, see [Software Architecture](SoftwareArchitecture.md)
- Traceability matrix compliance: ASPICE SWE.5 (Software Integration and Integration Test), SWE.6 (Software Qualification Test)

---

*For questions or suggestions, open an issue or contact the project maintainers.*

---
