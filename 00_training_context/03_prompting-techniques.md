# Session 3: Prompting Techniques

**Time:** 35 minutes (20 min core techniques + 15 min practice)

---

## Learning Objectives

By the end of this session, you will:
- **Exercise different prompting techniques** with worked examples.
- **Practice techniques** with mini-exercises and iterations.
- **Understand when to use each technique** to improve AI output quality.
- **Recognize common prompting pitfalls** and how to avoid them.
- **Build confidence generating high-quality prompts** for requirements, architecture, and tests.

---

## Why Prompting Matters

AI models respond to ***how*** you ask questions. A vague prompt yields vague code; a specific, contextual prompt yields better results. Think of prompting as **API design for AI**.

**Poor prompt:**
```
Write requirements for a motor controller.
```
→ Generic, vague, unusable output.

**Good prompt:**
```
Write 3 functional requirements for a brushed DC motor controller that drives a standing desk lift. Desk height range: 30–120 cm, motor max current: 2A, speed: 30 sec for full stroke. Use IEEE 29148 format: REQ-ID, statement (with "shall"), rationale, acceptance criteria.
```
→ Specific, measurable, traceable, implementable.

---

## Quick Reference: 8 Prompting Techniques

| # | Technique | What It Does | When to Use | Quick Pattern |
|---|-----------|--------------|-------------|---------------|
| 1 | **Be Specific** | Add numbers, constraints, context | Output too vague/generic | "Write N [artifact] for [system]. Context: [hardware/limits]. Metrics: [numbers]. Format: [structure]." |
| 2 | **Provide Context** | Give upstream facts (hardware, safety, tests) | AI misses domain knowledge | "Design [artifact]. Context: [HW/I/O/safety/tests]. Output: [format]." |
| 3 | **Request Format** | Specify output structure (table/code/diagram) | Need parseable/reusable output | "Create [artifact]. Format: markdown table with columns [A/B/C]. Include N rows." |
| 4 | **Include Examples** | Show quality/style sample | Want consistent results | "Follow this example → [paste]. Now produce N items on [topics], same detail level." |
| 5 | **Set Constraints** | Define boundaries (shall-only, testable, limits) | Enforce quality/standards | "Write N [artifacts]. Constraints: [must-haves/must-nots]. Violating = rejected." |
| 6 | **Request Traceability** | Link outputs to inputs (SysReq→SWR→Test) | Prevent orphan artifacts | "Create table: [ID \| Statement \| Derives From \| Verified By]. Use given upstream." |
| 7 | **Iterate** | Ask, review, refine | First answer inadequate | "Rewrite to be [quantitative/testable/contextual]. Add [missing elements]." |
| 8 | **Ask for Alternatives** | Get multiple options with pros/cons | Need to compare trade-offs | "Propose 3 options. For each: description, pros, cons, effort (1–5), best for." |

**Pro tip:** Combine techniques! Example: Be Specific + Provide Context + Request Format + Request Traceability = high-quality requirements table.

---

## Core Techniques (5 Essential — Master These First)

Each technique follows: (what) + (why) + (how-to prompt) + (mini-drill with solution).

### Technique 1: Be Specific
- **What**: Include numbers, constraints, context, measurable details.
- **Why**: More patterns → better AI matches.
- **Prompt pattern**: "Write N requirements for X. Context: [facts]. Metrics: [numbers/tolerances]. Format: [REQ-ID, statement, rationale, acceptance]."
- **Quick example**: 1 Hz blink on D13, 5V, 1s on/off, verify with scope.

#### Mini-Drill 1: Be Specific
**Given (vague):**  
"Motor shall move the desk."

**Your task:** Rewrite with load (kg), range (cm), speed (cm/s), current limit (A), safety constraint.

**Solution sketch:**  
"Motor shall lift 50kg load from 30cm to 120cm at 3cm/s (±10%), consuming ≤2A, and stop within 100ms of STOP button press."

**Why this works:**
- ✅ Load: 50kg (testable)
- ✅ Range: 30–120cm (measurable)
- ✅ Speed: 3cm/s ±10% (tolerance)
- ✅ Current: ≤2A (safety)
- ✅ Latency: ≤100ms (performance)

### Technique 2: Provide Context
- **What**: Give upstream facts (hardware, limits, safety, tests).
- **Why**: Reduces generic answers.
- **Prompt pattern**: "Design [artifact]. Context: [hardware, I/O, safety, verification]. Output: [architecture/state machine/components]."
- **Quick example**: Desk lift controller with UNO, L298N, buttons, STOP ≤100 ms.

#### Mini-Drill 2: Provide Context
**Given (vague):**  
"Write a test plan for the motor controller."

**Your task:** Add context about system, scenarios, tools, coverage.

**Solution sketch:**  
"Write a component test plan for a motor controller that lifts a standing desk.  

Context:  
- Motor: 12V DC, max 2A, stall 3A  
- Controls: UP/DOWN/STOP buttons, LIMIT switch  
- Safety: Motor stops ≤100ms on STOP  
- Failures: Stall (no motion 5s), LIMIT malfunction  
- Test framework: Google Test, mock HAL (no real HW)  

Include: Critical paths, edge cases, fault injection. Map each test to SWR."

**Why this works:** Hardware specs, failure modes, test tools, and traceability requirement all specified.

### Technique 3: Request Format
- **What**: Specify output shape (table/code/diagram).
- **Why**: Reduces parsing/cleanup.
- **Prompt pattern**: "Create [artifact]. Format: markdown table columns [...]. Include N items." 
- **Quick example** (traceability table, excerpt):

| Req ID  | Requirement                      | Test ID  | Test Case              | Acceptance Criteria                      |
|---------|----------------------------------|----------|------------------------|------------------------------------------|
| REQ-001 | Motor shall accept PWM 0–255     | TEST-001 | Verify PWM input range | 0–5V proportional, ±1 LSB resolution     |
| REQ-002 | Motor shall ramp speed 500 ms    | TEST-003 | Ramp time measurement  | 500 ± 50 ms to max speed                 |
| REQ-003 | Motor shall stop ≤100 ms on STOP | TEST-004 | STOP latency           | GPIO low within 100 ms of button press   |

#### Mini-Drill 3: Request Format
**Given (no format):**  
"Describe the pin assignments for the motor controller."

**Your task:** Rewrite specifying a markdown table format.

**Solution sketch:**  
"Create a pin assignment table for an Arduino UNO motor controller.  

Format: Markdown table with columns:  
- Pin Number (D0, D1, ... D13, A0, A1, ...)  
- Arduino Function (RX, TX, PWM, etc.)  
- Motor Function (PWM to L298N, button input, LED, etc.)  
- Direction (INPUT, OUTPUT)  
- Notes (pull-up required, active LOW, etc.)  

Include: UP button, DOWN button, LIMIT switch, STOP button, LED (status), motor PWM."

**Why this works:** Column names, expected rows, and all connections specified.

### Technique 4: Iterate
- **What**: Ask, review, refine.
- **Why**: First answer is rarely final.
- **Prompt pattern**: "Rewrite to be quantitative, testable, contextual; add rationale + acceptance." 
- **Quick example**: Turn "Motor shall move" into PWM 0–255 ±2%, stop ≤100 ms, ramp 500 ±50 ms with acceptance.

#### Mini-Drill 4: Iterate 
**Given (inadequate first output):**  
```cpp
class MotorController {
public:
  void control();
};
```

**Your task:** Write a refinement prompt that adds methods, members, comments, and stall detection.

**Solution sketch:**  
"The class is incomplete. Refine it to include:  
1. Constructor and destructor  
2. Methods: `setSpeed(pwm)`, `getSpeed()`, `stop()`, `detectStall()`, `checkLimits()`  
3. Private members: `motor_speed`, `stall_flag`, `last_motion_time`  
4. Detailed comments per method  
5. `checkMotorStall()` detects no motion for ≥5s at max PWM  

Format: Full C++ header file with inline documentation."

**Why this works:** Specific missing elements, concrete method names, and format requirement.

### Technique 5: Request Traceability
- **What**: Link outputs to inputs (SysReq/ConOps → SWR → Test).
- **Why**: Prevents orphan artifacts.
- **Prompt pattern**: "Create table: [ID | Statement | Derives From | Verified By]. Use given SysReq/ConOps."
- **Quick example**:

| SWR ID | SWR Statement                     | Derives From  | Verified By        |
|--------|-----------------------------------|---------------|--------------------|
| SWR-001| Motor accepts PWM 0–255           | SysReq-001    | TEST-001, TEST-002 |
| SWR-002| Ramp 500 ms                       | SysReq-001    | TEST-003           |
| SWR-003| Stop ≤100 ms on STOP              | SysReq-001    | TEST-004           |

#### Mini-Drill 5: Request Traceability 
**Given (no traceability):**  
"Write 5 test cases for the motor controller."

**Your task:** Add traceability to 5 software requirements (SWR-001 through SWR-005).

**Solution sketch:**  
"Write 5 unit test cases for the motor controller.  

Software Requirements (reference):  
- SWR-001: Motor PWM input 0–255  
- SWR-002: Speed ramp 500 ms  
- SWR-003: Emergency STOP ≤100 ms  
- SWR-004: Limit switch enforcement  
- SWR-005: Stall detection timeout (5 sec)  

For each test: state which SWR(s) it verifies, setup, expected result, acceptance.  

Format:  
| Test ID | SWR Link | Test Name | Expected Result |  
|---------|----------|-----------|-----------------|  
| TEST-001| SWR-001  | ...       | ...             |"

**Why this works:** Explicit SWR list, traceability requirement, and table format.

---

## Advanced Techniques (3 Optional — Use When Needed)

### Technique 6 (Advanced): Include Examples
- **What**: Show a sample that sets quality bar.
- **Why**: AI mimics the shown style.
- **Prompt pattern**: "Follow this example → [paste]. Now produce N items on topics [...], same structure." 
- **Use when**: Need consistent format/detail across multiple artifacts.

### Technique 7 (Advanced): Set Constraints
- **What**: Define boundaries (shall-only, testable, max length).
- **Why**: Forces precision and quality.
- **Prompt pattern**: "Write N requirements. Constraints: shall-only, quantitative, testable, no implementation." 
- **Use when**: Enforcing standards compliance (IEEE 29148, ASPICE).

### Technique 8 (Advanced): Ask for Alternatives
- **What**: Request multiple options with pros/cons.
- **Why**: Surfaces trade-offs.
- **Prompt pattern**: "Propose 3 options. For each: description, pros, cons, effort (1–5), best for."
- **Use when**: Making architecture or test strategy decisions; comparing approaches.

---

## Before/After Transformations (Common Fixes)

### Transformation 1: Vague → Specific (Technique 1)
**Before (vague):**  
"Write code for the project."

**After (Technique 1: Be Specific):**  
"Write a C++ class `MotorController` for Arduino UNO. Methods: `setSpeed(pwm)`, `stop()`, `detectStall()`. Use Arduino HAL (`digitalWrite`, `analogWrite`). Format: header comment + method signatures + inline comments."

**Why it works:** Added scope (class), platform (UNO), methods (3 named), HAL constraints, format.

### Transformation 2: Jargon → Context (Technique 2)
**Before (jargon dump):**  
"Generate ASPICE-compliant requirements."

**After (Technique 2: Provide Context):**  
"Generate 5 functional requirements in IEEE 29148 format.  
Format: REQ-ID, statement (with 'shall'), rationale, acceptance criteria.  
Derived from: SysReq-001 (desk motion control: 30–120cm in ≤30s, smooth, safe).  
Context: Arduino UNO, 12V motor, buttons, LIMIT switch."

**Why it works:** Specific standard, format details, upstream trace, hardware context.

### Transformation 3: Unstructured → Format (Technique 3)
**Before (no format):**  
"What's the architecture?"

**After (Technique 3: Request Format):**  
"What's the architecture? Present as:  
(1) Mermaid block diagram (components + data flow)  
(2) Module list with responsibility (1-line each)  
(3) Interfaces table (module | inputs | outputs | dependencies)"

**Why it works:** Three specific output formats, all parseable and reusable.

### Transformation 4: First-Try → Iterate (Technique 4)
**Before (accept blindly):**  
Ask once, copy result, done.

**After (Technique 4: Iterate):**  
Ask, review output for missing quantitative criteria, then: "Rewrite to add numbers (tolerances, timing), rationale, and pass/fail acceptance. Use 'shall' not 'should'."

**Why it works:** Feedback loop ensures quality before committing.

### Transformation 5: Orphan → Traceable (Technique 5)
**Before (no traceability):**  
"Write tests for the software."

**After (Technique 5: Request Traceability):**  
"Write 5 unit tests for the motor controller. Each test must reference which SWR it verifies.  

Software Requirements (given):  
- SWR-001: Motor PWM 0–255  
- SWR-002: Ramp 500ms  
- SWR-003: STOP ≤100ms  

Format: table with Test ID | SWR Link | Test Name | Expected Result."

**Why it works:** Explicit upstream artifacts, traceability mandate, structured output.

---

## Check Your Work (Self-Assessment)

After completing each mini-drill, verify your prompt against these criteria:

| Check | What to Look For | Pass = ✅ | Fail = 🔄 Iterate |
|-------|------------------|-----------|-------------------|
| **Specificity** | Numbers, constraints, context? | Includes ≥3 quantitative details | Vague or generic |
| **Format** | Output structure specified? | Table/code/diagram format named | No structure mentioned |
| **Traceability** | Links to upstream/downstream? | References SysReq/SWR/Test | Orphan artifacts |
| **Testability** | Can write pass/fail test? | Measurable acceptance criteria | Subjective terms (robust, good) |

**Good-enough threshold:** 3 of 4 checks pass → move on. If < 3, iterate using Technique 4.

**Example self-check:**  
Your prompt: "Write motor requirements."  
- Specificity: ❌ (no numbers)  
- Format: ❌ (no structure)  
- Traceability: ❌ (no upstream)  
- Testability: ❌ (too vague)  
**Result:** 0/4 → Iterate!

---

## Practice Checklist (Quick)

Before moving to the full desk automation project, verify you can:

- ✅ Write a specific prompt (includes numbers, context, constraints)
- ✅ Request a specific format (markdown table, C++ code block, diagram)
- ✅ Include an example to set quality expectations
- ✅ Add constraints (max words, "shall" not "should", testable, etc.)
- ✅ Request traceability (links to upstream/downstream artifacts)
- ✅ Iterate on AI output (refine, ask for alternatives)
- ✅ Identify prompting anti-patterns in your own questions

If any item feels unclear, reread that section or try the mini-drill again.

---

## Key Takeaways

- ✅ **Prompting is a skill:** It improves with practice and iteration.
- ✅ **Specificity beats vagueness:** Numbers, context, constraints → better outputs.
- ✅ **Format matters:** Specify output structure (table, code, diagram).
- ✅ **Examples set the bar:** Show what "good" looks like.
- ✅ **Iteration refines:** First output is rarely final.
- ✅ **Traceability prevents mistakes:** Every artifact should link to why it exists.
- ✅ **Multiple options expose trade-offs:** Never settle for one design without exploring others.

---

## What's Next?

Next session: [Session 4 – Desk Automation Full SysEng Cycle](04_00_deskautomation-syseng.md)

In that session, you'll apply all 8 prompting techniques to:
- Write a mission statement and system objectives
- Define requirements (functional, non-functional, safety)
- Design architecture and implementation
- Generate and validate code
- Write unit and integration tests
- Build a traceability matrix
- Self-assess against ASPICE process levels

---

## Practice Prompts (Try These!)

### Prompt 1: Specific Motor Requirement
**Your prompt:**
```
Write a functional requirement for motor speed control. Context: 
12V brushed DC motor, L298N driver, Arduino UNO PWM 0–255. Speed shall 
be proportional to PWM. Use IEEE 29148 format (REQ-ID, statement, 
rationale, acceptance criteria).
```

**Expected AI output (excerpt):**
```
REQ-MOT-001: The motor controller shall accept PWM duty cycle values 
in the range 0–255, where 0 = 0% duty (motor off) and 255 = 100% duty (max speed).

Rationale: PWM control via Arduino analogWrite() uses 8-bit resolution (0–255).

Acceptance Criteria:
- PWM input 0 → motor voltage = 0V ±0.1V
- PWM input 255 → motor voltage = 12V ±0.2V
- PWM input 128 → motor voltage = 6V ±0.2V
- Response time: PWM change → voltage change ≤10ms
```

**Self-check:** Does output include REQ-ID, "shall" statement, rationale, quantitative acceptance? If not, iterate.

### Prompt 2: Format-Requested Test Matrix
**Your prompt:**
```
Create a traceability matrix: Software Requirements → Unit Tests. 
Format: Markdown table (SWR ID | SWR Statement | Test ID | Test Name | 
Expected). Include ≥ 3 SWRs and ≥ 1 test per SWR.
```

**Expected AI output (excerpt):**
```markdown
| SWR ID | SWR Statement | Test ID | Test Name | Expected Result |
|--------|---------------|---------|-----------|------------------|
| SWR-001| Motor accepts PWM 0–255 | TEST-001 | Verify PWM range | Voltage 0–12V linear with PWM |
| SWR-001| Motor accepts PWM 0–255 | TEST-002 | Verify PWM clamp | PWM > 255 clamped to 255 |
| SWR-002| Ramp speed 500ms | TEST-003 | Measure ramp time | 500 ±50ms from 0 to max |
| SWR-003| STOP ≤100ms | TEST-004 | STOP latency | Motor off within 100ms |
```

**Self-check:** Correct columns? 1–2 tests per SWR? Pass/fail criteria clear?

### Prompt 3: Example-Driven Architecture
```
Design a motor controller architecture. Follow this pattern:

EXAMPLE LAYER:
HAL (Hardware Abstraction)
  - Input: button press → derived state
  - Output: PWM command → motor voltage

Now design for: buttons (UP/DOWN/STOP), motor (12V), limit switch, 
emergency stop. Use 3–4 layers. ASCII art + 1–2 line description per layer.
```

### Prompt 4: Constraint-Based Code Generation
```
Write a C++ method setMotorSpeed(pwm) for an Arduino motor controller.

Constraints:
- Input: 0–255 PWM value
- Use digitalWrite() and analogWrite()
- Include safety check: if pwm > 255, clamp to 255
- Include comment block (purpose, params, returns)
- Max 10 lines of code

Show complete method with error handling.
```

---

## Additional Resources

- **IEEE 29148 Requirements Handbook:** [ieee.org](https://ieee.org) (search "29148")
- **ASPICE 4.0 Process Guide:** [process-guide.automotivesoft.org](https://process-guide.automotivesoft.org)
- **AI Prompt Engineering Guide:** [platform.openai.com/docs/guides/prompt-engineering](https://platform.openai.com/docs/guides/prompt-engineering)
- **Worked Examples Repository:** See [00_context/examples](../00_context/examples) for additional prompts.


