

#ifndef SWITCH_STATE_T_DEFINED
#define SWITCH_STATE_T_DEFINED
typedef enum {
  SWITCH_STATE_OFF = 0,
  SWITCH_STATE_UP,
  SWITCH_STATE_DOWN
} SwitchState_t;
#endif
// DeskController.h
// -----------------------------------------------------------------------------
// Desk movement logic and state machine for Desk Automation Project
// -----------------------------------------------------------------------------


#ifndef DESKCONTROLLER_H
#define DESKCONTROLLER_H
#include <stdint.h>




typedef struct {
  SwitchState_t switch_state; /**< State of ON/OFF/ON switch */
} DeskAppInputs_t;

/**
 * @brief Application outputs produced by the Desk controller
 */
typedef struct {
  bool motor_enable;      /**< Enable motor driver */
  bool motor_direction;  /**< Motor direction: false=one way, true=reverse */
  uint8_t motor_pwm;     /**< Motor PWM value (0-255) */
} DeskAppOutputs_t;

typedef enum {
  APP_TASK_SUCCESS = 0,
  APP_TASK_ERROR
} DeskAppTask_Return_t;

/**
 * @brief Initialize the application logic and outputs to a safe state
 *
 * @param inputs Pointer to inputs structure (may be NULL)
 * @param outputs Pointer to outputs structure to initialize (must not be NULL)
 */
void DeskApp_task_init(const DeskAppInputs_t *inputs,
                                  DeskAppOutputs_t *outputs);

/**
 * @brief Execute one step of the application logic
 *
 * Performs state-machine dispatch based on current inputs and updates outputs.
 *
 * @param inputs Pointer to input snapshot (must not be NULL)
 * @param outputs Pointer to output structure to be updated (must not be NULL)
 * @return APP_TASK_SUCCESS on normal execution, APP_TASK_ERROR if inputs/outputs invalid or fatal error
 */
DeskAppTask_Return_t DeskApp_task(const DeskAppInputs_t *inputs,
                                  DeskAppOutputs_t *outputs);
#endif