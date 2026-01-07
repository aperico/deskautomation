# Automotive SPICE v3.1 — SYS & SWE Base Practices (Level 2 Focus)

This document collects the ASPICE Process Assessment Model (PAM) Base Practices (BPs) for the System (SYS) and Software (SWE) process groups relevant to assessing Level 2 (Managed). It combines the user's extracted content with the complete set of SYS/SWE base practices and organizes them for practical evidence collection.

> Note: This file focuses on base practices required to demonstrate process purpose (Level 1) and the Level 2 process attributes (Performance Management PA2.1 and Work Product Management PA2.2). Review and tailor BP evidence names to match repository artifacts.

---

## How to use this file
- For each BP below gather concrete evidence (files, commits, meeting notes) and add references in the "Evidence" subsection.
- Use the existing extracted summary (from `aspicebasepractices.txt`) as partial evidence and fill missing items.

---

# SYS Processes

## SYS.1 Requirements Elicitation and Review
Purpose: Elicit and review stakeholder requirements for the system.

Base Practices (BPs)
- BP1 Obtain customer/ stakeholder requirements and requests
- BP2 Identify and analyze stakeholders and their needs
- BP3 Define system boundaries and interfaces from stakeholder perspective
- BP4 Elicit and document stakeholder requirements
- BP5 Ensure stakeholder requirements are complete, consistent, understandable, and testable
- BP6 Resolve conflicts and negotiate requirements with stakeholders
- BP7 Establish acceptance criteria and verification methods
- BP8 Maintain records of requirements elicitation (meeting minutes, decisions)
- BP9 Conduct reviews of stakeholder requirements and record approvals

Evidence (example)
- SystemUseCases.md — approved use cases
- SoftwareRequirements.md — stakeholder & software-level requirements
- documentation/reviews/Requirements_Review_YYYYMMDD.md (create if missing)

---

## SYS.2 System Requirements Analysis and Allocation
Purpose: Transform stakeholder requirements to system requirements and allocate them to system elements.

Base Practices (BPs)
- BP1 Establish system-level requirements derived from stakeholder needs
- BP2 Structure and group system requirements (functional, non-functional, safety)
- BP3 Analyze system requirements for correctness, completeness, and testability
- BP4 Derive and document system verification criteria for each requirement
- BP5 Allocate system requirements to system elements (hardware, software, mechanical)
- BP6 Perform impact analysis on operating environment and constraints
- BP7 Maintain bidirectional traceability (UC → REQ → ARCH → TEST)
- BP8 Review system requirements and record approvals

Evidence (example)
- TraceabilityMatrix.md — allocation and bidirectional links
- System requirements spec (documentation/SoftwareRequirements.md)

---

## SYS.3 System Architectural Design and Review
Purpose: Create and review system architecture describing components, interfaces and behavior.

Base Practices (BPs)
- BP1 Develop system architectural design (static view)
- BP2 Describe dynamic behavior and interactions (state machines, sequences)
- BP3 Identify and specify interfaces between system elements
- BP4 Allocate system requirements to architecture elements
- BP5 Evaluate alternative architectures and record decisions (ADR)
- BP6 Define verification criteria for architecture-level properties
- BP7 Ensure traceability between architecture and requirements/tests
- BP8 Conduct architecture reviews and record outcomes (meeting minutes, approvals)

Evidence (example)
- SoftwareArchitecture.md
- ADR-### files
- Architecture review records

---

## SYS.4 System Integration and Verification
Purpose: Integrate system elements and verify the integrated system against system requirements.

Base Practices (BPs)
- BP1 Plan system integration strategy and environment (including HIL if applicable)
- BP2 Specify system integration test cases and expected results
- BP3 Integrate system elements according to plan
- BP4 Perform integration verification and record results
- BP5 Trace integration tests to system requirements and architecture
- BP6 Summarize and communicate integration results (integration test report)
- BP7 Maintain integration configuration and baselines
- BP8 Review integration outcomes and record approvals

Evidence (example)
- SoftwareIntegrationTestsSpecification.md
- tests/IntegrationTests.cpp
- CI artifacts (JUnit XML, coverage)

---

## SYS.5 System Qualification Testing
Purpose: Confirm the complete system fulfills stakeholder requirements under operational conditions.

Base Practices (BPs)
- BP1 Define system qualification test strategy (environments, acceptance criteria)
- BP2 Specify system qualification test cases and pass/fail criteria
- BP3 Execute qualification tests and record results
- BP4 Perform regression selection and re-execute as needed
- BP5 Trace qualification tests to requirements and acceptance criteria
- BP6 Produce system test report and record approvals
- BP7 Archive test artifacts linked to baselines/releases

Evidence (example)
- Qualification test plans
- Release test reports
- Archived CI artifacts per release

---

# SWE Processes

## SWE.1 Software Requirements Analysis
Purpose: Derive and manage software requirements allocated from the system level.

Base Practices (BPs)
- BP1 Identify software requirements derived from system requirements
- BP2 Structure and categorize software requirements
- BP3 Analyze software requirements for correctness, completeness, and verifiability
- BP4 Analyze the impact on the operating environment and constraints
- BP5 Define software verification criteria for each requirement
- BP6 Establish and maintain bidirectional traceability to system requirements, design and tests
- BP7 Communicate software requirements to development and test teams
- BP8 Manage requirements changes (change control)

Evidence (example)
- SoftwareRequirements.md
- TraceabilityMatrix.md
- Requirement change logs (git commits referencing requirements)

---

## SWE.2 Software Architectural Design
Purpose: Define software architecture and allocate requirements to software components.

Base Practices (BPs)
- BP1 Develop software architecture: static and dynamic views
- BP2 Allocate software requirements to software components
- BP3 Specify component interfaces and contracts
- BP4 Define behavior models (state machines, sequence diagrams)
- BP5 Analyze architecture alternatives and trade-offs
- BP6 Ensure architecture supports quality attributes (safety, performance)
- BP7 Define architecture-level verification criteria
- BP8 Maintain traceability to requirements and design
- BP9 Conduct architecture reviews and record approvals

Evidence (example)
- SoftwareArchitecture.md
- Interface definition documents
- Architecture review minutes

---

## SWE.3 Software Detailed Design and Unit Construction
Purpose: Produce detailed design and implement software units consistent with the software architecture.

Base Practices (BPs)
- BP1 Produce detailed design for each software component (static view)
- BP2 Produce dynamic design for components (algorithms, state behavior)
- BP3 Define unit interfaces and data structures
- BP4 Review detailed design against architecture and requirements
- BP5 Establish unit-level verification criteria and unit test cases
- BP6 Implement software units according to the detailed design
- BP7 Apply coding standards and perform code reviews
- BP8 Maintain traceability from units to design/architecture/requirements
- BP9 Manage unit-level configuration and control

Evidence (example)
- SoftwareDetailedDesign.md
- Source files under `source/arduino/`
- Unit test code `tests/SoftwareTests.cpp`
- Code review records and static analysis results

---

## SWE.4 Software Unit Verification
Purpose: Verify that software units correctly implement the detailed design.

Base Practices (BPs)
- BP1 Define unit verification strategy and environment
- BP2 Specify unit verification measures (test cases, test data)
- BP3 Select unit verification measures (regression subset)
- BP4 Execute unit verification and collect results
- BP5 Measure code coverage and meet coverage targets
- BP6 Trace unit tests to detailed design and requirements
- BP7 Summarize and communicate unit verification results
- BP8 Record unit verification approvals and any deviations

Evidence (example)
- Unit test results (CI JUnit XML)
- Coverage reports (gcov/lcov)
- Unit verification report

---

## SWE.5 Software Integration and Integration Verification
Purpose: Integrate software units and verify integrated behavior against software architecture and requirements.

Base Practices (BPs)
- BP1 Plan software integration strategy and test environment
- BP2 Specify integration verification measures and test cases
- BP3 Select integration regression tests
- BP4 Integrate software units incrementally and manage builds
- BP5 Execute integration verification and record results
- BP6 Ensure traceability between integration tests, architecture and requirements
- BP7 Summarize integration results and approvals

Evidence (example)
- Integration test specs and results
- CI logs showing integration builds and test execution
- Integration test report

---

## SWE.6 Software Qualification Testing
Purpose: Confirm that the integrated software meets software requirements and acceptance criteria.

Base Practices (BPs)
- BP1 Develop software qualification test strategy
- BP2 Specify qualification test cases and acceptance criteria
- BP3 Select regression suite for qualification
- BP4 Execute qualification tests in target-like environments
- BP5 Trace qualification results to software requirements
- BP6 Summarize qualification results and obtain sign-off
- BP7 Archive qualification test artifacts with baselines/releases

Evidence (example)
- Qualification test reports
- Release artifacts with attached test results

---

# Process Attributes Required for Level 2 (Managed)

## PA 2.1 Performance Management (applies to each process)
- Define measurable objectives for the process
- Plan process execution and monitoring activities
- Monitor performance using metrics (coverage, test pass rates, schedule adherence)
- Take corrective actions when performance deviates

## PA 2.2 Work Product Management (applies to each process)
- Define work product requirements (templates, contents)
- Establish version and configuration control for work products
- Document review and approval mechanisms for work products
- Archive and baseline work products for traceability

---

# Assessment & Evidence Checklist (practical)
For each BP in the processes above, collect:
- Work product files or references (file paths, commit ids)
- Review records (dates, attendees, decisions)
- Traceability links (line-item or matrix references)
- Test execution artifacts (JUnit XML, coverage reports)
- Configuration/baseline evidence (tags, release notes)

---

# Notes & Next Steps
- Review the repository and populate the "Evidence" sections with file links and commit ids.
- Create missing review records under `documentation/reviews/` for requirements and architecture.
- Add baselines (git tags) and archive CI artifacts per release for Level 2 evidence.

---

Document version: 1.0
Generated: 2026-01-07
Author: Automated extraction and review (assisted)
