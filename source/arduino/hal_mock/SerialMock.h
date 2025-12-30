#pragma once

#include <string>
#include <iostream>

class SerialMock {
public:
    void begin(unsigned long baud) {
        std::cout << "[SerialMock] begin(" << baud << ")\n";
    }

    void print(const std::string& s) {
        std::cout << s;
    }

    void print(int val) {
        std::cout << val;
    }

    void print(const char* s) {
        std::cout << s;
    }

    void println(const std::string& s) {
        std::cout << s << std::endl;
    }

    void println(int val) {
        std::cout << val << std::endl;
    }

    void println(const char* s) {
        std::cout << s << std::endl;
    }
};

// Global instance to mimic Arduino's Serial
extern SerialMock Serial;