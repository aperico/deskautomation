// DeskController.h
// -----------------------------------------------------------------------------
// Desk movement logic and state machine for Desk Automation Project
// -----------------------------------------------------------------------------

#ifndef DESKCONTROLLER_H
#define DESKCONTROLLER_H



/**
 * @brief Application input signals for the Desk controller
 *
 * All fields are sampled inputs for a single execution step and are
 * provided by the HAL layer (debounced button states, limit switches).
 */
typedef struct {
  bool btUPPressed;       /**< true when Up button is pressed (debounced) */
  bool btDOWNPressed;     /**< true when Down button is pressed (debounced) */
  bool upperLimitActive;  /**< true when upper limit switch asserts */
  bool lowerLimitActive;  /**< true when lower limit switch asserts */
} DeskAppInputs_t;

/**
 * @brief Application outputs produced by the Desk controller
 */
typedef struct {
  bool moveUp;   /**< request motor move up */
  bool moveDown; /**< request motor move down */
  bool stop;     /**< explicit stop request (highest priority) */
  bool error;    /**< application-level error indicator */
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
void DeskApp_task_init(DeskAppInputs_t *inputs,
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