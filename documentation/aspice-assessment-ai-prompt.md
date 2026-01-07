
## AI Prompt for Future ASPICE Assessments

### Assessment Instruction

Perform a comprehensive ASPICE (Automotive SPICE) v3.1 assessment of this repository, evaluating all SYS and SWE process areas for maturity Level 2 (Managed) or higher. Update this assessment document maintaining the exact format structure.

### Assessment Scope

**Process Areas to Assess:**

**System Engineering (SYS):**
- SYS.1 Requirements Elicitation and Review
- SYS.2 System Requirements Analysis and Allocation  
- SYS.3 System Architectural Design and Review
- SYS.4 System Integration and Verification

**Software Engineering (SWE):**
- SWE.1 Software Requirements Analysis
- SWE.2 Software Architectural Design
- SWE.3 Software Detailed Design and Unit Construction
- SWE.4 Software Integration and Verification
 - SWE.5 Software Component Verification and Integration Verification
 - SWE.6 Software Qualification Testing

**Supporting Processes:**
- Configuration Management (SUP.8)
- Traceability Management
- Risk and Safety Management (MAN.5)

### Evidence Collection Checklist

For each process area, systematically search for and document evidence of the following:

#### 1. Work Products (Artifacts)
- [ ] **Requirements documents:** SystemUseCases.md, SoftwareRequirements.md
- [ ] **Architecture documents:** SoftwareArchitecture.md with components, interfaces, states
- [ ] **Design documents:** SoftwareDetailedDesign.md with algorithms, data structures
- [ ] **Test specifications:** SoftwareTestCasesSpecification.md, SoftwareIntegrationTestsSpecification.md
- [ ] **Test implementations:** tests/SoftwareTests.cpp, tests/IntegrationTests.cpp
- [ ] **Traceability matrix:** TraceabilityMatrix.md with UC→REQ→ARCH→TEST links
- [ ] **Safety documentation:** SafetyNotes.md with hazard analysis and safety requirements
- [ ] **Coding standards:** codingguidelines.md with measurable rules
- [ ] **Source code:** All implementation files with proper structure and comments
- [ ] **Build configuration:** CMakeLists.txt, CMakePresets.json
- [ ] **CI/CD pipeline:** .github/workflows/*.yml with automation

#### 2. Review Evidence (CRITICAL for Level 2)
- [ ] **Review status markers:** Search for "Approved", "Reviewed", "Status" in all documents
- [ ] **Reviewer identification:** Check for "Reviewers:", "Review Team:", "Approval By:" fields
- [ ] **Review meeting minutes:** Look for documentation/reviews/ folder with meeting records
- [ ] **Review signatures:** Check for date, attendees, decisions, action items
- [ ] **Review comments:** Inline comments or separate review records
- [ ] **Approval tracking:** Version history showing review cycles

**Search Commands:**
```
grep -r "Review\|Approved\|Status:" documentation/
grep -r "Reviewers:\|Review Team:" documentation/
list files in documentation/reviews/
```

#### 3. Baseline and Configuration Management
- [ ] **Version tags:** Run `git tag -l` to check for semantic versioning (v1.0.0, v1.1.0)
- [ ] **Baseline records:** Check for Baselines.md or release notes
- [ ] **Change history:** Review git log for structured commits
- [ ] **CHANGELOG:** Look for CHANGELOG.md documenting version history
- [ ] **Branch strategy:** Examine git branches and merge practices
- [ ] **Release artifacts:** Check for tagged releases with attached documentation

**Search Commands:**
```
git tag -l
git log --oneline -20
search for CHANGELOG.md or Baselines.md
```

#### 4. Traceability Evidence
- [ ] **Forward traceability:** UC → REQ → ARCH → TEST chain completeness
- [ ] **Backward traceability:** TEST → REQ → UC verification
- [ ] **Coverage metrics:** Percentage of requirements with tests, architecture with verification
- [ ] **Orphan analysis:** Identify any untraceable elements
- [ ] **Gap analysis:** Document missing traceability links
- [ ] **Traceability tool:** Automated or manual matrix maintenance

**Search Commands:**
```
read TraceabilityMatrix.md completely
verify each UC has REQ mappings
verify each REQ has ARCH and TEST mappings
```

**Note:** When collecting traceability evidence also gather evidence for Process Attributes PA.2.1 (Performance Management) and PA.2.2 (Work Product Management) — e.g., metrics, baselines, templates, and approval records.

#### 5. Verification and Testing Evidence
- [ ] **Unit test specifications:** Test cases with ID, objective, preconditions, steps, expected results
- [ ] **Integration test specifications:** System-level test scenarios
- [ ] **Test implementations:** Actual test code with assertions
- [ ] **Test execution records:** CI logs, JUnit XML files, test reports
- [ ] **Coverage reports:** Code coverage metrics (gcov, lcov, or equivalent)
- [ ] **Test traceability:** Each test mapped to requirements
- [ ] **Test pass/fail status:** Evidence of successful test execution
- [ ] **Regression testing:** Automated test execution in CI/CD

**Search Commands:**
```
read all test specification files
count unit tests vs integration tests
check CI workflow for test execution steps
search for coverage.info or test-results*.xml
```

#### 6. Safety and Risk Management
- [ ] **Safety requirements:** Identified and prioritized safety-critical requirements
- [ ] **Safety verification:** Tests for emergency stops, fault detection, error recovery
- [ ] **Hazard analysis:** FMEA, HAZOP, or equivalent risk assessment
- [ ] **Safety verification report:** Documentation of safety test results
- [ ] **Compliance statements:** ISO 25119, ISO 26262, or other safety standards referenced
- [ ] **Safety review:** Independent safety assessment records

**Search Commands:**
```
grep -r "safety\|Safety\|SAFETY" documentation/
search SafetyNotes.md
filter requirements by "Safety" or "Critical" priority
identify emergency stop and fault handling tests
```

#### 7. Process Compliance Evidence
- [ ] **Process documentation:** Defined development process (waterfall, agile, V-model)
- [ ] **Work product templates:** Standardized formats for requirements, design, tests
- [ ] **Quality gates:** Defined checkpoints with entry/exit criteria
- [ ] **Metrics:** Quantitative measures (coverage %, defect density, traceability %)
- [ ] **Tool qualification:** Evidence of tool validation (compilers, static analyzers)
- [ ] **Training records:** (if applicable) Evidence of team competency

### Assessment Execution Steps

#### Step 1: Repository Structure Analysis
```
1. List all directories: documentation/, source/, tests/, .github/
2. Count files by type: *.md, *.cpp, *.h, *.yml
3. Identify missing artifacts (compare to artifact inventory)
```

#### Step 2: Documentation Completeness Review
```
For each document type:
- Read full content of SystemUseCases.md
- Read full content of SoftwareRequirements.md
- Read full content of SoftwareArchitecture.md
- Read full content of SoftwareDetailedDesign.md
- Read full content of TraceabilityMatrix.md
- Read test specifications
- Read SafetyNotes.md
- Check for review status, approval dates, reviewer names
```

#### Step 3: Code and Test Review
```
1. Count unit tests in tests/SoftwareTests.cpp
2. Count integration tests in tests/IntegrationTests.cpp
3. Verify test naming matches specification
4. Check test-to-requirement traceability
5. Review source code structure and comments
6. Verify coding standard compliance
```

#### Step 4: CI/CD and Automation Analysis
```
1. Read .github/workflows/*.yml completely
2. Document build steps, test execution, coverage reporting
3. Check for static analysis (Cppcheck, linters)
4. Verify artifact generation (JUnit XML, coverage reports)
5. Assess automation level (manual vs. automated)
```

#### Step 5: Traceability Verification
```
1. Open TraceabilityMatrix.md
2. Verify UC → REQ mappings (100% expected)
3. Verify REQ → ARCH mappings (100% expected)
4. Verify REQ → TEST mappings (100% expected)
5. Calculate coverage percentages
6. Identify orphans (requirements without tests, tests without requirements)
```

#### Step 6: Review Evidence Collection
```
1. Search for "Approved" in all documentation
2. Search for "Reviewers:" or "Review Team:"
3. List files in documentation/reviews/ (if exists)
4. Check git history for review-related commits
5. Document presence/absence of formal review records
```

#### Step 7: Gap Analysis for Each Process Area
```
For SYS.1, SYS.2, SYS.3, SYS.4, SWE.1-5:
- List all required base practices (BP1, BP2, BP3, etc.)
- For each base practice, identify evidence (present/absent)
- Determine achievement level: F (Fully), L (Largely), P (Partially), N (Not achieved)
- Document specific gaps with actionable recommendations
```

#### Step 8: Maturity Level Determination
```
Level 2 (Managed) requires:
- All base practices have evidence of planning and work product management
- Reviews are conducted and documented
- Traceability is maintained
- Configuration management is in place
- Metrics are defined and tracked

Level 3 (Established) additionally requires:
- Standard processes are defined and tailored
- Process assets are collected and reused
- Quantitative process control
- Formal baseline management with version tagging
```

### Status Rating Criteria

Use these criteria for assigning status to each process area:

**🟩 F (Fully Achieved):**
- All base practices have complete evidence
- All work products present and approved
- Reviews documented with clear outcomes
- No significant gaps identified
- Meets or exceeds Level 2 criteria

**🟦 L (Largely Achieved):**
- Most base practices have strong evidence
- Core work products present and approved
- Reviews documented but may lack some formality (e.g., no meeting minutes)
- Minor gaps that don't impact core functionality
- Meets Level 2 with small improvements needed

**🟨 P (Partially Achieved):**
- Some base practices have evidence
- Work products present but may lack approval or completeness
- Limited review evidence
- Significant gaps requiring attention
- Approaching Level 2 but needs work

**🟥 N (Not Achieved):**
- Insufficient or no evidence of base practices
- Work products missing or severely incomplete
- No review evidence
- Does not meet Level 2 criteria

### Document Update Instructions

Update the following sections maintaining the exact format:

#### 1. Assessment Header
```markdown
**Assessment date:** [Current date YYYY-MM-DD]
**Reviewer:** [Your name/team]
**Baseline:** Git commit [hash] (branch, date)
**Overall conclusion:** Level [X] ([status]) — [Brief justification]
```

#### 2. Summary Section
Update **Strengths** and **Areas for Enhancement** based on new findings

#### 3. Evaluation Results Table
For each process area, update:
- Status column (🟩 F / 🟦 L / 🟨 P / 🟥 N)
- Evidence / Notes with specific findings and gaps

#### 4. Maturity Summary Table
Update status and evidence summary for all 12 process areas

#### 5. Detailed Assessment Sections
For each SYS.x and SWE.x section:
- Update **Status:** line
- Add/remove evidence items (✅ for present, ⚠️ for gaps)
- Update **Gap:** bullets with new findings
- Revise **Recommendation:** with actionable improvements

#### 6. Coverage and Metrics Summary Table
Recalculate all metrics:
- Use Cases Documented: X/Y (Z%)
- Requirements with Test Coverage: X/Y (Z%)
- Test Traceability: X/Y (Z%)
- Update all Pass/Fail status

#### 7. Recommended Actions
Prioritize actions as:
- **High Priority:** Essential for Level 2/3
- **Medium Priority:** Strengthens maturity
- **Low Priority:** Process improvement

#### 8. Conclusion Section
Summarize:
- Overall maturity level achieved
- Key achievements (bulleted list)
- Level 3 foundations (if any)
- Primary gaps (bulleted list)
- Final recommendation

#### 9. Artifact Inventory
Update counts and add new artifacts:
- Documentation (X documents)
- Source Code files
- Test Code files
- Build and CI files
- Configuration files

### Output Format Requirements

**CRITICAL:** Maintain the exact markdown structure:
- Use `**bold**` for labels
- Use ✅ for present evidence
- Use ⚠️ for gaps/warnings
- Use 🟩/🟦/🟨/🟥 emoji for status
- Use `---` horizontal rules between major sections
- Use proper markdown tables with alignment
- Use `#`, `##`, `###` headers at correct levels
- Include all original sections even if unchanged

### Common Pitfalls to Avoid

1. **Missing review evidence:** Don't assume approval without explicit "Reviewed by" or "Approved" markers
2. **Incomplete traceability check:** Verify bidirectional traceability, not just forward links
3. **Ignoring CI evidence:** CI/CD artifacts count as verification evidence
4. **Overlooking safety:** Safety requirements need explicit identification and verification
5. **Baseline confusion:** Git commits alone aren't baselines without tags/releases
6. **Test execution vs. specification:** Both test specs AND executed tests are required
7. **Coverage gaps:** 100% requirement coverage doesn't mean 100% code coverage
8. **Review formality:** "Status: Approved" is better than nothing, but formal review records are needed for Level 3

### Final Validation Checklist

Before completing the assessment:
- [ ] All 12 process areas assessed (SYS.1-4, SWE.1-5, SUP.8, Traceability, Safety)
 - [ ] All process areas assessed (SYS.1-4, SWE.1-6, SUP.8, Traceability, Safety)
- [ ] Each process area has status (F/L/P/N) with justification
- [ ] Evidence bullets (✅) cite specific files or findings
- [ ] Gaps (⚠️) have actionable recommendations
- [ ] Metrics table has current values with Pass/Fail status
- [ ] Conclusion states clear maturity level (0/1/2/3)
- [ ] Artifact inventory updated with new files
- [ ] Baseline identifies git commit hash and date
- [ ] Overall conclusion matches detailed findings
- [ ] Recommendations prioritized and actionable

---

### Example Assessment Command Flow

```
# Repository structure
list_dir(.)
list_dir(documentation/)
list_dir(source/)
list_dir(tests/)

# Documentation review
read_file(documentation/SystemUseCases.md, 1, 100)
read_file(documentation/SoftwareRequirements.md, 1, 200)
read_file(documentation/SoftwareArchitecture.md, 1, 300)
read_file(documentation/TraceabilityMatrix.md, 1, 500)
read_file(documentation/SoftwareTestCasesSpecification.md, 1, 200)

# Code and test review
read_file(tests/SoftwareTests.cpp, 1, 500)
read_file(tests/IntegrationTests.cpp, 1, 500)

# CI/CD review
read_file(.github/workflows/cmake-single-platform.yml, 1, 100)

# Version control
run_in_terminal: git log --oneline -20
run_in_terminal: git tag -l
run_in_terminal: git branch -a

# Review evidence
grep_search: "Approved|Review|Status:" in documentation/

# Test artifacts
run_in_terminal: ls -la build/test-results*.xml (if exists)

# Update assessment
replace_string_in_file: aspiceassessments.md (multiple sections)
```

---

**Usage:** Copy the instruction block above and paste it to an AI assistant when performing ASPICE assessments. The assistant will systematically collect evidence, evaluate maturity, and update this document maintaining the established format and professional quality standards.