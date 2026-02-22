#include "desk_app.h"
#include "safety_config.h"
#include <stddef.h>  // For NULL definition

static AppState_t current_state = APP_STATE_IDLE;
static uint32_t state_entry_time = 0U;

// Separate fault flags to prevent recovery race conditions
static bool button_fault_latched = false;      // Dual button press fault
static bool external_fault_latched = false;    // External fault input
static bool current_fault_latched = false;     // Stuck-on or obstruction fault

// Timers for current-based fault detection (UINT32_MAX = not running)
static uint32_t stuck_on_timer_start_ms = UINT32_MAX;    // Stuck-on detection when motor should be stopped
static uint32_t obstruction_timer_start_ms = UINT32_MAX; // Obstruction detection during motion


static void transition_to(AppState_t next_state, uint32_t now_ms)
{
    current_state = next_state;
    state_entry_time = now_ms;
}

void APP_Init(void)
{
    current_state = APP_STATE_IDLE;
    state_entry_time = 0U;
    button_fault_latched = false;
    external_fault_latched = false;
    current_fault_latched = false;
    stuck_on_timer_start_ms = UINT32_MAX;
    obstruction_timer_start_ms = UINT32_MAX;
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
    if (inputs == NULL || outputs == NULL)
    {
        return;
    }

    // SAFETY-CRITICAL: Fault recovery - only clear faults that can actually be cleared
    if (current_state == APP_STATE_FAULT)
    {
        const bool both_buttons_released = !inputs->button_up && !inputs->button_down;
        const bool no_external_fault = !inputs->fault_in;
        
        // Clear button fault only if buttons released
        if (button_fault_latched && both_buttons_released)
        {
            button_fault_latched = false;
        }
        
        // Clear external fault only if external source cleared
        if (external_fault_latched && no_external_fault)
        {
            external_fault_latched = false;
        }
        
        // Clear current fault only if buttons released (user acknowledgment)
        if (current_fault_latched && both_buttons_released)
        {
            current_fault_latched = false;
            stuck_on_timer_start_ms = UINT32_MAX;     // Reset timers
            obstruction_timer_start_ms = UINT32_MAX;
        }
    }

    // SAFETY-CRITICAL: Simultaneous button press is a LATCHED fault condition
    if (inputs->button_up && inputs->button_down)
    {
        button_fault_latched = true;
    }

    // Propagate external fault to application layer (latched)
    if (inputs->fault_in)
    {
        external_fault_latched = true;
    }

    // SAFETY-CRITICAL: Both limit switches active is a TRANSIENT fault condition
    const bool dual_limit_fault = inputs->limit_upper && inputs->limit_lower;

    // Execute state machine logic to determine motor commands
    switch (current_state)
    {
        case APP_STATE_IDLE:
        default:
        {
            outputs->motor_cmd = MOTOR_STOP;
            outputs->motor_speed = 0U;
            outputs->led_bt_up = LED_OFF;
            outputs->led_bt_down = LED_OFF;
            outputs->led_error = LED_OFF;

            if (inputs->button_up && !inputs->limit_upper)
            {
                transition_to(APP_STATE_MOVING_UP, inputs->timestamp_ms);
                outputs->motor_cmd = MOTOR_UP;
                outputs->motor_speed = 255U;
                outputs->led_bt_up = LED_ON;
                outputs->led_bt_down = LED_OFF;
                outputs->led_error = LED_OFF;
            }
            else if (inputs->button_down && !inputs->limit_lower)
            {
                transition_to(APP_STATE_MOVING_DOWN, inputs->timestamp_ms);
                outputs->motor_cmd = MOTOR_DOWN;
                outputs->motor_speed = 255U;
                outputs->led_bt_up = LED_OFF;
                outputs->led_bt_down = LED_ON;
                outputs->led_error = LED_OFF;
            }
            break;
        }

        case APP_STATE_MOVING_UP:
        {
            outputs->motor_cmd = MOTOR_UP;
            outputs->motor_speed = 255U;
            outputs->led_bt_up = LED_ON;
            outputs->led_bt_down = LED_OFF;
            outputs->led_error = LED_OFF;

            if (!inputs->button_up || inputs->limit_upper)
            {
                transition_to(APP_STATE_IDLE, inputs->timestamp_ms);
                outputs->motor_cmd = MOTOR_STOP;
                outputs->motor_speed = 0U;
                outputs->led_bt_up = LED_OFF;
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
            outputs->led_bt_down = LED_ON;
            outputs->led_error = LED_OFF;

            if (!inputs->button_down || inputs->limit_lower)
            {
                transition_to(APP_STATE_IDLE, inputs->timestamp_ms);
                outputs->motor_cmd = MOTOR_STOP;
                outputs->motor_speed = 0U;
                outputs->led_bt_up = LED_OFF;
                outputs->led_bt_down = LED_OFF;
                outputs->led_error = LED_OFF;
            }
            break;
        }

        case APP_STATE_FAULT:
        {
            // Output safe fault state (will be updated below if any faults remain)
            handle_fault(outputs);
            break;
        }
    }

    // ========================================================================
    // FEATURE SEPARATION: Current sensing fault detection (MT_ROBUST only)
    // Runs in ALL states including FAULT to monitor actual motor behavior
    // Current sensing only available when motor_type indicates MT_ROBUST driver
    // MT_BASIC drivers do not support current sensing - behavior determined at runtime
    // ========================================================================

    // Check if current sensing is available based on motor type
    const bool has_current_sensing = (inputs->motor_type == MT_ROBUST);
    
    if (has_current_sensing)
    {
        // IBT_2 ROBUST DRIVER - Has integrated current sensing
        if (outputs->motor_cmd == MOTOR_STOP)
        {
            // CASE 1: Stuck-on/runaway detection when STOP is commanded
            // Motor should draw minimal current when stopped
            obstruction_timer_start_ms = UINT32_MAX;  // Reset obstruction timer (not moving)
            
            if (inputs->motor_current_ma > MOTOR_SENSE_THRESHOLD_MA)
            {
                // High current while stopped - start/continue timer
                if (stuck_on_timer_start_ms == UINT32_MAX)
                {
                    stuck_on_timer_start_ms = inputs->timestamp_ms;
                }
                else if ((inputs->timestamp_ms - stuck_on_timer_start_ms) >= MOTOR_SENSE_FAULT_TIME_MS)
                {
                    // Timeout expired - motor stuck on, latch fault
                    current_fault_latched = true;
                }
            }
            else
            {
                // Current normal - reset stuck-on timer
                stuck_on_timer_start_ms = UINT32_MAX;
            }
        }
        else  // outputs->motor_cmd == MOTOR_UP or MOTOR_DOWN
        {
            // CASE 2: Obstruction/jam detection during motion (SysReq-013, FSR-007)
            // Motor should not draw excessive current during normal movement
            stuck_on_timer_start_ms = UINT32_MAX;  // Reset stuck-on timer (motor is moving)
            
            if (inputs->motor_current_ma > MOTOR_SENSE_OBSTRUCTION_THRESHOLD_MA)
            {
                // High current during motion - start/continue timer for debouncing
                if (obstruction_timer_start_ms == UINT32_MAX)
                {
                    obstruction_timer_start_ms = inputs->timestamp_ms;
                }
                else if ((inputs->timestamp_ms - obstruction_timer_start_ms) >= MOTOR_SENSE_FAULT_TIME_MS)
                {
                    // Timeout expired - obstruction detected, latch fault
                    current_fault_latched = true;
                }
            }
            else
            {
                // Current normal - reset obstruction timer
                obstruction_timer_start_ms = UINT32_MAX;
            }
        }
    }
    else
    {
        // MT_BASIC DRIVER - No current sensing available
        // Reset timers unconditionally to prevent any stale fault state
        // Never set current_fault_latched (hardware limitation)
        stuck_on_timer_start_ms = UINT32_MAX;
        obstruction_timer_start_ms = UINT32_MAX;
    }

    // ========================================================================
    // SAFETY-CRITICAL: Consolidated fault handling
    // Combine all fault sources and transition to FAULT state if any active
    // ========================================================================
    const bool any_latched_fault = button_fault_latched || external_fault_latched || current_fault_latched;
    const bool any_fault_active = any_latched_fault || dual_limit_fault;
    
    outputs->fault_out = any_fault_active;

    if (any_fault_active)
    {
        current_state = APP_STATE_FAULT;
        handle_fault(outputs);
    }
    else if (current_state == APP_STATE_FAULT)
    {
        // All faults cleared - transition back to IDLE
        transition_to(APP_STATE_IDLE, inputs->timestamp_ms);
        outputs->motor_cmd = MOTOR_STOP;
        outputs->motor_speed = 0U;
        outputs->led_bt_up = LED_OFF;
        outputs->led_bt_down = LED_OFF;
        outputs->led_error = LED_OFF;
    }
}

AppState_t APP_GetState(void)
{
    return current_state;
}
