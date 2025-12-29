#ifndef DESKCONTROLLER_H
#define DESKCONTROLLER_H

typedef enum { TRUE = 1, FALSE = 0 } Boolean_t;

typedef struct {
  Boolean_t btUPPressed;
  Boolean_t btDOWNPressed;
  Boolean_t upperLimitActive;
  Boolean_t lowerLimitActive;
} DeskAppInputs_t;

typedef struct {
  Boolean_t moveUp;
  Boolean_t moveDown;
  Boolean_t stop;
  Boolean_t error;
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