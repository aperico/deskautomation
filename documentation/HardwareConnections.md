
# Hardware Connections & Pin Assignments

This guide details the hardware setup for the Automated Mechanical Desk Lift system. Follow these instructions to assemble, wire, and configure your desk controller safely and correctly.

---

## Required Components
- L298N motor driver
- JGY370 DC motor (12V)
- 9V–12V battery
- Elitek ON/OFF switch
- Arduino UNO (or compatible)
- Indicator LEDs (Error, Left, Right)
- Push buttons (Up, Down)

---


## Pin Mapping Overview
See [Software Architecture](SoftwareArchitecture.md#inputs-and-outputs-table) for a full overview of pin assignments and architecture. Edit [`PinConfig.h`](../source/arduino/PinConfig.h) to change hardware pin mappings.

---

## Pin Assignments (Current Code)


| Signal          | Arduino Pin | Notes                |
|-----------------|-------------|----------------------|
| ERROR_LED       | 13          | On-board LED         |
| LED_LEFT_PIN    | 5           | Left position LED    |
| LED_RIGHT_PIN   | 4           | Right position LED   |
| BUTTON_UP_PIN   | 3           | Up button            |
| BUTTON_DOWN_PIN | 2           | Down button          |
| IN1             | 8           | Motor direction 1    |
| IN2             | 9           | Motor direction 2    |
| ENA             | 10 (PWM)    | Motor speed (PWM)    |

---

## Wiring & Safety Guidance

### Safety Notes
- **Always disconnect power before wiring.**
- Motor power must **never** pass through the Arduino.
- The ON/OFF switch must be placed **between the battery and the motor driver**.
- Arduino GND and motor driver GND **must be common**.

### Wiring Diagrams
Refer to the diagrams below and in the project documentation for safe and correct wiring. If you add new hardware, update this file and the diagrams accordingly.

---

*If you have questions about hardware setup, open an issue or consult the project maintainers.*
