// DeskController.h
// -----------------------------------------------------------------------------
// Desk movement logic and state machine for Desk Automation Project
// -----------------------------------------------------------------------------

#ifndef DESKCONTROLLER_H
#define DESKCONTROLLER_H



typedef struct {
  bool btUPPressed;
  bool btDOWNPressed;
  bool upperLimitActive;
  bool lowerLimitActive;
} DeskAppInputs_t;

typedef struct {
  bool moveUp;
  bool moveDown;
  bool stop;
  bool error;
} DeskAppOutputs_t;

typedef enum {
  APP_TASK_SUCCESS = 0,
  APP_TASK_ERROR
} DeskAppTask_Return_t;

void DeskApp_task_init(DeskAppInputs_t *inputs,
                                  DeskAppOutputs_t *outputs);

DeskAppTask_Return_t DeskApp_task(const DeskAppInputs_t *inputs,
                                  DeskAppOutputs_t *outputs);
#endif