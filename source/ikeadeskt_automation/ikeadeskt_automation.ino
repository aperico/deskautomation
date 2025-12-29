#include "app.h"

static const int ERROR_LED = 13;
static const int MOVING_UP_LED = 12;
static const int MOVING_DOWN_LED = 11;
static const uint16_t BLINK_INTERVAL_MS = 500;
static const int IN1 = 8;
static const int IN2 = 9;
static const int ENA = 10;
static const uint8_t MOTOR_SPEED = 255;
static DeskAppInputs_t inputs;
static DeskAppOutputs_t outputs;
 
void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ERROR_LED, OUTPUT);
  HAL_StopMotor();
  delay(2000);   // Allow time before any motion


  // app inputs init
  inputs.btUPPressed = FALSE;
  inputs.btDOWNPressed = FALSE;
  inputs.upperLimitActive = FALSE;
  inputs.lowerLimitActive = FALSE;

  // app outputs init
  outputs.moveUp = FALSE;
  outputs.moveDown = FALSE;
  outputs.stop = TRUE;
  outputs.error = FALSE;
}

void HAL_SetErrorLED(const bool state){
  digitalWrite(ERROR_LED, state);
}


void HAL_SetMovingUpLED(const bool state){
  digitalWrite(MOVING_UP_LED, state);
}

void HAL_SetMovingDownLED(const bool state){
  digitalWrite(MOVING_DOWN_LED, state);
}

bool HAL_GetMovingDownLED(){
  return digitalRead(MOVING_DOWN_LED);
}

bool HAL_GetMovingUpLED(){
  return digitalRead(MOVING_UP_LED);
}

bool HAL_GetErrorLED(){
  return digitalRead(ERROR_LED);
}
  
void HAL_MoveUp(const uint8_t speed) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, speed);
}

void HAL_MoveDown(const uint8_t speed) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, speed);
}

void HAL_StopMotor() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 0);
}

void HAL_BlinkErrorLED(){
  static uint32_t lastBlinkTime = 0;
  if(millis() - lastBlinkTime >= BLINK_INTERVAL_MS){
    lastBlinkTime = millis();
    HAL_SetErrorLED(!HAL_GetErrorLED());
  }
} 

void HAL_BlinkUPLED(){
  static uint32_t lastBlinkTime = 0;
  if(millis() - lastBlinkTime >= BLINK_INTERVAL_MS){
    lastBlinkTime = millis();
    HAL_SetMovingUpLED(!HAL_GetMovingUpLED());
  }
}

void HAL_BlinkDOWNLED(){
  static uint32_t lastBlinkTime = 0;
  if(millis() - lastBlinkTime >= BLINK_INTERVAL_MS){
    lastBlinkTime = millis();
    HAL_SetMovingDownLED(!HAL_GetMovingDownLED());
  }
}


void loop() {
  static DeskAppTask_Return_t ret;
  
  
  ret = DeskApp_task(&inputs, &outputs);

  if(ret == APP_TASK_SUCCESS){
    if(outputs.moveUp == TRUE){
      HAL_SetErrorLED(false);
      HAL_BlinkUPLED();
      HAL_MoveUp(MOTOR_SPEED);
    }
    if(outputs.moveDown == TRUE){
      HAL_SetErrorLED(false);
      HAL_BlinkDOWNLED(); 
      HAL_MoveDown(MOTOR_SPEED);
    }
    if(outputs.stop == TRUE){
      HAL_SetErrorLED(false);
      HAL_SetMovingUpLED(false);
      HAL_SetMovingDownLED(false);
      HAL_StopMotor();
    }
  }else{
    HAL_SetErrorLED(true);
    HAL_SetMovingUpLED(false);
    HAL_SetMovingDownLED(false);
    HAL_BlinkErrorLED();
    HAL_StopMotor();
  }

}
