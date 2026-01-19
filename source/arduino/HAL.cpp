#include "HAL.h"

#if defined(ARDUINO)
  #include <Arduino.h>
#else
  /* Use host-side HAL mock implementations (millis, pin IO) */
  #include "hal_mock/HALMock.h"
#endif

/* internal configuration constants (internal linkage) */
static const uint32_t kBlinkIntervalMs   = 500u;
static const uint8_t  kDefaultMotorSpeed = 255u;
static HAL_Logger_t    hal_logger = NULL;

/* HAL internal state for blinking and LED snapshots */
static uint32_t lastBlinkTime = 0;
static bool     errorLedState = false;
static bool     upLedState    = false;
static bool     downLedState  = false;

/* Logging hook - enabled when DEBUG defined on Arduino */
/* HAL_LOG: if a logger is registered use it, otherwise use Serial when available */
#if defined(ARDUINO)
  #if defined(DEBUG)
    #define HAL_LOG(msg) do { if (hal_logger) { hal_logger(msg); } else { Serial.println(msg); } } while(0)
  #else
    #define HAL_LOG(msg) do { if (hal_logger) { hal_logger(msg); } else { (void)0; } } while(0)
  #endif
#else
  #define HAL_LOG(msg) do { if (hal_logger) { hal_logger(msg); } else { /* no-op on host by default */ } } while(0)
#endif

/* small helpers */
static inline bool readButtonPressed(const int pin) {
  /* Assumes wiring uses INPUT_PULLUP; LOW == pressed */
  return (digitalRead(pin) == LOW);
}

static void apply_movement_state(const bool moveUp, const bool moveDown) {
  if (moveUp && !moveDown) {
    HAL_MoveUp(kDefaultMotorSpeed);
    return;
  }
  if (moveDown && !moveUp) {
    HAL_MoveDown(kDefaultMotorSpeed);
    return;
  }
  HAL_StopMotor();
}

/* Debounce implementation */
bool HAL_debounceButton(const int pin, DebounceState *state, const uint32_t debounceDelay) {
  if (state == NULL) {
    return false;
  }

  const bool rawPressed = readButtonPressed(pin);
  const uint32_t now = millis();

  state->changed = false;

  if (rawPressed != state->lastReading) {
    /* raw reading changed, restart debounce timer */
    state->lastDebounceMs = now;
    state->lastReading = rawPressed;
  }

  /* wrap-safe comparison */
  if ((uint32_t)(now - state->lastDebounceMs) >= debounceDelay) {
    if (state->stableState != state->lastReading) {
      state->stableState = state->lastReading;
      state->changed = true;
    }
  }

  return state->stableState;
}

bool HAL_readDebounced(const int pin) {
  static DebounceState tmpStateUp = { false, false, false, 0 };
  /* Use default debounce delay for convenience */
  return HAL_debounceButton(pin, &tmpStateUp, 50u);
}

/* Initialization: configure pins, set safe defaults (no blocking/delays) */
void HAL_init(void) {
  #if defined(ARDUINO)
  Serial.begin(9600);
#endif

  pinMode(BUTTON_UP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);
  pinMode(BUTTON_UPPER_LIMIT_PIN, INPUT_PULLUP); // Upper limit switch
  pinMode(BUTTON_LOWER_LIMIT_PIN, INPUT_PULLUP); // Lower limit switch

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  
  pinMode(ERROR_LED, OUTPUT);
  pinMode(LED_RIGHT_PIN, OUTPUT);
  pinMode(LED_LEFT_PIN, OUTPUT);
 

  /* safe defaults: motor stopped, LEDs off */
  HAL_StopMotor();
  HAL_SetErrorLED(false);
  HAL_SetMovingUpLED(false);
  HAL_SetMovingDownLED(false);

  /* initialize blink manager timestamp */
  lastBlinkTime = millis();
}

void HAL_wait_startup(void) {
  /* Default short blocking wait to allow hardware/human to settle */
  const unsigned long start = millis();
  const unsigned long timeout = 1000u;
  while ((unsigned long)(millis() - start) < timeout) {
    HAL_Task();
  }
}

void HAL_set_logger(HAL_Logger_t logger) {
  hal_logger = logger;
}

/* Periodic task to handle blinking and other time-based HAL work.
   Call from main loop at least once per blink interval. */
void HAL_Task(void) {
  const uint32_t now = millis();
  if ((uint32_t)(now - lastBlinkTime) >= kBlinkIntervalMs) {
    lastBlinkTime = now;
    /* toggle only error LED blinking; callers control whether error LED is in "blink mode" */
    if (HAL_GetErrorLED()) {
      errorLedState = !errorLedState;
      digitalWrite(ERROR_LED, errorLedState ? HIGH : LOW);
    }
    /* future: manage other timed LED behavior here */
  }
}

/* Map application outputs into hardware actions.
   Prioritization: application error -> explicit stop -> movement -> idle */
void HAL_ProcessAppState(const DeskAppTask_Return_t ret, const DeskAppOutputs_t *outputs) {
  HAL_LOG("HAL_ProcessAppState entry");

  if (ret != APP_TASK_SUCCESS) {
    /* app-level error: indicate and stop motor */
    HAL_SetErrorLED(true);
    HAL_StopMotor();
    HAL_LOG("APP_TASK_ERROR: stop and error LED on");
    return;
  }

  if (outputs == NULL) {
    /* defensive: no outputs => be safe */
    HAL_StopMotor();
    HAL_SetErrorLED(false);
    HAL_LOG("NULL outputs: stopped");
    return;
  }

  /* set error indicator based on application-provided flag */
  HAL_SetErrorLED(outputs->error);

  /* explicit stop takes precedence */
  if (outputs->stop) {
    HAL_StopMotor();
    HAL_LOG("Output stop requested");
    return;
  }

  /* apply movement deterministically */
  apply_movement_state(outputs->moveUp, outputs->moveDown);
  HAL_LOG("Movement applied");
}

/* LED helpers: update internal state snapshot and write pin */
void HAL_SetErrorLED(const bool state) {
  digitalWrite(ERROR_LED, state ? HIGH : LOW);
  errorLedState = state;
}

void HAL_SetWarningLED(const bool state) {
  // Warning LED not implemented in current hardware
  // Reserved for future use
  (void)state;
}

void HAL_SetPowerLED(const bool state) {
  // Power LED not implemented in current hardware
  // Could be mapped to ERROR_LED in IDLE state for "ready" indication
  // For now, no-op to maintain API compatibility
  (void)state;
}

void HAL_SetMovingUpLED(const bool state) {
  digitalWrite(LED_LEFT_PIN, state ? HIGH : LOW);
  upLedState = state;
}

void HAL_SetMovingDownLED(const bool state) {
  digitalWrite(LED_RIGHT_PIN, state ? HIGH : LOW);
  downLedState = state;
}

bool HAL_GetMovingUpLED(void)   { return upLedState; }
bool HAL_GetMovingDownLED(void) { return downLedState; }
bool HAL_GetErrorLED(void)      { return errorLedState; }

/* Motor commands (defensive) */
void HAL_MoveUp(const uint8_t speed) {
  const uint8_t s = (speed > 255u) ? 255u : speed;
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, s);
  HAL_SetMovingUpLED(true);
  HAL_SetMovingDownLED(false);
}

void HAL_MoveDown(const uint8_t speed) {
  const uint8_t s = (speed > 255u) ? 255u : speed;
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, s);
  HAL_SetMovingDownLED(true);
  HAL_SetMovingUpLED(false);
}

void HAL_StopMotor(void) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 0);
  HAL_SetMovingUpLED(false);
  HAL_SetMovingDownLED(false);
}

/* Backwards-compatible blink helpers (now managed via HAL_Task) */
void HAL_BlinkErrorLED(void)  { /* no-op; prefer HAL_Task to manage blinking */ }
void HAL_BlinkUPLED(void)     { /* no-op; use HAL_Task if blinking required */ }
void HAL_BlinkDOWNLED(void)   { /* no-op; use HAL_Task if blinking required */ }

