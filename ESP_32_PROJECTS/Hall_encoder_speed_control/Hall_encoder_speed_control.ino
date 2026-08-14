#include <HardwareSerial.h>

// Use UART Port 2 (Serial2)
HardwareSerial MotorSerial(1);

// Define the TX pin for Serial2
// We don't need an RX pin for this one-way mode
const int RX_PIN = -1; // Not used
const int TX_PIN = 17;

void setup() {
  Serial.begin(9600);
  Serial.println("MDDS-10 Simplified Serial Mode (One-Way)");

  // Start the Motor Serial port
  MotorSerial.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);

  Serial.println("Sending speed commands...");
}

void loop() {
  // Example: Ramp speed up
  Serial.println("Full Forward");
  setMotorSpeed(20); // 100% forward
}

/**
 * @brief Sets the motor speed using SIMPLIFIED SERIAL mode.
 * @param speed Speed from -100 (full reverse) to +100 (full forward).
 */
void setMotorSpeed(int speed) {
  // Clamp speed to -100 to +100
  speed = constrain(speed, -200, 200);

  // Map speed to the simplified serial 7-bit range
  // 0 = Full Reverse, 64 = Stop, 127 = Full Forward
  // The MSB (8th bit) is 0 for Motor 1
  byte data = map(speed, -200, 200, 128, 255);

  // Send the single command byte
  MotorSerial.write(data);
}