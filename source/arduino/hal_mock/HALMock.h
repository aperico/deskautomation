#pragma once
#include "SerialMock.h"
#include <cstdint>

/* Arduino-like constants */
#ifndef OUTPUT
  #define OUTPUT 1
#endif
#ifndef INPUT
  #define INPUT 0
#endif
#ifndef LOW
  #define LOW 0
#endif
#ifndef HIGH
  #define HIGH 1
#endif
#ifndef INPUT_PULLUP
  #define INPUT_PULLUP 2
#endif

/* mock instance */
extern SerialMock Serial;

/* Minimal Arduino-like API for host unit tests */
void pinMode(int pin, int mode);
void digitalWrite(int pin, int value);
int  digitalRead(int pin);
void analogWrite(int pin, int value);
unsigned long millis(void);
void delay(unsigned long ms);