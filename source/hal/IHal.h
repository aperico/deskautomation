#pragma once

class IHal {
public:
  virtual ~IHal() = default;

  virtual void pinMode(int pin, int mode) = 0;
  virtual void digitalWrite(int pin, bool value) = 0;
  virtual void analogWrite(int pin, int value) = 0;
  virtual unsigned long millis() = 0;
};