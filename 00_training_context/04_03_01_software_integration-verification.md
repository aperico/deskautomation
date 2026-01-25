# Session 4.3.1: Software Integration Testing

**Focus:** Integration Tests
**Time:** ~30 minutes

---

## Part 1: Software Integration Tests – HAL Mocking & Layer Testing

### Why Mock the HAL Layer?

**The Problem:** 
Testing the full software stack (Main Loop → DeskApp → MotorController → HAL → Arduino hardware) requires:
- Physical Arduino board with motor driver, buttons, LEDs, limit switches
- Manual intervention (pressing buttons, measuring timing with oscilloscope)
- Inability to test edge cases (e.g., simultaneous button presses, electrical noise)
- Slow feedback loop (compile → upload → run → observe)

**The Solution: Abstraction + Mocking**
- DeskApp and MotorController depend only on **HAL interfaces** (function signatures), not implementations
- Replace real HAL (Arduino libraries) with **mock HAL** (fake implementations) for testing
- Mock HAL provides deterministic, programmable behavior (inject button states, simulate timing, measure PWM commands)
- Tests run on **any machine** without hardware (fast, repeatable, automated)

**Key Principle:** Your code architecture (4 layers) enables this:

**Production:** 
```
Main Loop (Arduino) → DeskApp → MotorController → HAL (real Arduino)
```

**Testing:**
```
Test harness → DeskApp → MotorController → HAL (mock)
```

### Abstraction Layer Pattern: Interface-Based Design

**How it works:**

1. **HAL is an interface** (header file `hal.h` with function declarations)
   - Defines: `HAL_readButton()`, `HAL_setMotorPWM()`, `HAL_getTimeMs()`, etc.
   - No implementation details (hidden in `hal.cpp`)

2. **DeskApp calls HAL functions, not specific hardware**
   - **Good**: `HAL_readButton()` → works with real HAL or mock HAL
   - **Bad**: `digitalWrite(PIN_BTN_UP, HIGH)` → tightly coupled to Arduino; can't mock

3. **Two implementations of HAL for different environments:**
   - **Production:** `hal.cpp` uses Arduino (or RTOS) APIs (`pinMode`, `digitalRead`, `analogWrite`)
   - **Testing:** `hal_mock.h/cpp` uses in-memory variables instead of real hardware

### Conditional Compilation for Production vs. Testing

**Pattern: Use `#if defined()` to switch implementations:**

```cpp
// In your main sketch or test harness:
#if defined(TEST_ENVIRONMENT)
  #include "hal_mock.h"       // Mock HAL for unit/integration tests
#else
  #include "hal.h"            // Real HAL for production Arduino
#endif
```

**Build commands:**
```bash
# Production (Arduino): compile without TEST_ENVIRONMENT flag
arduino-cli compile --board arduino:avr:uno 02_src/

# Testing: compile with TEST_ENVIRONMENT flag
g++ -DTEST_ENVIRONMENT -o test_runner 03_tests/*.cpp 02_src/desk_app.cpp 02_src/motor_controller.cpp hal_mock.cpp
```

### HAL Mock Implementation Example

**File: `tests/hal_mock.h`** (Replace real HAL for testing)

```cpp
#ifndef HAL_MOCK_H
#define HAL_MOCK_H

#include <stdint.h>
#include <stdbool.h>

// Mock state (internal to test harness)
namespace HALMock {
    extern ButtonState_t injectedButton;  // Test injects button state
    extern bool injectedLimitSwitch;      // Test injects limit switch state
    extern int16_t lastMotorPwm;          // Capture what PWM was commanded
    extern LEDColor_t lastLedColor;       // Capture what LED was set
    extern uint32_t mockTimeMs;           // Test controls system time
};

// Mock implementations (same signatures as real hal.h)
void HAL_init(void);
ButtonState_t HAL_readButton(void);
bool HAL_readLimitSwitch(void);
void HAL_setMotorPWM(int16_t pwm);
void HAL_setLED(LEDColor_t color);
uint32_t HAL_getTimeMs(void);

#endif // HAL_MOCK_H
```

**File: `tests/hal_mock.cpp`** (Mock implementation)

```cpp
#include "hal_mock.h"

namespace HALMock {
    ButtonState_t injectedButton = BUTTON_NONE;
    bool injectedLimitSwitch = false;
    int16_t lastMotorPwm = 0;
    LEDColor_t lastLedColor = LED_GREEN;
    uint32_t mockTimeMs = 0;
}

void HAL_init(void) { }  // No-op for mock

ButtonState_t HAL_readButton(void) {
    return HALMock::injectedButton;
}

bool HAL_readLimitSwitch(void) {
    return HALMock::injectedLimitSwitch;
}

void HAL_setMotorPWM(int16_t pwm) {
    HALMock::lastMotorPwm = pwm;
}

void HAL_setLED(LEDColor_t color) {
    HALMock::lastLedColor = color;
}

uint32_t HAL_getTimeMs(void) {
    return HALMock::mockTimeMs;
}
```

## Part 2: Integration Test Cases Implementation

Create test cases that verify DeskApp/MotorController interact correctly with HAL **interface** (without requiring real hardware).

**Prompt (Copy & Paste):**

```
TASK: Write 6–8 integration tests using Google Test that verify software layer interactions with HAL interface (mocked hardware).
Test cases shall also verify architecture elements (APIs) and communication protocols. Test cases shall also contain reference to upstream artifacts when possible.

FOCUS: Verify DeskApp → MotorController → HAL call sequences, not functional behavior. Functional tests already exist (component tests).

UPSTREAM DOCS:
- software architecture (layer interfaces, sequence diagrams)
- system and hardware elements

TEST COVERAGE (pick 6–10 cases):
- PWM Command Range 
- Button-to-HAL Flow
- LED State Tracking
- Motor Stop Command
- Limit Switch Routing
- Time-Dependent Ramp
- Mock State Isolation

TEST STRUCTURE:
- Harness: test_software_integration_hal.cpp
- Pattern: inject via HALMock::injected*, run DeskApp, assert on HALMock::last* captures

ASSERTION EXAMPLES:
- EXPECT_EQ(HALMock::lastMotorPwm, 255);
- EXPECT_TRUE(HALMock::lastLedColor == LED_GREEN);
- EXPECT_NEAR(HALMock::lastMotorPwm, 127, 20);  // allow tolerance

Format: C++ code using Google Test (ready to compile and run).
OUTPUT FILE: uner tests directory named "test_software_integration_hal"

```

---

**Next Step:** Proceed to [system testing](04_03_03_system-testing.md).