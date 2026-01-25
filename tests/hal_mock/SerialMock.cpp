#include "SerialMock.h"
#include <iostream>

void SerialMock::begin(unsigned long baud) { std::cout << "[SerialMock] begin(" << baud << ")\n"; }
void SerialMock::print(const std::string& s) { std::cout << s; }
void SerialMock::print(int val) { std::cout << val; }
void SerialMock::print(const char* s) { std::cout << s; }
void SerialMock::println(const std::string& s) { std::cout << s << std::endl; }
void SerialMock::println(int val) { std::cout << val << std::endl; }
void SerialMock::println(const char* s) { std::cout << s << std::endl; }