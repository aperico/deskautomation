# Concept of Operations (ConOps)

## Overview

This document describes operational scenarios for the Standing Desk Automation System, illustrating how users interact with the system during normal and emergency conditions. These scenarios guide system and software requirements development.

---

## Traceability

**Derived from:**
- [00_SystemMissionStatement.md](00_SystemMissionStatement.md)
- [01_SystemObjectives.md](01_SystemObjectives.md)

---

## Operational Context

**Users:** Office workers in commercial building environments  
**Operating Range:** Desk height adjustment between 30 cm (sitting) and 120 cm (standing)  
**Controls:** Two physical buttons (UP, DOWN)  
**Control Method:** Motion continues while button is held; motion stops when button is released  
**Safety Constraint:** System must halt motion immediately upon button release or hazardous condition detection  
**Normal Usage:** Multiple height adjustments per day over product lifetime

---

## Operational Scenarios

### Scenario 1: Normal Desk Raise (Sitting → Standing)

**Objective Coverage:** OBJ-001, OBJ-002, OBJ-004, OBJ-005

**Initial State:**
- Desk height: 75 cm (seated position)
- Load: 15 kg (monitor, keyboard, documents)
- All buttons: Released
- System: IDLE, ready for commands

**User Actions:**
1. User presses and holds UP button
2. User observes desk rising smoothly
3. When desk reaches desired height (110 cm), user releases UP button

**System Response:**
1. Within 1 second of UP button press, motor activates and desk begins rising
2. Desk moves upward smoothly at constant speed (~3 cm/sec)
3. Motion continues while UP button is held
4. When button is released, motor stops within 500 ms
5. System returns to IDLE state

**Final State:**
- Desk height: 110 cm (standing position)
- All buttons: Released
- System: IDLE, ready for next command

**Success Criteria:**
- Response time ≤ 1 sec from button press to motion start
- Smooth, continuous motion without jerking
- Desk reaches new height in ~12 seconds (35 cm travel)
- No overshoot or oscillation after button release

---

### Scenario 2: Normal Desk Lower (Standing → Sitting)

**Objective Coverage:** OBJ-001, OBJ-002, OBJ-004, OBJ-005

**Initial State:**
- Desk height: 110 cm (standing position)
- Load: 15 kg
- All buttons: Released
- System: IDLE, ready for commands

**User Actions:**
1. User presses and holds DOWN button
2. User observes desk lowering smoothly
3. When desk reaches desired height (75 cm), user releases DOWN button

**System Response:**
1. Within 1 second of DOWN button press, motor activates and desk begins lowering
2. Desk moves downward smoothly at constant speed (~3 cm/sec)
3. Motion continues while DOWN button is held
4. When button is released, motor stops within 500 ms
5. System returns to IDLE state

**Final State:**
- Desk height: 75 cm (seated position)
- All buttons: Released
- System: IDLE, ready for next command

**Success Criteria:**
- Response time ≤ 1 sec from button press to motion start
- Smooth, continuous motion without jerking
- Desk reaches new height in ~12 seconds (35 cm travel)
- No overshoot or oscillation after button release

---

### Scenario 3: Motion Halt During Upward Travel

**Objective Coverage:** OBJ-003

**Initial State:**
- Desk height: 60 cm (mid-range, rising)
- Desk motion: Moving upward at normal speed
- UP button: Pressed (held by user)
- System: MOVING_UP state

**User Actions:**
1. User observes unexpected object in path or decides to halt motion
2. User releases UP button

**System Response:**
1. Within 500 ms of button release:
   - Motor power is immediately cut
   - Desk motion halts
   - All upward motion ceases
2. System enters IDLE state
3. System is ready to accept new commands

**Final State:**
- Desk height: ~61 cm (stopped mid-travel with minimal coasting)
- All buttons: Released
- System: IDLE (motion halted, ready for next command)

**Success Criteria:**
- Total stop time ≤ 500 ms from button release to full motor halt
- Coasting distance ≤ 1 cm
- No residual motion after motor halt
- System immediately ready to accept new button press

---

### Scenario 4: Full-Stroke Height Adjustment (Minimum → Maximum)

**Objective Coverage:** OBJ-001, OBJ-004

**Initial State:**
- Desk height: 30 cm (minimum height, fully lowered)
- Load: 20 kg (maximum rated load)
- All buttons: Released
- System: IDLE

**User Actions:**
1. User presses and holds UP button
2. User maintains button press for full stroke duration
3. Desk reaches maximum height (120 cm)
4. User releases UP button

**System Response:**
1. Within 1 second, motor activates and desk begins rising
2. Desk continues smooth upward motion throughout 90 cm stroke
3. After ~30 seconds, desk reaches 120 cm (upper mechanical limit)
4. System automatically stops motor at limit (even if button still held)
5. System returns to IDLE state

**Final State:**
- Desk height: 120 cm (maximum height)
- All buttons: Released
- System: IDLE

**Success Criteria:**
- Full 90 cm stroke completed in ≤ 30 seconds
- Smooth operation throughout entire range with 20 kg load
- Automatic stop at mechanical limit
- No mechanical binding or strain

---

### Scenario 5: Simultaneous Button Press (Safety Edge Case)

**Objective Coverage:** OBJ-003 (safety through well-defined behavior)

**Initial State:**
- Desk height: 75 cm (mid-range)
- All buttons: Released
- System: IDLE

**User Actions:**
1. User accidentally presses both UP and DOWN buttons simultaneously

**System Response:**
1. System detects conflicting inputs
2. System does NOT activate motor (treats as invalid/unsafe command)
3. System remains in IDLE state
4. System waits for valid single-button input

**Final State:**
- Desk height: 75 cm (unchanged)
- UP and DOWN buttons: Both pressed
- System: IDLE (motor not activated)

**Success Criteria:**
- No motor activation during conflicting inputs
- System does not enter undefined state
- Normal operation resumes when valid single button pressed

---

## Operational Constraints

1. **Continuous Press Required:** Motion continues only while button is actively held
2. **Single-Direction Operation:** Only one direction of motion at a time; simultaneous UP+DOWN presses ignored
3. **Immediate Halt:** Motor stops within 500 ms of button release
4. **Limit Protection:** System stops automatically at mechanical limits (30 cm and 120 cm)
5. **Load Handling:** All scenarios assume load ≤ 20 kg; system behavior undefined above rated load

---

## Scenario Coverage Matrix

| Scenario | OBJ-001 | OBJ-002 | OBJ-003 | OBJ-004 | OBJ-005 |
|----------|---------|---------|---------|---------|---------|
| Scenario 1: Normal Raise | ✓ | ✓ | — | ✓ | ✓ |
| Scenario 2: Normal Lower | ✓ | ✓ | — | ✓ | ✓ |
| Scenario 3: Motion Halt | — | — | ✓ | — | — |
| Scenario 4: Full Stroke | ✓ | — | — | ✓ | — |
| Scenario 5: Simultaneous Press | — | — | ✓ | — | — |

**Coverage Analysis:**
- All 5 system objectives covered by at least one scenario
- Safety objective (OBJ-003) covered by 2 scenarios
- Normal operation scenarios validate responsiveness and smoothness
- Edge cases (full stroke, conflicts) validate robustness

---

**Document Version:** 1.0  
**Date:** January 24, 2026  
**Status:** Approved  
