/*
 * Arduino LED Blink Example
 * 
 * A minimal application demonstrating LED blinking every 500ms
 * 
 * Hardware:
 * - Arduino board (UNO, Nano, etc.)
 * - Built-in LED on pin 13
 */

// Pin definition
const int LED_PIN = 13;

// Blink interval in milliseconds
const unsigned long BLINK_INTERVAL = 500;

// Variables for timing
unsigned long previousMillis = 0;
bool ledState = LOW;

void setup() {
  // Initialize the LED pin as an output
  pinMode(LED_PIN, OUTPUT);
  
  // Start with LED off
  digitalWrite(LED_PIN, LOW);
}

void loop() {
  // Get current time
  unsigned long currentMillis = millis();
  
  // Check if it's time to toggle the LED
  if (currentMillis - previousMillis >= BLINK_INTERVAL) {
    // Save the current time
    previousMillis = currentMillis;
    
    // Toggle LED state
    ledState = !ledState;
    
    // Update LED
    digitalWrite(LED_PIN, ledState);
  }
}
