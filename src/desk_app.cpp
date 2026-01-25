#include "desk_app.h"

static AppState_t current_state = APP_STATE_IDLE;
static uint32_t state_entry_time = 0U;
static bool fault_latched = false;

static void transition_to(AppState_t next_state, uint32_t now_ms)
{
    current_state = next_state;
    state_entry_time = now_ms;
}

void APP_Init(void)
{
    current_state = APP_STATE_IDLE;
    state_entry_time = 0U;
    fault_latched = false;
}

static void handle_fault(AppOutput_t *outputs)
{
    outputs->motor_cmd = MOTOR_STOP;
    outputs->motor_speed = 0U;
    outputs->led_status = LED_ERROR;
    outputs->fault_out = true;
}

void APP_Task(const AppInput_t *inputs, AppOutput_t *outputs)
{
    if (inputs == nullptr || outputs == nullptr)
    {
        return;
    }

    // Propagate external fault to application layer
    if (inputs->fault_in)
    {
        fault_latched = true;
    }

    outputs->fault_out = fault_latched;

    if (fault_latched)
    {
        current_state = APP_STATE_FAULT;
        handle_fault(outputs);
        return;
    }

    switch (current_state)
    {
        case APP_STATE_IDLE:
        default:
        {
            outputs->motor_cmd = MOTOR_STOP;
            outputs->motor_speed = 0U;
            outputs->led_status = LED_IDLE;

            // Conflicting inputs: ignore
            if (inputs->button_up && inputs->button_down)
            {
                break;
            }

            if (inputs->button_up && !inputs->limit_upper)
            {
                transition_to(APP_STATE_MOVING_UP, inputs->timestamp_ms);
                outputs->motor_cmd = MOTOR_UP;
                outputs->motor_speed = 255U;
                outputs->led_status = LED_ACTIVE;
            }
            else if (inputs->button_down && !inputs->limit_lower)
            {
                transition_to(APP_STATE_MOVING_DOWN, inputs->timestamp_ms);
                outputs->motor_cmd = MOTOR_DOWN;
                outputs->motor_speed = 255U;
                outputs->led_status = LED_ACTIVE;
            }
            break;
        }

        case APP_STATE_MOVING_UP:
        {
            outputs->motor_cmd = MOTOR_UP;
            outputs->motor_speed = 255U;
            outputs->led_status = LED_ACTIVE;

            if (!inputs->button_up || inputs->limit_upper)
            {
                transition_to(APP_STATE_IDLE, inputs->timestamp_ms);
                outputs->motor_cmd = MOTOR_STOP;
                outputs->motor_speed = 0U;
                outputs->led_status = LED_IDLE;
            }
            break;
        }

        case APP_STATE_MOVING_DOWN:
        {
            outputs->motor_cmd = MOTOR_DOWN;
            outputs->motor_speed = 255U;
            outputs->led_status = LED_ACTIVE;

            if (!inputs->button_down || inputs->limit_lower)
            {
                transition_to(APP_STATE_IDLE, inputs->timestamp_ms);
                outputs->motor_cmd = MOTOR_STOP;
                outputs->motor_speed = 0U;
                outputs->led_status = LED_IDLE;
            }
            break;
        }

        case APP_STATE_FAULT:
        {
            handle_fault(outputs);
            break;
        }
    }
}

AppState_t APP_GetState(void)
{
    return current_state;
}
