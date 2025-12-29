#pragma once

#define OUTPUT 1
#define INPUT 0
#define LOW 0
#define HIGH 1

void pinMode(int pin, int mode);
void digitalWrite(int pin, bool value);
bool digitalRead(int pin);
void analogWrite(int pin, int value);
unsigned long millis();
void delay(unsigned long ms);