

#include "HAL.h"

#if defined(ARDUINO)
  #include <Arduino.h>
#else
  /* Use host-side HAL mock implementations (millis, pin IO) */
  #include "hal_mock/HALMock.h"
#endif


static const uint8_t  kDefaultMotorSpeed = 255u;
static HAL_Logger_t    hal_logger = NULL;

// Minimal logger for debug
#if defined(ARDUINO)
#  if defined(DEBUG)
#    define HAL_LOG(msg) do { Serial.println(msg); if (hal_logger) { hal_logger(msg); } } while(0)
#  else
#    define HAL_LOG(msg) do { if (hal_logger) { hal_logger(msg); } else { (void)0; } } while(0)
#  endif
#else
#  define HAL_LOG(msg) do { if (hal_logger) { hal_logger(msg); } else { /* no-op on host by default */ } } while(0)
#endif

// Read ON/OFF/ON switch state (UP, OFF, DOWN)
SwitchState_t HAL_ReadSwitchState(void) {
#if defined(ARDUINO)
  bool up = digitalRead(SWITCH_UP_PIN) == LOW;
  bool down = digitalRead(SWITCH_DOWN_PIN) == LOW;
  if (up && !down) return SWITCH_STATE_UP;
  if (down && !up) return SWITCH_STATE_DOWN;
  return SWITCH_STATE_OFF;
#else
  return SWITCH_STATE_OFF;
#endif
}

// Error state flag (file-local)
static bool hal_error_flag = false;

bool HAL_HasError(void) {
  return hal_error_flag;
}

void HAL_ClearError(void) {
  hal_error_flag = false;
}

/**
 * @brief Enable or disable both EN pins (defensive safety)
 *
 * @param enable true to enable, false to disable
 */
static inline void HAL_motorEnable(bool enable) {
  digitalWrite(R_EN_PIN, enable);
  digitalWrite(L_EN_PIN, enable);
}

/**
 * @brief Convert ADC value to current in Amps
 * @param adc_value Raw ADC reading (0-1023 for 10-bit ADC)
 * @param vref Reference voltage (typically 5.0V)
 * @param volts_per_amp Sensor scaling (e.g., 1.0V/A)
 * @return Current in Amps
 */
float HAL_adc_to_amps(int adc_value, float vref, float volts_per_amp) {
  float v_sense = (adc_value / 1023.0f) * vref;
  return v_sense / volts_per_amp;
}

void HAL_init(void) {
#if defined(ARDUINO)
  Serial.begin(9600);
  // Motor driver pins
  pinMode(RPWM_PIN, OUTPUT);
  pinMode(LPWM_PIN, OUTPUT);
  pinMode(R_EN_PIN, OUTPUT);
  pinMode(L_EN_PIN, OUTPUT);
  // Switch pins (active low)
  pinMode(SWITCH_UP_PIN, INPUT_PULLUP);
  pinMode(SWITCH_DOWN_PIN, INPUT_PULLUP);
  // Current sense pins (optional, usually analog inputs by default)
#ifdef R_IS_PIN
  pinMode(R_IS_PIN, INPUT);
#endif
#ifdef L_IS_PIN
  pinMode(L_IS_PIN, INPUT);
#endif
#endif
  HAL_StopMotor();
}

void HAL_Task(HAL_Ouputs_t *hal_outputs, bool motor_enable, uint8_t motor_pwm) {
    // Clear error flag if motor is not running
    bool motor_running = motor_enable && motor_pwm > 0;
    if (!motor_running) {
      hal_error_flag = false;
    }
  // for timed events; none needed in this minimal implementation
#if defined(ARDUINO)
  if (hal_outputs) {
    hal_outputs->r_current = analogRead(R_IS_PIN);
    hal_outputs->l_current = analogRead(L_IS_PIN);
  }
  static unsigned long lastLogTime = 0;
  unsigned long now = millis();

#define VREF 5.0f
#define VOLTS_PER_AMP 0.1f   // IBT-2: 1V/10A scaling (0.1 V/A)
#define OVERCURRENT_AMPS 5.0f // 4A supply, allow for surges
#define NOLOAD_AMPS 0.2f      // Typical no-load current

  if (now - lastLogTime >= 1000 && hal_outputs) {
    float r_amps = HAL_adc_to_amps(hal_outputs->r_current, VREF, VOLTS_PER_AMP);
    float l_amps = HAL_adc_to_amps(hal_outputs->l_current, VREF, VOLTS_PER_AMP);
    char buf[128];
    snprintf(buf, sizeof(buf), "R_IS: %d (%.2fA), L_IS: %d (%.2fA)", hal_outputs->r_current, r_amps, hal_outputs->l_current, l_amps);
    HAL_LOG(buf);

    // Only check for errors and log warnings if motor is running
    bool motor_running = motor_enable && motor_pwm > 0;
    if (motor_running) {
      // Overcurrent detection
      if (r_amps > OVERCURRENT_AMPS || l_amps > OVERCURRENT_AMPS) {
        HAL_LOG("WARNING: Overcurrent detected!");
        hal_error_flag = true;
      }
      // No-load detection
      if (r_amps < NOLOAD_AMPS && l_amps < NOLOAD_AMPS) {
        HAL_LOG("WARNING: No-load (open circuit or disconnected motor)");
        hal_error_flag = true;
      }
      // Stall detection (high current, but no movement logic would go here)
      if ((r_amps > OVERCURRENT_AMPS || l_amps > OVERCURRENT_AMPS) /* && motor enabled, not moving */) {
        HAL_LOG("WARNING: Possible stall condition!");
        hal_error_flag = true;
      }
    }
    lastLogTime = now;
  }
#endif
}


void HAL_ProcessAppState(const DeskAppTask_Return_t ret, const DeskAppOutputs_t *outputs, HAL_Ouputs_t *hal_outputs) {
  if (outputs == NULL || !outputs->motor_enable || outputs->motor_pwm == 0) {
    HAL_StopMotor();
    if (hal_outputs) HAL_Task(hal_outputs, false, 0);
    return;
  }
  if (outputs->motor_direction) {
    HAL_MoveDown(outputs->motor_pwm);
  } else {
    HAL_MoveUp(outputs->motor_pwm);
  }
  if (hal_outputs) HAL_Task(hal_outputs, outputs->motor_enable, outputs->motor_pwm);
}


void HAL_MoveUp(const uint8_t speed) {
#if defined(ARDUINO)
  HAL_motorEnable(true);
  analogWrite(RPWM_PIN, speed);
  analogWrite(LPWM_PIN, 0);
#endif
}


void HAL_MoveDown(const uint8_t speed) {
#if defined(ARDUINO)
  HAL_motorEnable(true);
  analogWrite(RPWM_PIN, 0);
  analogWrite(LPWM_PIN, speed);
#endif
}


void HAL_StopMotor(void) {
#if defined(ARDUINO)
  analogWrite(RPWM_PIN, 0);
  analogWrite(LPWM_PIN, 0);
  HAL_motorEnable(false);
#endif
}

void HAL_wait_startup(void) {
#if defined(ARDUINO)
  delay(1000);
#endif
}

void HAL_set_logger(HAL_Logger_t logger) {
  // No-op for now
}



