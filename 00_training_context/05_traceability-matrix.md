# Session 5.0: Traceability Matrix

**Focus:** Traceability

**Time:** ~15 minutes

---

## Part 1: Creating Traceability Matrix

Traceability matrix gives full visibility of how the artifacts are definided, implemented and tested.
The ultimate goal is to ensure 100% System Requirements are implemented and coverted with "passing" system test cases (same for all underlying layers).


**Prompt:**
```
Create a full traceability matrix for the desk automation system.

Format: Markdown table with columns:
- Mission Objective (OBJ-001, etc.)
- System Requirement (SysReq-001, etc.)
- Software Requirement (SWReq-001, etc.)
- Code Component (HAL, MotorController, DeskApp, etc.)
- Unit Test (TEST-001, etc.)
- Integration Test (optional)

Link each objective → system req → software req → component test → System Test.
- Verify no orphan artifacts (every requirement must have a test).
- On the header, add date of the last update.
- Identify each dependency file version (or respective last-update date)

Example rows:
| Objective | SysReq | SWReq | Component Test | System Test |

Complete the matrix for all artifacts.
Store in documentation traceability_matrix

After the table, create a table with all gaps and recommendations on how to close.
```

**Verify:**
- Every objective links to ≥1 system requirement
- Every system requirement links to ≥1 software requirement
- Every software requirement links to ≥1 component test
- Every component test links to ≥1 software Requirement
- Every system test links to ≥1 system requirement
- No orphan requirements (all traced top to bottom)

---

## Additional Resources

- **IEEE 29148 Requirements Standard:** [ieee.org](https://ieee.org)
- **ASPICE 4.0 Process Guide:** [automotivesoft.org](https://automotivesoft.org)
- **V-Model Tutorial:** [systems-engineering.org](https://systems-engineering.org)

----

**Next Step:** Proceed to [workflow pipelines](06_workdlow-pipelines.md).