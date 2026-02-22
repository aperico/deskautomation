#include "pin_config.h"
#include "hal.h"
#include "desk_app.h"
#include "motor_controller.h"
#include "motor_config.h"

// Non-blocking scheduler: run APP logic every 250 ms (SWReq-011: 250 ± 10 ms)
static const uint32_t APP_PERIOD_MS = 250U;
static uint32_t last_app_run_ms = 0U;
static AppOutput_t app_out_cached;
static bool motor_fault_latched = false;  // Latch motor controller faults (prevent blink loop)

void setup()
{
    // Configure HAL for the motor driver type before initializing hardware
    HAL_setMotorType(MotorConfig_getMotorType());
    HAL_init();
    MotorController_init();
    APP_Init();

    // Initialize cached outputs (safe defaults)
    app_out_cached.motor_cmd = MOTOR_STOP;
    app_out_cached.motor_speed = 0U;
    app_out_cached.led_bt_up = LED_OFF;
    app_out_cached.led_bt_down = LED_OFF;
    app_out_cached.led_error = LED_OFF;
    app_out_cached.fault_out = false;
    motor_fault_latched = false;  // Initialize motor fault latch
    last_app_run_ms = HAL_getTime();
}

void loop()
{
    const uint32_t now_ms = HAL_getTime();

    // Time-based schedule: update application at 250 ms cadence
    if ((now_ms - last_app_run_ms) >= APP_PERIOD_MS)
    {
        DeskControl_Task(now_ms);
        last_app_run_ms = now_ms;
    }

    

    // No blocking delay: loop remains non-blocking
}

void DeskControl_Task(uint32_t now_ms){
    // ========================================================================
    // Task: Read all hardware inputs and pass to application layer
    // HAL abstraction handles motor type differences internally:
    // - MT_BASIC: HAL_readMotorCurrent() always returns 0U (no hardware)
    // - MT_ROBUST: HAL_readMotorCurrent() returns actual current
    // ========================================================================
    AppInput_t inputs;
    inputs.button_up = HAL_readButton(BUTTON_UP);
    inputs.button_down = HAL_readButton(BUTTON_DOWN);
    inputs.limit_upper = HAL_readLimitSensor(LIMIT_UPPER);
    inputs.limit_lower = HAL_readLimitSensor(LIMIT_LOWER);
    inputs.fault_in = false;
    inputs.motor_type = MotorConfig_getMotorType();  // Pass motor type to app layer for runtime decisions
    inputs.timestamp_ms = now_ms;
    
    // Always read motor current - HAL handles motor type transparency
    // For MT_BASIC: Returns 0U (no hardware)
    // For MT_ROBUST: Returns actual current from sensor
    inputs.motor_current_ma = HAL_readMotorCurrent();

    AppOutput_t new_out;
    APP_Task(&inputs, &new_out);
    app_out_cached = new_out;

    // Motor control (ramp + stall detection)
    MotorControllerOutput_t mc_out = MotorController_update(app_out_cached.motor_cmd, app_out_cached.motor_speed, now_ms);

    // ========================================================================
    // Stall detection: Motor controller provides fault signal when stall detected
    // Application layer decides whether to latch based on motor type capabilities
    // ========================================================================
    if (mc_out.fault)
    {
        motor_fault_latched = true;  // Latch any identified fault
    }
    
    // Allow fault recovery when buttons released
    const bool both_buttons_released = !inputs.button_up && !inputs.button_down;
    if (motor_fault_latched && both_buttons_released)
    {
        motor_fault_latched = false;
        MotorController_init();  // Reset motor controller state
    }

    // Fault propagation and output application
    const bool fault_active = app_out_cached.fault_out || motor_fault_latched;
    if (fault_active)
    {
        HAL_setMotor(MOTOR_STOP, 0U);
        HAL_setLED(LED_BT_UP, LED_OFF);
        HAL_setLED(LED_BT_DOWN, LED_OFF);
        HAL_setLED(LED_ERROR, LED_ON);
    }
    else
    {
        HAL_setMotor(mc_out.dir, mc_out.pwm);
        HAL_setLED(LED_BT_UP, app_out_cached.led_bt_up);
        HAL_setLED(LED_BT_DOWN, app_out_cached.led_bt_down);
        HAL_setLED(LED_ERROR, app_out_cached.led_error);
    }
}
