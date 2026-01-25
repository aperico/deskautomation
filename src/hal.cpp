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
    pinMode(PIN_MOTOR_REN, OUTPUT);
    pinMode(PIN_MOTOR_LEN, OUTPUT);
    pinMode(PIN_MOTOR_RPWM, OUTPUT);
    pinMode(PIN_MOTOR_LPWM, OUTPUT);
    pinMode(PIN_LED_STATUS, OUTPUT);

    // Safe defaults
    digitalWrite(PIN_MOTOR_REN, LOW);
    digitalWrite(PIN_MOTOR_LEN, LOW);
    analogWrite(PIN_MOTOR_RPWM, 0);
    analogWrite(PIN_MOTOR_LPWM, 0);
    digitalWrite(PIN_LED_STATUS, LOW);
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
            digitalWrite(PIN_MOTOR_REN, LOW);
            digitalWrite(PIN_MOTOR_LEN, LOW);
            analogWrite(PIN_MOTOR_RPWM, 0);
            analogWrite(PIN_MOTOR_LPWM, 0);
            break;
        case MOTOR_UP:
            digitalWrite(PIN_MOTOR_REN, HIGH);
            digitalWrite(PIN_MOTOR_LEN, LOW);
            analogWrite(PIN_MOTOR_RPWM, speed);
            analogWrite(PIN_MOTOR_LPWM, 0);
            break;
        case MOTOR_DOWN:
            digitalWrite(PIN_MOTOR_REN, LOW);
            digitalWrite(PIN_MOTOR_LEN, HIGH);
            analogWrite(PIN_MOTOR_RPWM, 0);
            analogWrite(PIN_MOTOR_LPWM, speed);
            break;
    }
}

void HAL_setLED(LEDStatus_t status)
{
    switch (status)
    {
        case LED_OFF:
            digitalWrite(PIN_LED_STATUS, LOW);
            break;
        case LED_IDLE:
            digitalWrite(PIN_LED_STATUS, HIGH);
            break;
        case LED_ACTIVE:
        {
            // Blink at 1 Hz
            const uint32_t t = millis() % 1000U;
            digitalWrite(PIN_LED_STATUS, (t < 500U) ? HIGH : LOW);
            break;
        }
        case LED_ERROR:
            digitalWrite(PIN_LED_STATUS, HIGH);
            break;
        default:
            digitalWrite(PIN_LED_STATUS, LOW);
            break;
    }
}

uint32_t HAL_getTime(void)
{
    return millis();
}
