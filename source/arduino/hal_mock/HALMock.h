#pragma once
#include "SerialMock.h"
#define OUTPUT 1
#define INPUT 0
#define LOW 0
#define HIGH 1
#define INPUT_PULLUP 1
#define INPUT_PULLDOWN 0

SerialMock Serial;
void pinMode(int pin, int mode);
void digitalWrite(int pin, bool value);
bool digitalRead(int pin);
void analogWrite(int pin, int value);
unsigned long millis();
void delay(unsigned long ms);