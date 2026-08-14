#include <Arduino.h>
#include "STSServoDriver.h"

#define SERVO_RX 16
#define SERVO_TX 17

STSServoDriver servos;
HardwareSerial ServoSerial(2);

int degreesToSteps(int degrees) {
  return (degrees * 4096) / 360;
}

void setup() {
  Serial.begin(115200);
  delay(2000);

  Serial.println("\nSTS MULTI-SERVO STEP MODE CONTROL");
  Serial.println("Send: <servo_id> <degrees>");
  Serial.println("Example: 1 720   OR   2 -360");

  ServoSerial.begin(1000000, SERIAL_8N1, SERVO_RX, SERVO_TX);

  if (!servos.init(&ServoSerial)) {
    Serial.println("❌ Servo init failed");
    while (1);
  }

  Serial.println("✅ Servo bus connected");

  // Set all servos to STEP mode
  servos.setMode(0xFE, STSMode::STEP);  // 0xFE = broadcast
}

void loop() {
  if (Serial.available()) {

    int servoId = Serial.parseInt();
    int rotateDegrees = Serial.parseInt();

    // Ignore invalid input
    if (servoId < 0 || rotateDegrees == 0) {
      Serial.println("⚠️ Invalid input. Use: <servo_id> <degrees>");
      return;
    }

    int steps = degreesToSteps(rotateDegrees);

    Serial.print("Servo ");
    Serial.print(servoId);
    Serial.print(" -> ");
    Serial.print(rotateDegrees);
    Serial.println(" degrees");

    servos.setTargetPosition(servoId, steps, 500);

    delay(100);
    while (servos.isMoving(servoId))
      delay(50);

    Serial.println("✅ Done\n");
  }
}
