#include "HALMock.h"
#include <chrono>
#include <thread>

/* define Serial instance (matches extern in headers) */
SerialMock Serial;

/* Simple in-memory pin state (optional enhancement) */
static int pin_states[64] = {0};

/* basic implementations used by host tests */
void pinMode(int pin, int mode) { (void)pin; (void)mode; }
void digitalWrite(int pin, int value) { if (pin >= 0 && pin < 64) pin_states[pin] = value ? 1 : 0; (void)pin; (void)value; }
int digitalRead(int pin) { if (pin >= 0 && pin < 64) return pin_states[pin]; return LOW; }
void analogWrite(int pin, int value) { if (pin >= 0 && pin < 64) pin_states[pin] = value; (void)pin; (void)value; }

/* millis using steady_clock for monotonic time in host tests */
unsigned long millis(void) {
    using namespace std::chrono;
    static const steady_clock::time_point start = steady_clock::now();
    auto delta = duration_cast<milliseconds>(steady_clock::now() - start);
    return static_cast<unsigned long>(delta.count());
}

void delay(unsigned long ms) { std::this_thread::sleep_for(std::chrono::milliseconds(ms)); }