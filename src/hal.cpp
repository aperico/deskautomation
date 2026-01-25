#include "hal.h"

#ifdef TESTENVIRONMENT
#include "hal_mock/HALMock.h"
#else
#include <Arduino.h>
#endif

// Debounce configuration
static const uint32_t DEBOUNCE_MS = 50U;

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
}

static void init_outputs(void)
{
    pinMode(PIN_MOTOR_EN1, OUTPUT);
    pinMode(PIN_MOTOR_EN2, OUTPUT);
    pinMode(PIN_MOTOR_PWM, OUTPUT);
    
    // Initialize 3 status LEDs
    pinMode(PIN_LED_BT_UP, OUTPUT);
    pinMode(PIN_LED_BT_DOWN, OUTPUT);
    pinMode(PIN_LED_ERROR, OUTPUT);

    // Safe defaults: Motor stopped, all LEDs off
    digitalWrite(PIN_MOTOR_EN1, LOW);
    digitalWrite(PIN_MOTOR_EN2, LOW);
    analogWrite(PIN_MOTOR_PWM, 0);
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

void HAL_setMotor(MotorDirection_t dir, uint8_t speed)
{
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
