// DeskControllerTestInterface.h
// -----------------------------------------------------------------------------
// White-box test interface for DeskController private functions
// This header exposes internal state and functions for unit testing purposes.
// -----------------------------------------------------------------------------

#ifndef DESKCONTROLLER_TESTINTERFACE_H
#define DESKCONTROLLER_TESTINTERFACE_H

#include "../source/arduino/DeskController.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Internal state enumeration (mirrored from DeskController.cpp)
 */
typedef enum {
  APP_STATE_IDLE = 0,
  APP_STATE_MOVE_UP,
  APP_STATE_MOVE_DOWN,
  APP_STATE_DWELL,
  APP_STATE_ERROR
} AppState_t;

/**
 * @brief Test interface structure for accessing internal state
 */
typedef struct {
  int currentState;
  unsigned long dwellStartMs;
  unsigned long movementStartMs;
} DeskControllerInternalState_t;

/* Function prototypes for private state handlers (for white-box testing) */
void handle_idle_test(const DeskAppInputs_t *inputs, DeskAppOutputs_t *outputs);
void handle_move_up_test(const DeskAppInputs_t *inputs, DeskAppOutputs_t *outputs);
void handle_move_down_test(const DeskAppInputs_t *inputs, DeskAppOutputs_t *outputs);
void handle_dwell_test(const DeskAppInputs_t *inputs, DeskAppOutputs_t *outputs);

/* State inspection and manipulation functions */
void DeskApp_get_internal_state(DeskControllerInternalState_t *state);
void DeskApp_set_internal_state(const DeskControllerInternalState_t *state);

/* Time control for deterministic testing */
void DeskApp_set_time_ms(unsigned long timeMs);
unsigned long DeskApp_get_time_ms(void);

/* Constants exposure for validation */
extern const unsigned long kDwellMs_test;
extern const unsigned long kMovementTimeoutMs_test;

#ifdef __cplusplus
}
#endif

#endif // DESKCONTROLLER_TESTINTERFACE_H