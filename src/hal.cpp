#include "hal.h"
#include "safety_config.h"

#ifdef TESTENVIRONMENT
#include "hal_mock/HALMock.h"
#else
#include <Arduino.h>
#endif

// Motor type: Set at runtime via HAL_setMotorType()
// Allows HAL to adapt pin initialization and control based on actual motor driver
static MotorType_t g_motor_type = MT_BASIC;  // Default to MT_BASIC if not explicitly set

void HAL_setMotorType(MotorType_t motor_type)
{
    g_motor_type = motor_type;
}

// Debounce configuration (SWReq-009: 20ms ± 5ms)
static const uint32_t DEBOUNCE_MS = 20U;

static uint32_t last_button_time[BUTTON_COUNT] = {0, 0};
static bool button_raw_state[BUTTON_COUNT] = {false, false};
static bool button_stable_state[BUTTON_COUNT] = {false, false};

static const uint8_t button_pins[BUTTON_COUNT] = {PIN_BUTTON_UP, PIN_BUTTON_DOWN};
static const uint8_t limit_pins[LIMIT_COUNT] = {PIN_LIMIT_UPPER, PIN_LIMIT_LOWER};


static void init_inputs(void)
{
    pinMode(PIN_BUTTON_UP, INPUT_PULLUP);
    pinMode(PIN_BUTTON_DOWN, INPUT_PULLUP);
    pinMode(PIN_LIMIT_UPPER, INPUT_PULLUP);
    pinMode(PIN_LIMIT_LOWER, INPUT_PULLUP);

    // Initialize motor current sensing pin (MT_ROBUST only)
    if (g_motor_type == MT_ROBUST)
    {
        // IBT_2 Driver - Has integrated current sensing via shunt resistor
        pinMode(PIN_MOTOR_SENSE, INPUT);
    }
    // MT_BASIC: PIN_MOTOR_SENSE not used (no current sensing hardware)
}

static void init_outputs(void)
{
    if (g_motor_type == MT_BASIC)
    {
        // ========================================================================
        // L298N INITIALIZATION (3-pin control)
        // ========================================================================
        pinMode(PIN_MOTOR_EN1, OUTPUT);
        pinMode(PIN_MOTOR_EN2, OUTPUT);
        pinMode(PIN_MOTOR_PWM, OUTPUT);
        
        // Safe defaults: Motor stopped
        digitalWrite(PIN_MOTOR_EN1, LOW);
        digitalWrite(PIN_MOTOR_EN2, LOW);
        analogWrite(PIN_MOTOR_PWM, 0);
    }
    else if (g_motor_type == MT_ROBUST)
    {
        // ========================================================================
        // IBT_2 INITIALIZATION (2-pin PWM control)
        // ========================================================================
        pinMode(PIN_MOTOR_LPWM, OUTPUT);
        pinMode(PIN_MOTOR_RPWM, OUTPUT);
        
        // Optional: Configure diagnostic input
        pinMode(PIN_MOTOR_CIN, INPUT);
        
        // Safe defaults: Motor stopped
        analogWrite(PIN_MOTOR_LPWM, 0);
        analogWrite(PIN_MOTOR_RPWM, 0);
    }
    
    // Initialize 3 status LEDs (common to all motor types)
    pinMode(PIN_LED_BT_UP, OUTPUT);
    pinMode(PIN_LED_BT_DOWN, OUTPUT);
    pinMode(PIN_LED_ERROR, OUTPUT);

    // Safe defaults: all LEDs off
    digitalWrite(PIN_LED_BT_UP, LOW);
    digitalWrite(PIN_LED_BT_DOWN, LOW);
    digitalWrite(PIN_LED_ERROR, LOW);
}

void HAL_init(void)
{
    init_inputs();
    init_outputs();
    for (uint8_t i = 0; i < BUTTON_COUNT; ++i)
    {
        button_raw_state[i] = false;
        button_stable_state[i] = false;
        last_button_time[i] = 0U;
    }
}

bool HAL_readButton(ButtonID_t button)
{
    const uint32_t now = millis();
    const bool raw_pressed = (digitalRead(button_pins[button]) == LOW);

    if (raw_pressed != button_raw_state[button])
    {
        button_raw_state[button] = raw_pressed;
        last_button_time[button] = now;
    }

    const uint32_t elapsed = now - last_button_time[button];
    if (elapsed >= DEBOUNCE_MS)
    {
        button_stable_state[button] = button_raw_state[button];
    }

    return button_stable_state[button];
}

bool HAL_readLimitSensor(LimitID_t sensor)
{
    // Active-low sensors with pull-ups
    return (digitalRead(limit_pins[sensor]) == LOW);
}

uint16_t HAL_readMotorCurrent(void)
{
    if (g_motor_type == MT_ROBUST)
    {
        // IBT_2 Driver - Has integrated current sensing via shunt resistor
        const uint16_t adc = static_cast<uint16_t>(analogRead(PIN_MOTOR_SENSE));
        const uint32_t voltage_mv = (static_cast<uint32_t>(adc) * ADC_REF_MV) / 1023U;
        const uint32_t current_ma = (voltage_mv * 1000U) / SHUNT_MILLIOHMS;
        return static_cast<uint16_t>(current_ma);
    }
    else
    {
        // MT_BASIC Driver - No current sensing available
        // PIN_MOTOR_SENSE not connected to current sensor, return safe default
        return 0U;
    }
}

void HAL_setMotor(MotorDirection_t dir, uint8_t speed)
{
    if (g_motor_type == MT_BASIC)
    {
        // ========================================================================
        // L298N MOTOR CONTROL (3-pin: EN1, EN2, PWM)
        // ========================================================================
        // Direction via EN1/EN2 (mutually exclusive)
        // Speed via PWM on PIN_MOTOR_PWM
        
        switch (dir)
        {
            case MOTOR_STOP:
            default:
                digitalWrite(PIN_MOTOR_EN1, LOW);
                digitalWrite(PIN_MOTOR_EN2, LOW);
                analogWrite(PIN_MOTOR_PWM, 0);
                break;
            case MOTOR_UP:
                digitalWrite(PIN_MOTOR_EN1, HIGH);
                digitalWrite(PIN_MOTOR_EN2, LOW);
                analogWrite(PIN_MOTOR_PWM, speed);
                break;
            case MOTOR_DOWN:
                digitalWrite(PIN_MOTOR_EN1, LOW);
                digitalWrite(PIN_MOTOR_EN2, HIGH);
                analogWrite(PIN_MOTOR_PWM, speed);
                break;
        }
    }
    else if (g_motor_type == MT_ROBUST)
    {
        // ========================================================================
        // IBT_2 MOTOR CONTROL (2-pin PWM: LPWM, RPWM)
        // ========================================================================
        // Direction and speed controlled by PWM ratio:
        // - UP:   LPWM=255 (full), RPWM=0   (ramp down as needed for speed)
        // - DOWN: LPWM=0,   RPWM=255 (full, ramp down as needed for speed)
        // - STOP: LPWM=0,   RPWM=0
        
        switch (dir)
        {
            case MOTOR_STOP:
            default:
                analogWrite(PIN_MOTOR_LPWM, 0);
                analogWrite(PIN_MOTOR_RPWM, 0);
                break;
            case MOTOR_UP:
                // Left PWM high (counterclockwise = up), Right PWM for speed control
                analogWrite(PIN_MOTOR_LPWM, 255);
                analogWrite(PIN_MOTOR_RPWM, 255 - speed);
                break;
            case MOTOR_DOWN:
                // Right PWM high (clockwise = down), Left PWM for speed control
                analogWrite(PIN_MOTOR_LPWM, 255 - speed);
                analogWrite(PIN_MOTOR_RPWM, 255);
                break;
        }
    }
}

/**
 * @brief Set individual LED state
 * 
 * Controls one of the 3 independent status LEDs:
 * - LED_BT_UP (Pin 11): UP button pressed indicator
 * - LED_BT_DOWN (Pin 12): DOWN button pressed indicator  
 * - LED_ERROR (Pin 13): Error/fault state indicator
 * 
 * @param led - LED identifier (LED_BT_UP, LED_BT_DOWN, LED_ERROR)
 * @param state - LED state (LED_OFF=0 or LED_ON=1)
 */
void HAL_setLED(LEDID_t led, LEDState_t state)
{
    const bool level = (state == LED_ON) ? HIGH : LOW;
    
    switch (led)
    {
        case LED_BT_UP:
            digitalWrite(PIN_LED_BT_UP, level);
            break;
        case LED_BT_DOWN:
            digitalWrite(PIN_LED_BT_DOWN, level);
            break;
        case LED_ERROR:
            digitalWrite(PIN_LED_ERROR, level);
            break;
        default:
            // Invalid LED ID - do nothing
            break;
    }
}

uint32_t HAL_getTime(void)
{
    return millis();
}
