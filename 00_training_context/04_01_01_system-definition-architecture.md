# Session 4.1.1: Software Architecture Design

**Time:** ~15 minutes

---

## Part 1: Software Architecture Design

Design the **high-level software architecture:** which components do what, how they communicate, and the state machine.

**Prompt:**
```
Design the software architecture for the desk motor controller.

Context:
- Derive from upstream documentation, especially software Requirements and hardware constraints.

Design criteria:
- Testable: Core application logic separate from hardware (HAL)
- Simple: Suitable for resource-constrained Arduino
- Safe: State machine enforces safety rules
- Real-time: Button response ≤ 1 sec

Propose a 3–4 layer architecture:

EXAMPLE LAYER STRUCTURE:
1. HAL (Hardware Abstraction Layer)
   - GPIO read/write wrappers (buttons, limit switch, LED)
   - PWM output wrapper
   - Timing utilities
   - Conversion tables and function (e.g.: analog inputs to logical inputs)
   - Motor Controller
      - setSpeed(pwm): Request motor speed
      - getRampProfile(): Calculate smooth acceleration

3. Desk Application Control APP
   - state: IDLE, RAISING, LOWERING, FAULT
   - APP_Init(...): setup pre-run and initialization
   - APP_Task(&inputs, &outputs): process tick from application state
      - It shall gather inputs and outputs as I/O structs
      - call state machine 
      - agnostic to hardware - no references to underlying RTOS or hardware-dependent functions
   - needs to run as stand-alone application/library

4. Main Loop / Signal Layer
   - Poll buttons → derive input → populate input structure
   - Call App task (passing input structure and output structure) 
   - Call HAL based functions based on the output values / structure
   - Update LED based on state
   - Command Motor as needed

Now design for the desk lift. Provide:
1. Layer names and responsibilities
2. Key public methods/functions per layer
3. One-sentence description per layer
4. Data flow (input → layer → output)
5. Sequence diagram for each use case
6. Document interfaces inputs and outputs, including data types and value ranges, and architecture decisions.
7. Architectural elements are based on System and Requirements needs.
8. Create a traceability table showing relationship between SYS, SW requirement to Architectural elements.
9. GPIO pin definitions in a separated file
10. APP Layer API has 2 public functions APP_Init(...) and APP_Task(&inputs, &outputs)
11. Document clear software APIs for each layer
12. HAL is the only module with access to low-level OS-like calls.

Format: Mermaid UML diagrams showing layers, state machines, and sequence diagrams and data flows.
Store software architecture under documentation directory
```

**Review:**
- Are the abstraction layers correct?
- Are the diagrams easy to understand and render correctly?
- Are all elements of the architecture traceable to requirements?

## Part 2: State Machine Design

If needed, request a state machine to be implemented with more details.

**Prompt:**
```
Design a state machine for the desk motor controller.

States: IDLE, RAISING, LOWERING, FAULT

Inputs:
- Button UP pressed
- Button DOWN pressed
- Button STOP pressed
- Limit switch triggered (desk at top)
- Motor stall detected (no motion for 5 sec)

For each state, specify:
1. Entry action (what happens when entering state)
2. State behavior (what happens while in state)
3. Exit action (what happens when leaving)
4. Transitions (which input → next state)

Format: State transition table or Mermaid diagram.
Include: all states, all inputs, safety rules (e.g., STOP always goes to IDLE) - add state machine documentation within software architecture document.
```

## Part 3: Sequence Diagrams

**Why Sequence Diagrams?** They visualize how the architecture layers interact in time. 
They answer: "What order do functions get called?" and "Do components interact correctly?"

**Prompt (Copy & Paste):**

```
TASK: Generate architecture-level Mermaid sequence diagrams for standing desk automation - consider the top 5 most important functional system requirements.

Reference upstream documentation

SEQUENCE DIAGRAMS (create 1 for each system requirement):

FORMAT REQUIREMENTS:
- Use Mermaid 'sequenceDiagram' syntax
- Message types: → (call), -->> (return), -x (async/fire-and-forget)
- Participant order (left to right): User, Main Loop, DeskApp, MotorController, HAL, Hardware elements
- Timeline annotations: use 'autonumber' and timing notes (e.g., '+50ms', '+500ms')
- Add 'Note over' blocks for state transitions and PWM values
- Include all function calls: processInput(), executeState(), setTargetSpeed(), getRampedSpeed(), setMotorPWM(), setLED()

VALIDATION (after generation):
 - All generated diagrams use proper Mermaid syntax (render without errors)
 - Each diagram covers 1 system requirement (traced in documentation)
 - Participants match your 4-layer architecture
 - Timing annotations match SWReq constraints (debounce 50ms, ramp 500±50ms, - STOP ≤100ms, stall 2sec)
 - State transitions deterministic (no undefined paths)
 - PWM values realistic (0–255 linear profiles for ramping)
 - LED status reflects state changes (green→raising/lowering, amber→stopping, red→fault)
 - Motor direction (RAISE vs LOWER) handled correctly per controller design

STORAGE: Save diagrams in software architecture document with brief caption per diagram and a traceability table linking sequences → SysReq coverage.
```

**After generating diagrams:**
- Review: Do the sequences match your state machine transitions?
- Verify: Do function calls align with your layer architecture?
- Validate: All sequences cover the critical paths; no gaps in fault handling or safety scenarios?


---

## Key Takeaways

- **Top-down approach:** Start with mission (why), then objectives (what), then requirements (how much), then architecture (how)
- **Traceability is essential:** Every requirement traces to objective; every architecture element traces to requirement
- **Abstraction layers matter:** System requirements stay hardware-agnostic; software requirements stay implementation-agnostic
- **State machine provides safety:** Deterministic behavior with well-defined transitions
- **AI accelerates drafting:** But human validation ensures correctness and completeness

---

**Next Step:** Proceed to [04_02_software-construction.md](04_02_system-construction.md) for Detailed Design and Construction.
