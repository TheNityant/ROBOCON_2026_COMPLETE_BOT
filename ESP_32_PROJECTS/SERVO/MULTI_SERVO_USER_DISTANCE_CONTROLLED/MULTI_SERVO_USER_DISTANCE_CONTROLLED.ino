#include <Arduino.h>
#include "STSServoDriver.h"

#define SERVO_RX 16
#define SERVO_TX 17

const byte SERVO_ID_1 = 3;
const byte SERVO_ID_2 = 2;

STSServoDriver servos;
HardwareSerial ServoSerial(2);

// ===== MECHANISM CONSTANTS =====
const int STEPS_PER_REV = 4096;
const float CM_PER_REV  = 2.0;     // 2 cm per 360°
const int SPEED = 500;

// Track relative positions
long currentPos1 = 0;
long currentPos2 = 0;

// Convert distance (cm) → servo steps
long distanceCmToSteps(float distanceCm)
{
  // steps = distance × (4096 / 2)
  return (long)(distanceCm * (STEPS_PER_REV / CM_PER_REV));
}

void setup()
{
  Serial.begin(115200);
  delay(2000);

  Serial.println("\nSTS DUAL SERVO DISTANCE CONTROL");
  Serial.println("Enter distance in cm");
  Serial.println("Example: 2.0  OR  -1.5");

  ServoSerial.begin(1000000, SERIAL_8N1, SERVO_RX, SERVO_TX);

  if (!servos.init(&ServoSerial))
  {
    Serial.println("❌ Servo init failed");
    while (1);
  }

  Serial.println("✅ Servo bus connected");

  // STEP mode for both servos
  servos.setMode(SERVO_ID_1, STSMode::STEP);
  servos.setMode(SERVO_ID_2, STSMode::STEP);
}

void loop()
{
  if (Serial.available())
  {
    float distanceCm = Serial.parseFloat();

    if (distanceCm == 0.0)
    {
      Serial.println("⚠️ Invalid input. Enter distance in cm");
      return;
    }

    long steps = distanceCmToSteps(distanceCm);

    // Update target positions
    currentPos1 += steps;
    currentPos2 += steps;

    Serial.print("Moving BOTH servos by ");
    Serial.print(distanceCm);
    Serial.println(" cm");

    servos.setTargetPosition(SERVO_ID_1, currentPos1, SPEED, true);
    servos.setTargetPosition(SERVO_ID_2, currentPos2, SPEED, true);

    // 🔥 Start both together
    servos.trigerAction();

    delay(100);
    while (servos.isMoving(SERVO_ID_1) || servos.isMoving(SERVO_ID_2))
      delay(50);

    Serial.println("✅ Motion complete\n");
  }
}
