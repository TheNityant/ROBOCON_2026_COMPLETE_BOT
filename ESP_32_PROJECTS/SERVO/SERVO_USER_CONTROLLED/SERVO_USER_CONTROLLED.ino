#include <Arduino.h>
#include "STSServoDriver.h"

#define SERVO_RX 16
#define SERVO_TX 17

const byte SERVO_ID_1 = 2;
const byte SERVO_ID_2 = 3;
const byte SERVO_ID_3 = 4;

STSServoDriver servos;
HardwareSerial ServoSerial(2);

// Convert degrees → steps (0-4095 per 360°)
int degreesToSteps(int degrees) {
  return (degrees * 4096) / 360;
}

void setup() {
  Serial.begin(115200);
  delay(2000);

  Serial.println("\n==============================");
  Serial.println("STS MULTI SERVO STEP CONTROL");
  Serial.println("Enter degrees: 720 or -360");
  Serial.println("==============================");

  // Servo UART start
  ServoSerial.begin(1000000, SERIAL_8N1, SERVO_RX, SERVO_TX);

  if (!servos.init(&ServoSerial)) {
    Serial.println("❌ Servo init failed");
    while (1);
  }

  Serial.println("✅ Servo bus connected");

  // STEP mode for all
  servos.setMode(SERVO_ID_1, STSMode::STEP);
  delay(50);
  servos.setMode(SERVO_ID_2, STSMode::STEP);
  delay(50);
  servos.setMode(SERVO_ID_3, STSMode::STEP);
  delay(50);

  Serial.println("✅ All servos in STEP mode");
}

void loop() {

  if (Serial.available()) {

    int rotateDegrees = Serial.parseInt();

    // clear serial buffer
    while (Serial.available()) Serial.read();

    if (rotateDegrees == 0) {
      Serial.println("⚠️ Enter valid degree like 720 or -360");
      return;
    }

    int steps = degreesToSteps(rotateDegrees);

    Serial.print("\nRotating all servos -> ");
    Serial.print(rotateDegrees);
    Serial.println(" degrees");

    Serial.print("Steps: ");
    Serial.println(steps);

    // Send together (synchronized)
    servos.setTargetPosition(SERVO_ID_1, steps, 500);
    servos.setTargetPosition(SERVO_ID_2, steps, 500);
    servos.setTargetPosition(SERVO_ID_3, steps, 500);

    Serial.println("✅ Command sent\n");
  }
}