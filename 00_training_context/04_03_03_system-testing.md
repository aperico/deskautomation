# Session 4.3.3: System Testing

**Focus:** System testing and test specification generation
**Time:** ~30 minutes

---

## System Testing

**Purpose:** Test system requirements at the integrated system level (user perspective).

**Approach:** 
- Manual inputs and observations on the target environment
- Traceable test cases derived from system requirements
- ISTQB-compliant documentation with explicit test steps

**Deliverable:** System Test Specification document with detailed test case steps, preconditions, and expected results.

---

## Part 1: Creating System Test Case Specification

**Prompt (Copy & Paste):**
```
TASK: Generate system test specification with explicit test steps.
Derive system-level test cases from the provided system requirements document.
Each test case must be traceable to one or more system requirements.

UPSTREAM DOCS:
- System Requirements (provide link or paste content)

TEST SCOPE:
- Target: Integrated system with all components (hardware + firmware)
- Input Method: Manual user actions on the target environment
- Output: System behavior, sensor feedback, control signals
- Exclusions: Internal software unit tests (covered separately)

COVERAGE CRITERIA:
- Ensure all system requirements are covered by at least one test case
- If a requirement cannot be tested manually, document why and propose alternative verification method
- Include both positive (happy path) and negative (error handling) test cases

TEST CASE TEMPLATE (ISTQB-compliant):
- Test Case ID (format: SYS-TC-XXX)
- Title (concise description of what is being tested)
- Requirement Traceability (SYS-REQ IDs)
- Preconditions (system state before test execution)
- Test Steps (numbered, sequential, with expected outcome per step)
- Expected Result (final system behavior)
- Priority (must-have, should-have, or nice-to-have)
- Test Type (functional, safety, performance, usability, etc.)

ACCEPTANCE CRITERIA:
- 8–10 system test cases
- 100% traceability coverage
- Each test case includes 3–7 explicit test steps
- Each step includes both action and expected outcome
- All preconditions clearly stated

OUTPUT FORMAT:
- Markdown file (system_test_specification.md)
- Location: tests
- Include a coverage traceability table at the end
```

---

**Next Step:** Proceed to [Traceability Matrix](05_traceability-matrix.md) to validate end-to-end coverage.