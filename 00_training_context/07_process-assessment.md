# Session 7: Process Assessment

**Focus:** Evaluating maturity based on ASPICE 4

**Time:** ~20 minutes


---

## Part 1: ASPICE 4.0 Process Assessment

**Objective:** Perform objective, evidence-based ASPICE 4.0 maturity assessment for Standing Desk Automation System project.

**Scope:** Evaluate software development processes against ASPICE 4.0 process model; identify gaps; recommend improvements.

**Assessment Approach:** 
- Review ACTUAL project artifacts (not hypothetical)
- Evaluate against ASPICE capability levels (1-5)
- Document evidence for each finding
- Identify gaps with specific corrective actions
- Focus on safety-critical embedded systems domain (ISO 25119 relevant)

Prompt:
```
You are an ASPICE 4.0 Process Assessment Lead for embedded software projects in the automotive/industrial domain.

CONTEXT - PROJECT ARTIFACTS:
Below is evidence from the Standing Desk Automation System (embedded C++, Arduino UNO, safety-critical motor control):

Artifact Evidence:
   - Search within project file structure for all artifacts needed

TASK: Perform ASPICE 4.0 maturity assessment covering ENGINEERING PROCESSES ONLY:

**PROCESS GROUP 1 (SYSTEM LEVEL)**
   - SYS.1 (Stakeholder Requirements Definition)
   - SYS.2 (System Requirements Analysis)
   - SYS.3 (System Architectural Design)
   - SYS.4 (System Integration & Integration Testing)
   - SYS.5 (System Qualification Testing)
   
   Assess: Evidence of stakeholder engagement, requirements completeness, design review, integration planning, test strategy?

**PROCESS GROUP 2: (SOFTWARE LEVEL)**
   - SWE.1 (Software Requirements Analysis)
   - SWE.2 (Software Architectural Design)
   - SWE.3 (Software Detailed Design & Implementation)
   - SWE.4 (Software Unit Verification)
   - SWE.5 (Software Integration & Integration Testing)
   - SWE.6 (Software Qualification Testing)
   
   Assess: Requirements traceability, architecture clarity, code quality, unit test coverage, integration test planning, system test planning

**NOTE:** Support (SUP) and Management (MAN) processes NOT included in this assessment. Focus is exclusively on SYS and SW engineering processes.

ASSESSMENT RULES (EVIDENCE-BASED):

❌ DO NOT assume compliance; verify against ACTUAL artifacts
❌ DO NOT give high ratings without documented evidence
❌ DO NOT ignore missing processes; call out gaps explicitly
✅ REQUIRE: Design reviews (show evidence of review comments/approvals)
✅ REQUIRE: Test evidence (pass/fail reports, traceability, coverage metrics)
✅ REQUIRE: Version control (git commits, tags, baselines)
✅ REQUIRE: Change management (if changes made, show control process)
✅ REQUIRE: Risk register (identify, assess, mitigate known risks)
✅ REQUIRE: Metrics (test pass rates, defect counts, traceability completeness)
✅ REQUIRE: Quantified data (not subjective statements like "good" or "mature")

DELIVERABLE FORMAT:

### Part A: Capability Level Assessment by Process

For each ASPICE process (SYS.1, SW.1, etc.):
| Process ID | Process Name | Capability Level | Evidence | Gaps |
|------------|-------------|------------------|----------|------|
| SYS.1 | Stakeholder Requirements Definition | 2 (Repeatable) | Mission Statement (00_mission_statement.md) + 5 OBJ with measurable criteria + ConOps scenarios | Gap: No stakeholder sign-off documented; recommend: Add stakeholder approval matrix with dates |
| ... | ... | ... | ... | ... |

Capability Levels (ASPICE):
- Level 1 (Performed): Process produces required outputs; no formal documentation or reviews required
- Level 2 (Repeatable): Process outputs reviewed; requirements managed; basic planning exists
- Level 3 (Defined): Process standardized; tailored for project; documented in procedure; training delivered
- Level 4 (Capable): Process measured; metrics tracked; quantitative targets set; continuous improvement
- Level 5 (Innovative): Process optimized; lessons learned captured; innovation tools employed

### Part B: Maturity Profile

| Process Area | Current Level | Target Level | Gap |
|--------------|---------------|--------------|-----|
| System Level (SYS.1-5) | 2 (avg) | 3 | Need stakeholder reviews, acceptance criteria sign-off, architecture review |
| Software Level (SW.1-6) | 2 | 3 | Need software architecture review meeting, integration test plan formalization, SW design documentation |

### Part C: Gap Analysis with Corrective Actions (SYS & SW Processes)

| Gap ID | ASPICE Process | Finding | Impact | Corrective Action | Priority | Timeline | Owner |
|--------|----------------|---------|--------|-------------------|----------|----------|-------|
...

### Part D: Maturity Level Assessment (SYS & SW Processes)

**Current Estimated Maturity Level:** 2 (Repeatable / Managed)

**Rationale:**
- ✅ Strengths (evidence of Level 2):
  
- ⚠️ Weaknesses (blocking Level 3 and above):


- ➜ Assessment: Project PRODUCES required engineering artifacts at Level 2 (outputs exist), but lacks PROCESS DEFINITION and FORMAL REVIEWS required for Level 3

**Target Maturity Level:** 3 (Defined / Managed)

**Justification:** For safety-critical automotive/industrial embedded systems (ISO 26262), Level 3 is industry minimum; demonstrates:
- Formal design reviews and approvals (not ad-hoc decisions)
- Documented test plans and strategies (not informal testing)
- Peer code reviews (not solo development)
- Quantified test coverage and traceability evidence (not assumed)
- Process procedures (roles, responsibilities, inputs/outputs standardized)

### Part E: Roadmap to Level 3 (SYS & SW Processes Only)
...

IMPORTANT ASSESSMENT GUIDELINES:
- Be CRITICAL: Identify every gap; do not assume compliance
- Be SPECIFIC: Quote evidence sources; reference artifact paths/line numbers
- Be QUANTIFIED: "No metrics" is better than "weak metrics"; count items, measure coverage
- Be ACTIONABLE: Every gap must have a corrective action with owner and date
- Be HONEST: If evidence is missing, say so; don't infer or assume
- Be TRACEABLE: Link gaps to ASPICE processes (e.g., GAP-ASP-001 → SUP.2 Change Management)

OUTPUT ONLY the assessment report in Markdown format with all sections above (A-E). Focus exclusively on SYS and SW processes; do NOT include Support (SUP) or Management (MAN) processes. No introduction, no caveats, no disclaimers. Assume reader is C-level executive familiar with ASPICE but unfamiliar with this specific project.
```

