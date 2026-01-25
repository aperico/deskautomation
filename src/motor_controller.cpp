#include "motor_controller.h"

static const uint32_t RAMP_TIME_MS = 500U;          // 0 -> 255 in 500 ms
static const uint32_t STALL_TIMEOUT_MS = 2000U;     // 2 s timeout
static const uint8_t MIN_ACTIVE_PWM = 10U;          // below this treated as no drive

static MotorDirection_t last_dir = MOTOR_STOP;
static uint32_t dir_start_time = 0U;
static uint32_t last_update_time = 0U;
static uint32_t low_pwm_start_time = 0U;

void MotorController_init(void)
{
    last_dir = MOTOR_STOP;
    dir_start_time = 0U;
    last_update_time = 0U;
    low_pwm_start_time = 0U;
}

static uint8_t ramp_pwm(uint8_t target_pwm, uint32_t elapsed_ms)
{
    if (target_pwm == 0U)
    {
        return 0U;
    }
    if (elapsed_ms >= RAMP_TIME_MS)
    {
        return target_pwm;
    }
    const uint32_t scaled = (static_cast<uint32_t>(target_pwm) * elapsed_ms) / RAMP_TIME_MS;
    return static_cast<uint8_t>(scaled);
}

MotorControllerOutput_t MotorController_update(MotorDirection_t cmd_dir, uint8_t target_pwm, uint32_t now_ms)
{
    MotorControllerOutput_t out;
    out.dir = cmd_dir;
    out.pwm = 0U;
    out.fault = false;

    // Reset timers on direction change or stop
    if (cmd_dir != last_dir)
    {
        dir_start_time = now_ms;
        low_pwm_start_time = now_ms;
        last_dir = cmd_dir;
    }

    if (cmd_dir == MOTOR_STOP)
    {
        out.dir = MOTOR_STOP;
        out.pwm = 0U;
        low_pwm_start_time = now_ms;
    }
    else
    {
        const uint32_t elapsed = now_ms - dir_start_time;
        const uint8_t effective_pwm = ramp_pwm(target_pwm, elapsed);
        out.pwm = effective_pwm;

        if (effective_pwm <= MIN_ACTIVE_PWM)
        {
            // Track low PWM duration for stall detection placeholder
            if (low_pwm_start_time == 0U)
            {
                low_pwm_start_time = now_ms;
            }
            const uint32_t low_elapsed = now_ms - low_pwm_start_time;
            if (low_elapsed >= STALL_TIMEOUT_MS)
            {
                out.fault = true;
            }
        }
        else
        {
            low_pwm_start_time = now_ms; // reset when PWM rises above threshold
        }
    }

    last_update_time = now_ms;
    return out;
}
