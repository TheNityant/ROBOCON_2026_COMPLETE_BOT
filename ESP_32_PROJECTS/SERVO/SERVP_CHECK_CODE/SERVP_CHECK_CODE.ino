#include <Arduino.h>

#define RXD2 16
#define TXD2 17

void setup() {
  Serial.begin(1000000); 
  Serial2.begin(1000000, SERIAL_8N1, RXD2, TXD2); 
}

void loop() {
  if (Serial.available()) {
    Serial2.write(Serial.read());
  }
  if (Serial2.available()) {
    Serial.write(Serial2.read());
  }
}