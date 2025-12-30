# Automated Mechanical Desk Lift  
**Arduino-Based Control System**

## Overview
This project implements a **motorized height adjustment system** for a mechanical desk using an **Arduino-based controller**.  
The system replaces manual adjustment with an electric drive while emphasizing **safety**, **modularity**, and **incremental development**.

The architecture explicitly supports:
- Temporary hardware for early development (JGY370 + L298N)
- Final hardware for production use (high-torque worm gear motor + BTS7960)
- Future extensibility (limit switches, current sensing, height presets)

The software is structured to allow **hardware replacement without rewriting control logic**.

---

## Project Goals
- Electrically raise and lower a mechanical desk
- Enable safe early prototyping without limit switches
- Abstract hardware dependencies via a HAL
- Allow host-based unit testing using `g++`

---

## Out of Scope (Initial Phase)
- Automatic height presets
- User profiles
- Wireless or network connectivity
- Closed-loop position control

---

## System Actors

| Actor | Description |
|------|------------|
| User | Operates the desk |
| Arduino Controller | Executes control logic |
| DC Motor | Provides mechanical movement |
| Motor Driver | Controls motor direction and power |
| Power Source | Battery or external supply |

---

## System Use Cases

### UC-01: Power the Desk Control System
**Primary Actor:** User

**Description:**  
The user supplies power to the motor driver via a physical ON/OFF switch.

**Preconditions:**
- Battery is connected
- Wiring is correct

**Main Flow:**
1. User toggles the power switch to ON
2. Motor driver receives power
3. Arduino powers up (USB or regulated supply)

**Postconditions:**
- System is powered and idle

**Safety Notes:**
- Power switch is placed **between the battery and the motor driver**
- Motor must not start automatically on power-up

---

### UC-02: Raise Desk
**Primary Actor:** User

**Description:**  
The user commands the desk to move upward.

**Preconditions:**
- System is powered
- No movement is currently active

**Main Flow:**
1. User issues an “Up” command
2. Arduino sets motor direction to UP
3. Motor driver energizes the motor
4. Desk moves upward

**Alternate Flow (Timeout Protection):**
- If movement exceeds the configured maximum duration, the motor is stopped automatically

**Postconditions:**
- Desk height is increased
- Motor is stopped safely

---

### UC-03: Lower Desk
**Primary Actor:** User

**Description:**  
The user commands the desk to move downward.

**Preconditions:**
- System is powered
- Desk is not assumed to be at its lowest position

**Main Flow:**
1. User issues a “Down” command
2. Arduino sets motor direction to DOWN
3. Motor driver energizes the motor
4. Desk moves downward

**Postconditions:**
- Desk height is decreased
- Motor is stopped safely

---

### UC-04: Emergency Stop (Software-Based)
**Primary Actor:** System

**Description:**  
The system automatically stops the motor to prevent damage or unsafe operation.

**Trigger Conditions:**
- Maximum run-time exceeded
- Invalid state detected
- Unexpected reset

**Main Flow:**
1. Arduino disables motor driver outputs
2. Motor stops immediately

**Postconditions:**
- Desk remains in its current position
- Error is latched until recovery conditions are met (see UC-09)

---

### UC-05: Hardware Upgrade Without Software Rewrite
**Primary Actor:** Developer

**Description:**  
Replace temporary hardware with final motor and driver.

**Preconditions:**
- Hardware Abstraction Layer (HAL) implemented
- Pin configuration isolated from logic

**Main Flow:**
1. Replace motor driver (L298N → BTS7960)
2. Update pin mappings
3. Reuse existing control logic

**Postconditions:**
- System operates with new hardware
- No user-visible behavior changes

---

### UC-06: Host-Based Unit Testing
**Primary Actor:** Developer

**Description:**  
Validate control logic without Arduino hardware.

**Preconditions:**
- HAL abstraction implemented
- Host toolchain (`g++`) available

**Main Flow:**
1. Compile logic for host environment
2. Mock Arduino HAL functions
3. Execute unit tests

**Postconditions:**
- Logic validated independently of hardware
- Reduced regression risk

---

### UC-07: Dual Button Handling (Up Priority)
**Primary Actor:** User

**Description:**  
When both Up and Down commands are pressed simultaneously, Up has priority and motion is upward if allowed.

**Preconditions:**
- System is powered
- Neither limit prevents the chosen direction

**Main Flow:**
1. Both buttons are pressed
2. Controller prioritizes Up
3. Motor moves upward

**Postconditions:**
- Upward movement begins (unless blocked by limits)

---

### UC-08: Limit Lockout Behavior
**Primary Actor:** System

**Description:**  
Movement must not initiate in the direction of an active limit, and if both limits are reported active, no movement is allowed.

**Preconditions:**
- System is powered
- Limit signals may be active

**Main Flow:**
1. User requests motion toward an active limit
2. Controller blocks movement and remains idle

**Alternate Flow:**
- If both upper and lower limits are active, all movement is blocked

**Postconditions:**
- No motion is commanded; system remains idle

---

## Key Design Constraints
- Initial operation without limit switches
- Motor must stop on reset or fault
- Direction changes require a stop before reversal
- Motor must never start automatically on power-up

---

## Planned Enhancements
- Upper and lower limit switches
- Current sensing for stall detection
- Height presets
- EEPROM-based calibration
- Soft-start and soft-stop PWM control

---

## Wiring Diagrams

### Safety Notes
- Always disconnect power before wiring
- Motor power must **never** pass through the Arduino
- The ON/OFF switch must be placed **between the battery and the motor driver**
- Arduino GND and motor driver GND **must be common**

---


## Wiring Diagram (Final Hardware)

### Components
- Arduino UNO
- BTS7960 motor driver
- NFP-5840 / 31ZYS-D worm gear motor
- 12V–24V battery
- Elitek ON/OFF switch

### Conceptual Wiring Diagram

```text
Battery (+)
|
|----[ Elitek Switch ]----> BTS7960 B+
Battery (-) ----------------> BTS7960 B-
|
+---- Arduino GND

Arduino D5 (PWM) ----------> RPWM
Arduino D6 (PWM) ----------> LPWM
Arduino D7 ----------------> R_EN
Arduino D8 ----------------> L_EN

BTS7960 M+ ----------------> Motor +
BTS7960 M- ----------------> Motor -
```

#### Power
| From | To |
|----|----|
| Battery + | Elitek switch input |
| Elitek switch output | BTS7960 B+ |
| Battery − | BTS7960 B− |
| BTS7960 B− | Arduino GND |

#### Control Signals
| Arduino | BTS7960 |
|----|----|
| D5 (PWM) | RPWM |
| D6 (PWM) | LPWM |
| D7 | R_EN |
| D8 | L_EN |

#### Motor Output
| BTS7960 | Motor |
|----|----|
| M+ | + |
| M− | − |

### Notes
- Ensure a common ground between Arduino and BTS7960.
- Use appropriately sized wiring and a fuse on battery +.
- Do not power the motor from Arduino 5V; motor power is through BTS7960.

---

## Use Case ↔ Unit Test Traceability

| Use Case | Test(s) |
|---|---|
| UC-01: Power the Desk Control System | DeskAppTest.UC01_Power_IdleStop_NoError; DeskAppTest.UC01_NoButtonsPressed_IdleNoMovement |
| UC-02: Raise Desk | DeskAppTest.UC02_UpPressed_MovesUp_WhenNotAtUpperLimit; DeskAppTest.UC02_UpPressed_DoesNotMoveUp_WhenAtUpperLimit |
| UC-03: Lower Desk | DeskAppTest.UC03_DownPressed_MovesDown_WhenNotAtLowerLimit; DeskAppTest.UC03_DownPressed_DoesNotMoveDown_WhenAtLowerLimit |
| UC-04: Emergency Stop (Software-Based) | DeskAppTest.UC04_EmergencyStop_FromUp_WhenLowerLimitActive; DeskAppTest.UC04_EmergencyStop_FromDown_WhenUpperLimitActive |
| UC-09: Error Recovery (Latched → Idle) | DeskAppTest.UC09_ErrorRecovery_ToIdle_WhenSafe |
| UC-05: Hardware Upgrade Without Software Rewrite | Not directly unit-testable; covered by HAL abstraction and pin documentation |
| UC-06: Host-Based Unit Testing | SmokeTest.BasicTruth |
| UC-07: Dual Button Handling (Up Priority) | DeskAppTest.UC07_BothButtonsPressed_UpHasPriority_WhenNotAtUpperLimit |
| UC-08: Limit Lockout Behavior | DeskAppTest.UC08_BothButtonsPressed_NoMovement_WhenAtBothLimits |
---

## Pin Assignments (Current Code)

The current HAL targets the temporary L298N driver. Pin constants are defined in [source/arduino/HAL.h](source/arduino/HAL.h):

| Signal | Arduino Pin | Notes |
|---|---|---|
| `ERROR_LED` | D13 | On-board LED pin |
| `MOVING_UP_LED` | D12 | Status indicator |
| `MOVING_DOWN_LED` | D11 | Status indicator |
| `IN1` | D8 | Motor direction (L298N IN1) |
| `IN2` | D9 | Motor direction (L298N IN2) |
| `ENA` | D10 (PWM) | Motor speed (L298N ENA) |

UNO PWM-capable pins: D3, D5, D6, D9, D10, D11.

### Migration Notes (BTS7960)
- Recommended mapping for BTS7960 in this project:
	- `D5` → `RPWM`
	- `D6` → `LPWM`
	- `D7` → `R_EN`
	- `D8` → `L_EN`
- Ensure common ground between Arduino and BTS7960.
- Update HAL constants to match BTS7960 if/when migrating.


## Architecture

- **PinConfig.h**  
  Centralizes all Arduino pin assignments for LEDs, buttons, and the motor driver.  
  *Edit this file to change hardware pin mappings.*

- **HAL.h / HAL.cpp**  
  Hardware Abstraction Layer. Provides functions to initialize and control hardware components (LEDs, buttons, motor driver) without exposing low-level details to the main application logic.

- **DeskController.h / DeskController.cpp**  
  Contains the main application logic and state machine for desk movement. Processes user inputs and determines outputs for hardware control.

- **arduino.ino**  
  Entry point for the application. Handles setup, main loop, and high-level control flow. Reads inputs, runs application logic, and updates outputs.

---

## Hardware Connections

| Function                | Arduino Pin |
|-------------------------|-------------|
| Error Indicator LED     | 13          |
| Left Position LED       | 5           |
| Right Position LED      | 4           |
| Button Up               | 3           |
| Button Down             | 2           |
| Motor IN1               | 8           |
| Motor IN2               | 9           |
| Motor ENA (PWM)         | 10          |

---


3. **Operation:**  
   - Use the UP and DOWN buttons to control desk movement.
   - LEDs indicate desk position and errors.



---

## Code Structure

- **Modular Design:**  
  Hardware, logic, and configuration are separated for clarity and maintainability.

- **State Management:**  
  Uses input/output structs to pass data between layers.

- **Non-blocking Loop:**  
  The main loop is designed to be responsive and non-blocking.

---