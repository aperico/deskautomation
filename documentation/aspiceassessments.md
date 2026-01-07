

# ASPICE Assessment Checklist

**Assessment Date:** January 6, 2026


**Overall Process Maturity Statement:**
Most base practices for system and software engineering are fully or largely achieved, with strong evidence in requirements, architecture, and testing. Integration and validation steps are partially manual; further automation and documentation improvements are recommended for full ASPICE compliance.

This assessment summarizes ASPICE compliance for the Automated Mechanical Desk Lift project. It covers System (SYS) and Software Engineering (SWE) base practices, with a clear status legend and process maturity overview. Use this as a quick reference for compliance, gaps, and improvement planning.

---



## Status Legend
| Code | Meaning |
|------|---------|
| N    | Not achieved |
| P    | Partially achieved |
| L    | Largely achieved |
| F    | Fully achieved |

---


## System Engineering (SYS) Assessment

| Process Area | Base Practice | Status | Evidence/Notes |
|--------------|--------------|--------|----------------|
| SYS.1 Requirements Elicitation | Collect stakeholder requirements | F | Stakeholder requirements documented ([SoftwareRequirements.md](SoftwareRequirements.md)) |
| SYS.1 Requirements Elicitation | Analyze stakeholder requirements | F | Analysis included in requirements and design docs |
| SYS.2 System Requirements Analysis | Derive system requirements | F | System requirements mapped ([TraceabilityMatrix.md](TraceabilityMatrix.md)) |
| SYS.2 System Requirements Analysis | Validate system requirements | L | Validation process described, mostly automated; some manual steps remain |
| SYS.3 System Architectural Design | Develop system architecture | F | Architecture documented ([SoftwareArchitecture.md](SoftwareArchitecture.md)) |
| SYS.3 System Architectural Design | Allocate system requirements | F | Allocation shown ([TraceabilityMatrix.md](TraceabilityMatrix.md)) |
| SYS.4 System Integration & Verification | Plan system integration | P | Integration plan partially documented |
| SYS.4 System Integration & Verification | Perform system integration | L | Manual integration steps described; most scenarios covered |
| SYS.4 System Integration & Verification | Verify system | L | Verification by unit/integration tests; coverage is high |

---


## Software Engineering (SWE) Assessment

| Process Area | Base Practice | Status | Evidence/Notes |
|--------------|--------------|--------|----------------|
| SWE.1 Software Requirements Analysis | Analyze software requirements | F | Requirements analysis ([SoftwareRequirements.md](SoftwareRequirements.md)) |
| SWE.1 Software Requirements Analysis | Validate software requirements | L | Validation described; most requirements covered |
| SWE.2 Software Architectural Design | Develop software architecture | F | Architecture ([SoftwareArchitecture.md](SoftwareArchitecture.md)) |
| SWE.2 Software Architectural Design | Allocate software requirements | F | Allocation ([TraceabilityMatrix.md](TraceabilityMatrix.md)) |
| SWE.3 Detailed Design & Unit Construction | Develop detailed design | F | Detailed design ([SoftwareDetailedDesign.md](SoftwareDetailedDesign.md)) |
| SWE.3 Detailed Design & Unit Construction | Implement software units | F | Implementation ([source/arduino/](../source/arduino/)) |
| SWE.3 Detailed Design & Unit Construction | Verify software units | L | Unit tests ([tests/UnitTests.cpp](../tests/UnitTests.cpp)); coverage is high |
| SWE.4 Integration & Verification | Plan software integration | P | Integration plan partially documented |
| SWE.4 Integration & Verification | Perform software integration | L | Manual integration steps described; most scenarios covered |
| SWE.4 Integration & Verification | Verify software | L | Verification by unit/integration tests; coverage is high |

---


## Process Maturity Overview

| Process Area | Maturity |
|--------------|----------|
| SYS.1 Requirements Elicitation | F |
| SYS.2 System Requirements Analysis | F |
| SYS.3 System Architectural Design | F |
| SYS.4 System Integration & Verification | L |
| SWE.1 Software Requirements Analysis | F |
| SWE.2 Software Architectural Design | F |
| SWE.3 Detailed Design & Unit Construction | L |
| SWE.4 Integration & Verification | L |

---


---


### Summary & Next Steps

- Most base practices are fully or largely achieved, with evidence linked to documentation.
- Integration and validation steps are partially manual; automation and documentation improvements recommended.
- Use this checklist to guide compliance reviews and process improvements.

---

## Action Items & Recommendations

| Area | Action Item | Recommendation |
|------|-------------|----------------|
| System Integration & Verification | Automate integration and verification steps | Implement CI/CD pipelines for automated system integration and test execution |
| Requirements Validation | Increase automation and traceability | Use requirement management tools and link validation results directly to requirements |
| Documentation | Improve integration and validation documentation | Add step-by-step guides and update evidence in documentation files |
| Software Integration & Verification | Expand automated test coverage | Add more integration tests and automate their execution in CI/CD |
| Process Reviews | Schedule regular ASPICE compliance reviews | Set up periodic reviews to ensure ongoing compliance and address gaps |

*For details and evidence, see referenced documentation files. Update this checklist as processes evolve or new evidence is added.*
