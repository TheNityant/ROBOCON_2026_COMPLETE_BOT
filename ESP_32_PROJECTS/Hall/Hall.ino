/*
 * ESP32 Encoder Reading Program
 * * This program uses the ESP32's built-in hardware pulse counter (PCNT)
 * to read a quadrature encoder (like your Hall effect encoder) 
 * without slowing down the main processor.
 * * Required Library:
 * 1. Go to Tools > Manage Libraries...
 * 2. Search for "ESP32Encoder"
 * 3. Install the one by David P. M.
 */

#include "ESP32Encoder.h"

// --- Configuration ---
// Change these GPIO pins to match your wiring
#define ENCODER_A_PIN 14 
#define ENCODER_B_PIN 12
// ---------------------

// Create an encoder object
ESP32Encoder encoder;

// This variable will hold the last count to check for changes
// Using int64_t (a 64-bit number) prevents the count from
// overflowing (resetting to zero) on long-running projects.
int64_t lastEncoderCount = 0;

void setup() {
  // Start the Serial Monitor to print the readings
  Serial.begin(115200);

  // Set up the ESP32 to use its internal pull-up resistors
  // Use UP if your encoder signals rest HIGH, DOWN if they rest LOW.
  // If you have your own external resistors, use NONE.
  ESP32Encoder::useInternalWeakPullResistors = UP;

  // Attach the encoder pins
  // 'attachFullQuad' is recommended:
  // It counts on all 4 signal edges (A rise, A fall, B rise, B fall)
  // giving you the highest (4x) resolution from your encoder.
  encoder.attachFullQuad(ENCODER_A_PIN, ENCODER_B_PIN);

  // Set the starting count to 0
  encoder.setCount(0);

  Serial.println("Encoder reader initialized.");
  Serial.println("Turn the encoder to see the count change.");
}

void loop() {
  // Get the current count directly from the hardware counter
  int64_t currentEncoderCount = encoder.getCount();

  // Check if the encoder has moved since the last check
  if (currentEncoderCount != lastEncoderCount) {
    
    // Print the new position
    Serial.print("Encoder Count: ");
    Serial.println(currentEncoderCount);

    // Update the last count
    lastEncoderCount = currentEncoderCount;
  }

  // You can do other tasks here. The hardware counter works
  // in the background and will not miss pulses even if you
  // add delays or other complex code.
  delay(10); // A small delay to keep the serial monitor readable
}