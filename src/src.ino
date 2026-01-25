#include "pin_config.h"
#include "hal.h"
#include "desk_app.h"
#include "motor_controller.h"

// Non-blocking scheduler: run APP logic every 250 ms
static const uint32_t APP_PERIOD_MS = 100U;
static uint32_t last_app_run_ms = 0U;
static AppOutput_t app_out_cached;

void setup()
{
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
    last_app_run_ms = HAL_getTime();
}

void loop()
{
    const uint32_t now_ms = HAL_getTime();

    // Time-based schedule: update application at 250 ms cadence
    if ((now_ms - last_app_run_ms) >= APP_PERIOD_MS)
    {
        AppInput_t inputs;
        inputs.button_up = HAL_readButton(BUTTON_UP);
        inputs.button_down = HAL_readButton(BUTTON_DOWN);
        inputs.limit_upper = HAL_readLimitSensor(LIMIT_UPPER);
        inputs.limit_lower = HAL_readLimitSensor(LIMIT_LOWER);
        inputs.fault_in = false; // placeholder for external fault propagation
        inputs.timestamp_ms = now_ms;

        AppOutput_t new_out;
        APP_Task(&inputs, &new_out);
        app_out_cached = new_out;
        last_app_run_ms = now_ms;


        // Motor control (ramp + stall detection placeholder)
        MotorControllerOutput_t mc_out = MotorController_update(app_out_cached.motor_cmd, app_out_cached.motor_speed, now_ms);

        // Fault propagation and output application
        const bool fault_active = app_out_cached.fault_out || mc_out.fault;
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

    

    // No blocking delay: loop remains non-blocking
}
