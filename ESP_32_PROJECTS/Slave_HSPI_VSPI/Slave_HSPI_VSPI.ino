#include <Wire.h>

// --- PIN DEFINITIONS FOR ESP32-S3 ---
// Adjust these if you are using different pins!
// Common S3 defaults are SDA=8, SCL=9.
// If you are using the D14/D15 pins labeled on a dev board, check their GPIO numbers.
#define I2C_SDA 21 
#define I2C_SCL 22

#define SLAVE_ADDR 0x10

volatile int receivedValue = 0;
volatile bool newData = false;

void setup() {
  // 1. Start Serial for debugging
  Serial.begin(115200);
  while (!Serial); // Wait for Serial to be ready (good for S3)
  
  Serial.println("ESP32-S3 I2C Receiver Started...");

  // 2. Configure I2C Pins
  // This is crucial for ESP32-S3 as default pins vary by board
  Wire.setPins(I2C_SDA, I2C_SCL);
  
  // 3. Start I2C as SLAVE
  Wire.begin(SLAVE_ADDR);
  
  // 4. Register the "Event Handler"
  // When data arrives, the function 'receiveEvent' is called automatically
  Wire.onReceive(receiveEvent);
  
  Serial.println("Waiting for Encoder Data from STM32...");
}

void loop() {
  // If we received a new number in the background interrupt...
  if (newData) {
    Serial.print("Encoder Position: ");
    Serial.println(receivedValue);
    newData = false; // Reset the flag
  }
  
  // Small delay to keep the loop sane
  delay(10);
}

// --- INTERRUPT FUNCTION: RUNS WHEN DATA ARRIVES ---
void receiveEvent(int howMany) {
  // We expect exactly 2 bytes (High Byte + Low Byte)
  if (howMany == 2) {
    byte highByte = Wire.read();
    byte lowByte = Wire.read();

    // Recombine the bytes: 
    // Shift High Byte 8 steps left, then OR with Low Byte
    receivedValue = (highByte << 8) | lowByte;
    
    newData = true; // Tell the loop we have data
  } else {
    // If we get garbage size, throw it away
    while(Wire.available()) Wire.read();
  }
}