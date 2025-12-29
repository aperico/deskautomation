#include "HALMock.h"

void pinMode(int pin, int mode){}
void digitalWrite(int pin, bool value){}
bool digitalRead(int pin){ return false;}
void analogWrite(int pin, int value){}
unsigned long millis(){ return 0;}
void delay(unsigned long ms){}