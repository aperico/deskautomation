#pragma once
#include <string>

class SerialMock {
public:
    void begin(unsigned long baud);
    void print(const std::string& s);
    void print(int val);
    void print(const char* s);
    void println(const std::string& s);
    void println(int val);
    void println(const char* s);
};
 
// extern instance defined in SerialMock.cpp
extern SerialMock Serial;