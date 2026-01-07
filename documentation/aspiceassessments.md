# ASPICE Maturity Classification (Re-evaluated)

**Overall ASPICE Maturity Level:** Level 2 (Managed Process)

**Assessment Date:** January 7, 2026

Status legend (color indicators)
- 🟩 F = Fully achieved
- 🟦 L = Largely achieved
- 🟨 P = Partially achieved
- 🟥 N = Not achieved

Summary: After a repo-wide review (documentation, source, tests, CI), evidence has improved for requirements, architecture, unit verification and automated integration test orchestration. Remaining gaps are hardware-in-the-loop automation and fully automated traceability verification; therefore overall maturity remains Level 2 with stronger evidence in several process areas.

---

## System Engineering (SYS) Assessment

| Process Area | Base Practice | Status | Evidence / Notes |
|---|---|---:|---|
| SYS.1 Requirements Elicitation | Collect stakeholder requirements | 🟩 F | SoftwareRequirements.md present and linked. |
| SYS.1 Requirements Elicitation | Analyze stakeholder requirements | 🟩 F | Analysis and allocations documented. |
| SYS.2 System Requirements Analysis | Derive system requirements | 🟩 F | Requirement derivation and allocation in docs; TraceabilityMatrix.md updated. |
| SYS.2 System Requirements Analysis | Validate system requirements | 🟦 L | Unit + integration tests mapped to requirements; hardware validation partly manual. |
| SYS.3 System Architectural Design | Develop system architecture | 🟩 F | SoftwareArchitecture.md exists and maps to modules. |
| SYS.3 System Architectural Design | Allocate system requirements | 🟩 F | Allocation visible in TraceabilityMatrix.md and design docs. |
| SYS.4 System Integration & Verification | Plan system integration | 🟦 L | Integration test spec added; CI runs integration tests; formal system integration plan partial. |
| SYS.4 System Integration & Verification | Perform system integration | 🟦 L | Host integration tests with HALMock run in CI; hardware integration remains manual. |
| SYS.4 System Integration & Verification | Verify system | 🟦 L | Automated verification for software; hardware-level verification and evidence collection require process extension. |

---

## Software Engineering (SWE) Assessment

| Process Area | Base Practice | Status | Evidence / Notes |
|---|---|---:|---|
| SWE.1 Software Requirements Analysis | Analyze software requirements | 🟩 F | Requirements-to-design traceability present. |
| SWE.1 Software Requirements Analysis | Validate software requirements | 🟦 L | Automated tests map to requirements; some hardware validation manual. |
| SWE.2 Software Architectural Design | Develop software architecture | 🟩 F | Architecture documented; HAL/API contracts defined. |
| SWE.2 Software Architectural Design | Allocate software requirements | 🟩 F | Allocation present and traceable to tests. |
| SWE.3 Detailed Design & Unit Construction | Develop detailed design | 🟩 F | DetailedDesign and comments present. |
| SWE.3 Detailed Design & Unit Construction | Implement software units | 🟩 F | Source code, HAL and HALMock implemented; modularization improved. |
| SWE.3 Detailed Design & Unit Construction | Verify software units | 🟩 F | Unit tests exist and execute in CI; good coverage for controller logic. |
| SWE.4 Integration & Verification | Plan software integration | 🟦 L | Integration specs added; still opportunity to formalize sequencing and HIL. |
| SWE.4 Integration & Verification | Perform software integration | 🟦 L | Integration tests (application + HAL mock) added and wired into CI with CTest labels. |
| SWE.4 Integration & Verification | Verify software | 🟦 L | Integration verification automated for host; end-to-end hardware verification pending automation. |

---

## Process Maturity Overview

| Process Area | Maturity |
|---|---|
| SYS.1 Requirements Elicitation | 🟩 F |
| SYS.2 System Requirements Analysis | 🟩 F |
| SYS.3 System Architectural Design | 🟩 F |
| SYS.4 System Integration & Verification | 🟦 L |
| SWE.1 Software Requirements Analysis | 🟩 F |
| SWE.2 Software Architectural Design | 🟩 F |
| SWE.3 Detailed Design & Unit Construction | 🟩 F |
| SWE.4 Integration & Verification | 🟦 L |

---

## Key Evidence Summary (what was reviewed)
- Source: source/arduino/*.cpp / *.h (DeskController, HAL, HALMock, PinConfig)
- Tests: tests/SoftwareTests.cpp (unit), tests/IntegrationTests.cpp and tests/Integration_SmokeTests.cpp (integration)
- Build: CMakeLists.txt updated to build and link IntegrationTests; gtest_discover_tests used and CTest labels set
- CI: .github/workflows/cmake-single-platform.yml updated to run Unit and Integration labeled tests and collect artifacts
- Docs: TraceabilityMatrix.md, SoftwareTestCasesSpecification.md, SoftwareIntegrationTestsSpecification.md, SoftwareRequirements.md, SoftwareArchitecture.md, README.md, documentation/Glossary.md

---

## Remaining Gaps & Recommended Next Steps
- Hardware-in-the-loop automation: design and integrate HIL test harness and CI stage to exercise firmware on target hardware (required for full system verification).
- Traceability automation: adopt tooling (or scripts) to validate links between requirements, tests and results and auto-generate evidence reports.
- Formal integration plan: document stepwise integration and verification procedures for system-level (hardware + software) builds and reviewers.
- Evidence collection: standardize storing JUnit/XML artifacts per run and linking them to requirement IDs.

---

## Conclusion
Re-evaluation confirms strong evidence for requirements, architecture and software unit verification. Integration verification for software is automated on-host (HALMock) and runs in CI; system-level hardware verification and traceability automation remain the principal blockers to ascend to ASPICE Level 3.

*For further evidence or to run a targeted re-assessment after implementing HIL automation, rerun this review and update the assessment file.*
