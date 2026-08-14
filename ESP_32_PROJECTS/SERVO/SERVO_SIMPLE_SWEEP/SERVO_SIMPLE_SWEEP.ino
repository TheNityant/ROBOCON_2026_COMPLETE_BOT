// Multi-turn STEP mode control via Serial Monitor
// +degrees = CW, -degrees = CCW

#include <Arduino.h>
#include "STSServoDriver.h"

#define SERVO_RX 16
#define SERVO_TX 17

const byte SERVO_ID = 0;

STSServoDriver servos;
HardwareSerial ServoSerial(2);

int degreesToSteps(int degrees)
{
  return (degrees * 4096) / 360;
}

void setup()
{
  Serial.begin(115200);
  delay(2000);

  Serial.println("\nSTS STEP MODE SERIAL CONTROL");
  Serial.println("Enter degrees (e.g. 720 or -360) and press Enter");

  ServoSerial.begin(1000000, SERIAL_8N1, SERVO_RX, SERVO_TX);

  if (!servos.init(&ServoSerial))
  {
    Serial.println("❌ Servo init failed");
    while (1);
  }

  Serial.println("✅ Servo connected");

  // STEP = relative multi-turn movement
  servos.setMode(SERVO_ID, STSMode::STEP);
}

void loop()
{
  // Wait for user input
  if (Serial.available())
  {
    int rotateDegrees = Serial.parseInt();  // reads signed int

    if (rotateDegrees == 0)
    {
      Serial.println("⚠️ Zero movement ignored");
      return;
    }

    int steps = degreesToSteps(rotateDegrees);

    Serial.print("Rotating ");
    Serial.print(rotateDegrees);
    Serial.println(" degrees");

    servos.setTargetPosition(SERVO_ID, steps, 500);

    delay(100);
    while (servos.isMoving(SERVO_ID))
      delay(50);

    Serial.println("✅ Movement complete\n");
  }
}
