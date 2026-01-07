# ASPICE Maturity Assessment

**Assessment date:** 2026-01-07
**Reviewer:** Automated repo scan (evidence required from project team)

**Overall conclusion:** **Level 2 (Managed) NOT demonstrated** — insufficient formal review and verification evidence. Many required base practices lack objective evidence; see detailed findings and required artifacts.

**Status legend (color indicators)**

- 🟩 F = Fully achieved (evidence present)
- 🟦 L = Largely achieved (most evidence present)
- 🟨 P = Partially achieved (some evidence; gaps remain)
- 🟥 N = Not achieved (no evidence)

---

## Summary

- Current artifacts (requirements, architecture, unit tests, integration tests, HAL mock, CI) show engineering activity.
- Critical ASPICE evidence missing: formal requirement/design/code/test reviews, test execution evidence mapped to requirement IDs, configuration baselines, and hardware integration evidence.
- Under full ASPICE criteria several process areas downgrade from L/F to P/N until evidence is provided.

---

## Evaluation Results

| Process Area                                            | Base Practice (summary)                                | Status | Evidence / Action                                                                                                                                    |
| ------------------------------------------------------- | ------------------------------------------------------ | -----: | ---------------------------------------------------------------------------------------------------------------------------------------------------- |
| SYS.1 Requirements Elicitation & Review                 | Stakeholder requirements documented; reviews required  |   🟥 N | Requirements exist (SoftwareRequirements.md). Create review records:`documentation/reviews/REQ_Review_YYYYMMDD.md`.                                |
| SYS.2 System Requirements Analysis & Allocation         | Requirement derivation & allocation                    |   🟨 P | Allocation documented (TraceabilityMatrix.md). Add allocation review & approval artifacts.                                                           |
| SYS.3 System Architectural Design & Review              | Architecture documented; review required               |   🟨 P | Architecture present (SoftwareArchitecture.md). Run architecture review and publish minutes/actions.                                                 |
| SYS.4 System Integration & Verification (System‑level) | Integration plan, HIL procedures, system test evidence |   🟥 N | Partial integration spec exists. Produce system integration procedures, run HIL, publish JUnit/XML and mapping (`documentation/test-evidence.md`). |
| SWE.1 Software Requirements Analysis                    | Requirements analysis and validation                   |   🟨 P | Requirements→design mapping exists; add validation review evidence and test mapping.                                                                |
| SWE.2 Software Architectural Design                     | Software architecture and allocation                   |   🟨 P | Architecture and allocations present; add design review records.                                                                                     |
| SWE.3 Detailed Design & Unit Construction               | Detailed design, implementation, unit verification     |   🟦 L | Unit tests and code present; add coverage reports and unit-test review evidence.                                                                     |
| SWE.4 Integration & Verification (Software)             | Host integration tests & CI; test artifacts per run    |   🟥 N | Integration tests exist; configure CI to store JUnit/XML per run and link tests to SR IDs.                                                           |
| Process & Configuration Management                      | Source control, baselines, change records              |   🟥 N | Repo present; add baseline records (tags/commit IDs) and review baselines.                                                                           |
| Traceability & Verification Evidence                    | Traceability matrix and verification review            |   🟥 N | TraceabilityMatrix.md exists; perform and document a traceability verification review and evidence file.                                             |
| Risk & Safety Evidence                                  | Safety requirements and verification (fault tests)     |   🟥 N | Safety SRs identified; create and execute safety verification tests and publish results tied to SR IDs.                                              |

---

## Maturity Summary (high level)

| Process Area                              | Re‑evaluated Status | Recommendation to reach 🟩 Fully                                                                                              |
| ----------------------------------------- | -------------------: | ----------------------------------------------------------------------------------------------------------------------------- |
| SYS.1 Requirements Elicitation & Review   |                 🟥 N | Run formal requirements reviews (minutes, attendees, actions), resolve items, publish signed review artifacts.                |
| SYS.2 System Requirements Analysis        |                 🟨 P | Produce allocation review records and approvals; update TraceabilityMatrix.md with reviewer sign‑off.                        |
| SYS.3 System Architectural Design         |                 🟨 P | Conduct architecture review, record decisions/action items, update architecture docs and baselines.                           |
| SYS.4 System Integration & Verification   |                 🟥 N | Create system integration plan, implement HIL procedures, execute system tests and store JUnit/XML evidence mapped to SR IDs. |
| SWE.1 Software Requirements Analysis      |                 🟨 P | Perform requirements validation review, link validation tests to SR IDs and publish review minutes.                           |
| SWE.2 Software Architectural Design       |                 🟨 P | Hold design reviews with recorded approvals; ensure design-to-requirement allocation is signed off.                           |
| SWE.3 Unit Implementation & Verification  |                 🟦 L | Add unit test coverage reports, record unit test reviews and CI artifacts; close any review action items.                     |
| SWE.4 Software Integration & Verification |                 🟥 N | Automate integration test artifact collection (CI), map tests to SR IDs, perform integration reviews and publish evidence.    |

---

## Required evidence to progress Level 2 → 3

1. Formal review artifacts for requirements, architecture, design and tests (minutes, attendees, actions).
2. Baseline identifiers (tags/commit IDs) used for the assessment.
3. Test execution artifacts (JUnit/XML) for unit, integration and system tests; each test linked to SR IDs.
4. Traceability review report confirming all SRs covered by tests and design.
5. Hardware integration (HIL) test procedures and results for safety‑critical SRs.
6. Code review evidence (PRs with approvals or review logs).

---

## Immediate action plan (minimal, high priority)

- Generate templates in `documentation/reviews/`: `REQ_Review`, `ARCH_Review`, `TEST_Review`, `TRACE_Checklist`.
- Configure CI to store test XML artifacts under `build/test-evidence/` and upload as release artifacts.
- Execute traceability verification and record the report.
- Re‑run assessment and update this document with links to evidence.

---

## Notes

- This re‑evaluation is conservative: code and test artifacts show development maturity, but ASPICE requires documented reviews and traceable verification evidence. Provide the listed artifacts to achieve "Largely" or "Fully" status for the flagged areas.
