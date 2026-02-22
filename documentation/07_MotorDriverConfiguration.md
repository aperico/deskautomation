# Motor Driver Configuration Guide

## Overview

The Automated Desk Lift System supports two configurable motor driver types:

| Aspect | L298N (MT_BASIC) | IBT_2 (MT_ROBUST) |
|--------|-----------------|-------------------|
| **Driver Type** | Dual H-Bridge | Intelligent H-Bridge |
| **Cost** | Low cost (~$2-5 USD) | Moderate cost (~$10-15 USD) |
| **Max Current** | 2A sustained | 5-10A sustained |
| **Pin Control** | 3 pins (EN1, EN2, PWM) | 2 pins (LPWM, RPWM) |
| **Direction Mode** | Enable pins (mutually exclusive) | PWM ratio control |
| **Efficiency** | ~80% | ~92% |
| **Thermal Protection** | None (manual) | Integrated |
| **Dead-Time** | Manual in firmware | Hardware + firmware |
| **Soft-Start** | Software only | Hardware + software |
| **Diagnostic Output** | None | Current input (CIN) pin |

## Motor Configurations

### Basic System (MT_BASIC) - L298N
- **Motor Driver:** L298N Dual H-Bridge
- **GearBox Motor:** Standard 12V DC motor
- **Application:** Cost-optimized standing desk for general office use
- **Features:**
  - Proven, well-documented design
  - Simple control logic (3 GPIO pins)
  - Adequate for normal desk lifting (up to ~50kg)
  - No integrated thermal protection

**Schematic:**
```
    +5V ──┐
          │ Arduino
    ┌─────────────────────────────┐
    │    Digital/PWM Pins         │
    │  EN1(6)  EN2(7)  PWM(9)     │
    │    ↓        ↓        ↓       │
    └────┼────────┼────────┼──────┘
         │        │        │
         └────────┼────────┤
                  │        │
              ┌───┴────┐   │
              │ L298N  │───┘
              │ Driver │
              └───┬────┘
                  │
              Motor (12V)
```

**Pin Assignments (MT_BASIC):**
- PIN_MOTOR_EN1 = 6  (Enable UP direction)
- PIN_MOTOR_EN2 = 7  (Enable DOWN direction)
- PIN_MOTOR_PWM = 9  (PWM speed control)

**Control Logic (MT_BASIC):**
```
Mode        │ EN1 │ EN2 │ PWM │ Motor State
─────────────┼─────┼─────┼─────┼─────────────
MOTOR_UP    │ HIGH│ LOW │ 0-255│ Rotates UP with speed PWM
MOTOR_DOWN  │ LOW │ HIGH│ 0-255│ Rotates DOWN with speed PWM
MOTOR_STOP  │ LOW │ LOW │ any │ Coasts to stop
```

### Robust System (MT_ROBUST) - IBT_2
- **Motor Driver:** IBT_2 Intelligent Motor Driver
- **GearBox Motor:** 31ZY05840 12V DC geared motor
- **Application:** High-capacity standing desk for heavy-duty use
- **Features:**
  - Advanced PWM ratio control
  - Integrated thermal protection
  - Integrated current diagnostics
  - Dead-time control hardware
  - Higher current capacity (5-10A)
  - Better EMI filtering

**Schematic:**
```
    +5V ──┐
          │ Arduino
    ┌─────────────────────────────┐
    │    PWM Pins                 │
    │  LPWM(9)    RPWM(10)        │
    │    ↓           ↓            │
    └────┼───────────┼────────────┘
         │           │
         ├───────┬───┘
                 │
             ┌───┴────┐
             │ IBT_2  │
             │ Driver │
             └───┬────┘
                 │
              Motor (12V)
                 
      CIN(A1) ← Current Diagnostics
```

**Pin Assignments (MT_ROBUST):**
- PIN_MOTOR_LPWM = 9   (Left PWM - counterclockwise/UP)
- PIN_MOTOR_RPWM = 10  (Right PWM - clockwise/DOWN)
- PIN_MOTOR_CIN = A1   (Current diagnostics input, optional)

**Control Logic (MT_ROBUST):**
```
Mode        │ LPWM │ RPWM │ Motor State
─────────────┼──────┼──────┼──────────────────────
MOTOR_UP    │ 255  │0-127 │ Rotates UP; speed = PWMS ratio
MOTOR_DOWN  │0-127 │ 255  │ Rotates DOWN; speed = PWM ratio
MOTOR_STOP  │ 0    │ 0    │ Stopped (hold)
```

Speed control is achieved by the PWM duty cycle ratio. When moving UP:
- LPWM = 255 (full)
- RPWM = 255 - speed (0-255)
- This creates a PWM ratio that controls motor speed

## Configuration Selection

### 1. Via motor_config.h (Compile-Time)

Edit `src/motor_config.h` line with `#define MOTOR_TYPE`:

```cpp
// For L298N (Basic) system:
#define MOTOR_TYPE MT_BASIC      // Default

// For IBT_2 (Robust) system:
#define MOTOR_TYPE MT_ROBUST
```

### 2. Via CMake Build Option

Configure at build time:

```bash
# Build for L298N (default)
cmake -S . -B build
cmake --build build --config Release

# Build for IBT_2
cmake -S . -B build -DMOTOR_TYPE=MT_ROBUST
cmake --build build --config Release
```

Or via VS Code tasks:
1. Edit `CMakeLists.txt` to add `-DMOTOR_TYPE=MT_ROBUST` to the cmake configure
2. Run "CMake: Clean" and "CMake: Configure" from VS Code command palette

### 3. Post-Build Configuration

After building, the active motor type is fixed in the binary. To change:

1. Edit `motor_config.h`: Change `#define MOTOR_TYPE` value
2. Clean build: Run "CMake: Clean" task
3. Rebuild: Run "CMake: Build" task
4. (Do NOT use rebuild; use clean + build)

## Pin Assignment Verification

### Verify Current Configuration

The active pin assignments can be verified at runtime:

```cpp
// In hal.h, the following are defined based on MOTOR_TYPE:

// If MT_BASIC:
const uint8_t PIN_MOTOR_EN1;   // = 6
const uint8_t PIN_MOTOR_EN2;   // = 7
const uint8_t PIN_MOTOR_PWM;   // = 9

// If MT_ROBUST:
const uint8_t PIN_MOTOR_LPWM;  // = 9
const uint8_t PIN_MOTOR_RPWM;  // = 10
const uint8_t PIN_MOTOR_CIN;   // = A1
```

### Expected Arduino Pinout

**All Motor Types:**
- Digital Pins 2-3: Buttons (UP, DOWN)
- Digital Pins 4-5: Limit switches (UPPER, LOWER)
- Analog Pin A0: Motor current sensing

**L298N (MT_BASIC):**
- Digital Pins 6-7: Motor enable (EN1, EN2)
- PWM Pin 9: Motor speed (PWM)
- Digital Pins 10-11-13: Status LEDs
- Analog Pin A0: Current sense

**IBT_2 (MT_ROBUST):**
- PWM Pins 9-10: Motor control (LPWM, RPWM)
- Analog Pin A1: Current diagnostics (optional)
- Digital Pins 10-11-13: Status LEDs
- Analog Pin A0: Current sense

## Safety Configuration

Both motor types use the same safety thresholds defined in `safety_config.h`:

```cpp
#define MOTOR_SENSE_THRESHOLD_MA 150U              // Stuck-on detection (100ms timeout)
#define MOTOR_SENSE_OBSTRUCTION_THRESHOLD_MA 200U  // Obstruction detection (immediate)
```

If the IBT_2 motor (31ZY05840) has significantly different current characteristics:

1. Verify actual stall current and normal operating current
2. Update thresholds in `safety_config.h` if needed
3. Re-test safety scenarios in test suite

## Hardware Changes Checklist

When switching from MT_BASIC to MT_ROBUST:

### Physical Changes to Arduino Board
- [ ] Connect LPWM (Arduino pin 9) to IBT_2 left PWM input
- [ ] Connect RPWM (Arduino pin 10) to IBT_2 right PWM input
- [ ] Disconnect old EN1 (pin 6) and EN2 (pin 7) connections if present
- [ ] Connect IBT_2 motor power terminals (verify polarity)
- [ ] Connect current sense line to A0 (same as before)
- [ ] Optionally: Connect CIN diagnostic pin to A1

### Firmware Changes
- [ ] Edit `motor_config.h`: Set `#define MOTOR_TYPE MT_ROBUST`
- [ ] Verify pin assignments in `pin_config.h` (should auto-select)
- [ ] Rebuild firmware
- [ ] Load onto Arduino
- [ ] Test motor movement in both directions
- [ ] Verify no compile errors

## Testing Both Configurations

### Full Test Suite for Both Motor Types

Run the complete test suite with both motor driver types:

```bash
# Test with L298N (MT_BASIC)
cmake -S . -B build -DMOTOR_TYPE=MT_BASIC
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure

# Test with IBT_2 (MT_ROBUST)
cmake -S . -B build -DMOTOR_TYPE=MT_ROBUST
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

### Expected Results

All 37 tests should pass with both motor types:
- 6 Unit Tests
- 14 Integration Tests
- 17 Component Tests

The test suite uses mock drivers and is independent of actual motor hardware.

## Migration Guide: L298N to IBT_2

### Step-by-Step Migration

1. **Hardware Phase:**
   - Design new PCB or perf-board layout for IBT_2
   - Verify pin assignments and connections
   - Test basic motor control manually

2. **Firmware Phase:**
   - Change `motor_config.h`: MT_BASIC → MT_ROBUST
   - Verify new motor moves correctly
   - Test all states (UP, DOWN, STOP)
   - Test safety conditions (obstruction, stuck-on)

3. **Testing Phase:**
   - Run full test suite
   - Verify all 37 tests pass
   - Test manual limits and buttons
   - Test safety fault injection (if possible)

4. **Integration Phase:**
   - Install in stand desk prototype
   - Test load capacity (~50-100kg range)
   - Test emergency stop
   - Verify current detection thresholds

## Troubleshooting

### Motor Not Moving After Configuration Change

**Symptoms:** Firmware compiles but motor doesn't move

**Causes:**
1. Wrong pin connections to motor driver
2. Wrong MOTOR_TYPE selected in motor_config.h
3. Motor enable signal not reaching driver
4. Motor power supply disconnected

**Solutions:**
1. Verify `pin_config.h` shows correct pins for current MOTOR_TYPE
2. Check `motor_config.h` is set to expected type (MT_BASIC or MT_ROBUST)
3. Measure voltage at motor driver input pins
4. Verify motor power supply is connected and correct voltage

### Compilation Errors

**Error:** "PIN_MOTOR_EN1 was not declared in this scope"

**Cause:** Wrong MOTOR_TYPE or conflicting #includes

**Solution:**
```bash
# Clean everything and rebuild
Remove-Item -Path "build" -Recurse -Force
cmake -S . -B build -DMOTOR_TYPE=MT_BASIC
cmake --build build --config Release
```

### Safety Thresholds Not Working

**Symptom:** Obstruction/stuck-on detection not triggering

**Possible Cause:** Motor type draws different current than expected

**Solution:**
1. Measure actual motor current in normal operation
2. Measure stall current (blocked motor)
3. Update `MOTOR_SENSE_THRESHOLD_MA` and `MOTOR_SENSE_OBSTRUCTION_THRESHOLD_MA` as needed
4. Re-test with new thresholds

## References

- [Motor Control Architecture](06_DetailedDesign.md)
- [Hardware Abstraction Layer](../src/hal.h)
- [Pin Configuration](../src/pin_config.h)
- [Motor Configuration Header](../src/motor_config.h)
- [Safety Configuration](../src/safety_config.h)
- [Software Requirements](04_SoftwareRequirements.md)

---

**Document Version:** 2.0  
**Last Updated:** 2026-02-21  
**Motor Driver Support:** L298N (MT_BASIC) and IBT_2 (MT_ROBUST)
