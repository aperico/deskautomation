#include "app.h"

static AppState_t appState = APP_STATE_IDLE;

void process_move_up(){
  appState = APP_STATE_MOVE_UP;
}

void process_move_down(){
  appState = APP_STATE_MOVE_DOWN;
}

void process_stop(){
  appState = APP_STATE_ERROR;
}

DeskAppTask_Return_t DeskApp_task(const DeskAppInputs_t *inputs, DeskAppOutputs_t *outputs){
DeskAppTask_Return_t ret = APP_TASK_SUCCESS;  
  
  // Default outputs
  outputs->moveUp = FALSE;
  outputs->moveDown = FALSE;
  outputs->stop = FALSE;

  switch(appState){
    outputs->error = FALSE;

    case APP_STATE_IDLE:
      outputs->stop = TRUE;
      // buttons are mutually exclusive
      if(inputs->btUPPressed){
        process_move_up();
      }else if(inputs->btDOWNPressed){
        process_move_down();
      }else{
        process_stop();
      }
      break;
    case APP_STATE_MOVE_UP:
      outputs->moveUp = TRUE;
      if((inputs->upperLimitActive == TRUE) || (inputs->btUPPressed == FALSE)){
        process_stop();
      }

      if(inputs->lowerLimitActive == TRUE){
        appState = APP_STATE_ERROR;
      }
      break;
    case APP_STATE_MOVE_DOWN:
      outputs->moveDown = TRUE;
      
      if((inputs->lowerLimitActive == TRUE) || (inputs->btDOWNPressed == FALSE)){
        process_stop();
      }
      if(inputs->upperLimitActive == TRUE){
        appState = APP_STATE_ERROR;
      }

      break;
    case APP_STATE_ERROR:
      outputs->stop = TRUE;
      outputs->error = TRUE;
      // TODO create method to reset the desk to a safe position
      break;
    default:
      appState = APP_STATE_IDLE;
      outputs->stop = TRUE;
      break;
  }

  return ret;
}
