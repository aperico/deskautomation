# Session 4: Desk Automation – Full SysEng Cycle (Index)

**Total Time:** ~60–90 minutes  

**Focus:** Complete V-Model systems engineering: Mission → Requirements → Architecture → Implementation → Verification → Validation

---

## Overview

This session guides you through a **complete systems engineering lifecycle** for automating a standing desk using an Arduino UNO, motor driver, and push-button controls. You'll apply **top-down design** principles (mission → objectives → requirements → architecture → code) and validate the system against all requirements.

**Real-world scenario:** Replace a manual desk crank with motorized UP/DOWN/STOP buttons that operate smoothly, safely, and responsively.

**Outcome:** A fully traceable, tested system where every line of code links back to a requirement, and every requirement traces to the mission.

---

## Prerequisites

- Completed Sessions 1–3 (LED blink, toolchain, prompting techniques)
- Hardware parts: Working Arduino UNO with motor driver, buttons, limit switches
- VS Code + Copilot + Arduino IDE + Google Test installed

###  Setting up Agent Rules

Prompt:

```
Operate following these rules:

- Role: Embedded Software System Specialist for motorizing an Ikea desk (replace manual crank with motorized up/down buttons).

- Goals: Deliver safe, traceable, maintainable, minimalistic, standards-compliant artifacts (requirements, architecture, tests) that stay at the correct abstraction level.

- Inputs to request (up front): Mission statement, system objectives, use cases, hardware list, constraints (safety, timing, power), and any existing requirements or diagrams.

- Guardrails: Follow ISO 25119, ASPICE 4, and relevant IEEE safety-critical practices; avoid downstream/implementation leakage in higher-level docs; keep traceability intact.

- Outputs to produce: Markdown artifacts with clear IDs, rationales, acceptance criteria, and upstream links; concise plans before drafting; options with pros/cons when making design choices.

- Working style: Restate objectives; surface assumptions; ask for missing context before drafting; propose a short plan; keep language concise and testable; highlight open risks and verification approach; employ systems engineering "top-down" approach.

- Review discipline: Remove downstream references from higher-level docs (e.g., SWE-REQ in system-level docs, implementation details in requirements) and maintain abstraction layers: System Context -> System Requirements -> Software Requirements -> Architecture -> Code.

```

## Project Context & Problem Statement

**Prompt** (copy and paste to Copilot):

```
For the desk automation project, note the following:

PROBLEM STATEMENT:
We need to automate a manual standing desk. The desk is currently 
operated by a hand crank and needs to be converted to motorized control.

HARDWARE AVAILABLE:
- Standing Desk: Height range 30–120 cm, max load 20 kg
- Motor: 12V brushed DC motor with gearbox, nominal 2A current
- Motor Driver: L298N H-bridge (PWM input 0–255, output ±12V)
- Microcontroller: Arduino UNO (ATmega328P, 16 MHz, 2 KB RAM)
- User Controls: 3 push buttons (UP, DOWN, STOP)
- Safety: 1 limit switch (at desk top / max height)
- Feedback: 1 LED for status indication

CONSTRAINTS:
- Desk height: 30–120 cm range
- Load capacity: ≤ 20 kg
- Lift time: ≤ 30 seconds for full stroke (sitting → standing)
- User response: Button press → motion within ≤ 1 second
- Emergency stop: STOP button halts motion within ≤ 100 ms
- Motor stall detection: Detect jam/blockage within 5 seconds
- Button debounce: 50 ms hysteresis (eliminate noise/chatter)

GOAL:
Build a safe, traceable, testable, maintainable system that:
1. Raises and lowers the desk smoothly under user control
2. Responds immediately to button presses (UP, DOWN, STOP)
3. Enforces height limits via mechanical limit switch
4. Detects motor stall and enters safe FAULT state
5. Has clear LED status indication (green=ready, amber=moving, red=fault)

```


## Session Structure

1. [System & Software Definitions](04_01_00_system-definition.md)
2. [Software Architecture](04_01_01_system-definition-architecture.md)
3. [Software Construction](04_02_software-construction.md)
4. [Software Component Verification](04_03_00_component-verification.md)
5. [Software Integration Verification](04_03_01_software_integration-verification.md)
6. [System Testing](04_03_03_system-testing.md)
