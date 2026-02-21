#include "desk_app.h"
#include "safety_config.h"

static AppState_t current_state = APP_STATE_IDLE;
static uint32_t state_entry_time = 0U;
static bool fault_latched = false;
static uint32_t motor_fault_start_ms = 0U;


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
    motor_fault_start_ms = 0U;
}

/**
 * @brief Handle fault condition - stop motor and activate error LED
 * 
 * Sets all outputs to safe fault state:
 * - Motor stopped (direction=STOP, speed=0)
 * - All button LEDs off (no button feedback during fault)
 * - Error LED on (visual fault indication)
 * - Fault flag set (for external monitoring)
 */
static void handle_fault(AppOutput_t *outputs)
{
    outputs->motor_cmd = MOTOR_STOP;
    outputs->motor_speed = 0U;
    outputs->led_bt_up = LED_OFF;      // Turn off button indicators during fault
    outputs->led_bt_down = LED_OFF;
    outputs->led_error = LED_ON;        // Activate error indicator
    outputs->fault_out = true;
}

void APP_Task(const AppInput_t *inputs, AppOutput_t *outputs)
{
    if (inputs == nullptr || outputs == nullptr)
    {
        return;
    }

    // SAFETY-CRITICAL: Check fault recovery conditions first
    // Allow recovery from fault when command buttons released AND no other faults active
    if (fault_latched && current_state == APP_STATE_FAULT)
    {
        const bool both_buttons_released = !inputs->button_up && !inputs->button_down;
        const bool no_external_fault = !inputs->fault_in;
        
        if (both_buttons_released && no_external_fault)
        {
            fault_latched = false;  // Clear latched fault condition
        }
    }

    // SAFETY-CRITICAL: Simultaneous button press is a LATCHED fault condition
    // Both UP and DOWN pressed together indicates user error - requires release to clear
    if (inputs->button_up && inputs->button_down)
    {
        fault_latched = true;
    }

    // Propagate external fault to application layer (latched)
    if (inputs->fault_in)
    {
        fault_latched = true;
    }

    // SAFETY-CRITICAL: Both limit switches active is a TRANSIENT fault condition
    // Fault active only while both limits are triggered - clears automatically
    const bool dual_limit_fault = inputs->limit_upper && inputs->limit_lower;
    
    // Combined fault status: latched faults OR transient faults
    const bool fault_active = fault_latched || dual_limit_fault;
    
    outputs->fault_out = fault_active;

    if (fault_active)
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
            outputs->led_bt_up = LED_OFF;      // LEDs off in IDLE - motor not moving
            outputs->led_bt_down = LED_OFF;
            outputs->led_error = LED_OFF;

            if (inputs->button_up && !inputs->limit_upper)
            {
                transition_to(APP_STATE_MOVING_UP, inputs->timestamp_ms);
                outputs->motor_cmd = MOTOR_UP;
                outputs->motor_speed = 255U;
                outputs->led_bt_up = LED_ON;    // LED ON only when moving up
                outputs->led_bt_down = LED_OFF;
                outputs->led_error = LED_OFF;
            }
            else if (inputs->button_down && !inputs->limit_lower)
            {
                transition_to(APP_STATE_MOVING_DOWN, inputs->timestamp_ms);
                outputs->motor_cmd = MOTOR_DOWN;
                outputs->motor_speed = 255U;
                outputs->led_bt_up = LED_OFF;
                outputs->led_bt_down = LED_ON;  // LED ON only when moving down
                outputs->led_error = LED_OFF;
            }
            break;
        }

        case APP_STATE_MOVING_UP:
        {
            outputs->motor_cmd = MOTOR_UP;
            outputs->motor_speed = 255U;
            outputs->led_bt_up = LED_ON;        // Keep UP indicator active while moving
            outputs->led_bt_down = LED_OFF;
            outputs->led_error = LED_OFF;

            if (!inputs->button_up || inputs->limit_upper)
            {
                transition_to(APP_STATE_IDLE, inputs->timestamp_ms);
                outputs->motor_cmd = MOTOR_STOP;
                outputs->motor_speed = 0U;
                outputs->led_bt_up = LED_OFF;   // Turn off indicator when stopping
                outputs->led_bt_down = LED_OFF;
                outputs->led_error = LED_OFF;
            }
            break;
        }

        case APP_STATE_MOVING_DOWN:
        {
            outputs->motor_cmd = MOTOR_DOWN;
            outputs->motor_speed = 255U;
            outputs->led_bt_up = LED_OFF;
            outputs->led_bt_down = LED_ON;      // Keep DOWN indicator active while moving
            outputs->led_error = LED_OFF;

            if (!inputs->button_down || inputs->limit_lower)
            {
                transition_to(APP_STATE_IDLE, inputs->timestamp_ms);
                outputs->motor_cmd = MOTOR_STOP;
                outputs->motor_speed = 0U;
                outputs->led_bt_up = LED_OFF;
                outputs->led_bt_down = LED_OFF;  // Turn off indicator when stopping
                outputs->led_error = LED_OFF;
            }
            break;
        }

        case APP_STATE_FAULT:
        {
            // Check if all fault conditions have cleared (both latched and transient)
            // Note: dual_limit_fault is already defined in outer scope
            const bool any_fault_active = fault_latched || dual_limit_fault;
            
            if (!any_fault_active)
            {
                // All faults cleared - transition back to IDLE
                transition_to(APP_STATE_IDLE, inputs->timestamp_ms);
                outputs->motor_cmd = MOTOR_STOP;
                outputs->motor_speed = 0U;
                outputs->led_bt_up = LED_OFF;
                outputs->led_bt_down = LED_OFF;
                outputs->led_error = LED_OFF;
            }
            else
            {
                // Faults still active - remain in FAULT state
                handle_fault(outputs);
            }
            break;
        }
    }

    // SAFETY-CRITICAL: Detect stuck-on/runaway when STOP is commanded
    if (outputs->motor_cmd == MOTOR_STOP)
    {
        if (inputs->motor_current_ma > MOTOR_SENSE_THRESHOLD_MA)
        {
            if (motor_fault_start_ms == 0U)
            {
                motor_fault_start_ms = inputs->timestamp_ms;
            }
            else if ((inputs->timestamp_ms - motor_fault_start_ms) >= MOTOR_SENSE_FAULT_TIME_MS)
            {
                fault_latched = true;
            }
        }
        else
        {
            motor_fault_start_ms = 0U;
        }
    }
    else
    {
        motor_fault_start_ms = 0U;
    }

    if (fault_latched)
    {
        current_state = APP_STATE_FAULT;
        handle_fault(outputs);
    }
}

AppState_t APP_GetState(void)
{
    return current_state;
}
