#include <Arduino.h>
#include "STSServoDriver.h"

// ================= SERVO UART =================
#define SERVO_RX 16
#define SERVO_TX 17

const byte SERVO_ID_1 = 0;
const byte SERVO_ID_2 = 1;

STSServoDriver servos;
HardwareSerial ServoSerial(2);

// ================= ULTRASONIC =================
#define trigPin 25
#define echoPin 26

// ================= MECHANISM =================
// 360° servo rotation = 2 cm linear motion
const float CM_PER_REV = 2.0;
const int STEPS_PER_REV = 4096;

// ================= STATE =================
long currentPos1 = 0;
long currentPos2 = 0;
float lastDistance = -1;   // invalid initial value

// ================= UTILS =================
int degreesToSteps(float degrees) {
  return (degrees * STEPS_PER_REV) / 360.0;
}

float readDistanceCM() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000); // timeout ~5m
  if (duration == 0) return -1;

  return duration * 0.034 / 2;
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);
  delay(2000);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  ServoSerial.begin(1000000, SERIAL_8N1, SERVO_RX, SERVO_TX);

  if (!servos.init(&ServoSerial)) {
    Serial.println("❌ Servo init failed");
    while (1);
  }

  Serial.println("✅ Servo bus connected");

  servos.setMode(SERVO_ID_1, STSMode::STEP);
  servos.setMode(SERVO_ID_2, STSMode::STEP);

  Serial.println("📏 Ultrasonic distance → servo motion ready");
}

// ================= LOOP =================
void loop() {
  float distance = readDistanceCM();

  if (distance < 0) {
    Serial.println("⚠️ Distance read failed");
    delay(300);
    return;
  }

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // First valid reading → just store it
  if (lastDistance < 0) {
    lastDistance = distance;
    return;
  }

  // Distance change
  float deltaCM = distance - lastDistance;

  // Ignore noise
  if (abs(deltaCM) < 0.2) {
    delay(300);
    return;
  }

  // Convert distance → degrees
  float degrees = (deltaCM / CM_PER_REV) * 360.0;
  int steps = degreesToSteps(degrees);

  Serial.print("Moving by ");
  Serial.print(degrees);
  Serial.println(" degrees");

  currentPos1 += steps;
  currentPos2 += steps;

  servos.setTargetPosition(SERVO_ID_1, currentPos1, 600);
  servos.setTargetPosition(SERVO_ID_2, currentPos2, 600);

  delay(100);
  while (servos.isMoving(SERVO_ID_1) || servos.isMoving(SERVO_ID_2))
    delay(30);

  lastDistance = distance;
  delay(300);
}
