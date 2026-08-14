#include <HardwareSerial.h>
#include <Wire.h>

// --- I2C CONFIGURATION ---
#define I2C_SLAVE_ADDR 0x08
#define I2C_SDA 21 // Matches Main ESP32
#define I2C_SCL 22 // Matches Main ESP32

// --- MUST MATCH MAIN ESP32 EXACTLY ---
struct LocomotionData {
  uint8_t cross;
  uint8_t circle;
  uint8_t square;
  uint8_t mode; // Received from Main ESP32 router
} locoData;

// ------------ TWO MDDS10 DRIVERS ------------
HardwareSerial Driver1(1);   
HardwareSerial Driver2(2);   
#define TX1 16
#define TX2 17

// ------------ ENCODERS ------------
#define ENC1_A 25
#define ENC1_B 26
#define ENC2_A 23
#define ENC2_B 19
#define ENC3_A 32
#define ENC3_B 33
#define ENC4_A 34
#define ENC4_B 35

// ------------ CONSTANTS ------------
const int PPR = 270;
const float WHEEL_DIAMETER = 1.2;
const float CIRCUMFERENCE = PI * WHEEL_DIAMETER;
const float PULSES_PER_METER = PPR / CIRCUMFERENCE;

const int TARGET_RPM = 300; 
unsigned long lastRpmCheck = 0;
const int checkInterval = 100; // Check and adjust every 100ms

// --- PRINT LOGIC VARIABLES ---
unsigned long lastPrintTime = 0;
const int printInterval = 250; // Print debug data every 250ms

// ------------ VARIABLES ------------
volatile long pulse1 = 0, pulse2 = 0, pulse3 = 0, pulse4 = 0;
long prevPulses[4] = {0, 0, 0, 0};
int motorSpeeds[4] = {25, 25, 25, 25}; // Initial starting speed (0-63)
long targetPulses = 0;
bool motorRunning = false;

enum MoveMode { IDLE, FORWARD, REVERSE, LEFT, RIGHT };
MoveMode currentMode = IDLE;

// Button States
bool crossPressed = false, circlePressed = false, squarePressed = false;

// ------------ I2C RECEIVE EVENT ------------
void receiveEvent(int howMany) {
  // Only process if we received the exact expected number of bytes
  if (howMany == sizeof(locoData)) {
    uint8_t* ptr = (uint8_t*)&locoData;
    for (int i = 0; i < sizeof(locoData); i++) {
      ptr[i] = Wire.read();
    }
  } else {
    // If we get a corrupted/partial transmission, flush the buffer
    while (Wire.available()) {
      Wire.read();
    }
  }
}

// ------------ UART WRITE FUNCTION ------------
void uartWrite(HardwareSerial &driver, uint8_t channel, uint8_t dir, uint8_t speed) {
  if (speed > 63) speed = 63; // Cap at max MDDS10 speed
  uint8_t udata = (channel << 7) | (dir << 6) | (speed & 0x3F);
  driver.write(udata);
}

// ------------ STEP-BY-STEP SPEED LOGIC ------------
void adjustSpeedsToTarget() {
  unsigned long now = millis();
  if (now - lastRpmCheck < checkInterval) return; // Only adjust every 100ms

  float dt = (now - lastRpmCheck) / 1000.0;
  long currentPulses[4];
  
  noInterrupts();
  currentPulses[0] = pulse1; currentPulses[1] = pulse2;
  currentPulses[2] = pulse3; currentPulses[3] = pulse4;
  interrupts();

  for (int i = 0; i < 4; i++) {
    // 1. Calculate Actual RPM
    float deltaP = abs(currentPulses[i] - prevPulses[i]);
    float actualRPM = (deltaP / PPR) * (60.0 / dt);

    // 2. Increase or Decrease by 1
    if (actualRPM < TARGET_RPM) {
      if (motorSpeeds[i] < 63) motorSpeeds[i]++; 
    } 
    else if (actualRPM > TARGET_RPM) {
      if (motorSpeeds[i] > 0) motorSpeeds[i]--;
    }

    prevPulses[i] = currentPulses[i];
  }
  lastRpmCheck = now;
}

// ------------ DIRECTION LOGIC ------------
#define CW 0
#define CCW 1

void applyMotorCommands() {
  switch(currentMode) {
    case FORWARD:
      uartWrite(Driver1, 0, CW, motorSpeeds[0]);  uartWrite(Driver1, 1, CCW, motorSpeeds[1]);
      uartWrite(Driver2, 0, CW, motorSpeeds[2]);  uartWrite(Driver2, 1, CW, motorSpeeds[3]);
      break;
    case RIGHT:
      uartWrite(Driver1, 0, CCW, motorSpeeds[0]); uartWrite(Driver1, 1, CCW, motorSpeeds[1]);
      uartWrite(Driver2, 0, CCW, motorSpeeds[2]); uartWrite(Driver2, 1, CW, motorSpeeds[3]);
      break;
    case LEFT:
      uartWrite(Driver1, 0, CW, motorSpeeds[0]);  uartWrite(Driver1, 1, CW, motorSpeeds[1]);
      uartWrite(Driver2, 0, CW, motorSpeeds[2]);  uartWrite(Driver2, 1, CCW, motorSpeeds[3]);
      break;
    default: stopMotor(); break;
  }
}

void stopMotor() {
  uartWrite(Driver1, 0, CW, 0); uartWrite(Driver1, 1, CW, 0);
  uartWrite(Driver2, 0, CW, 0); uartWrite(Driver2, 1, CW, 0);
}

void startMove(MoveMode mode, float distance) {
  noInterrupts();
  pulse1 = pulse2 = pulse3 = pulse4 = 0;
  interrupts();
  for(int i=0; i<4; i++) { prevPulses[i] = 0; motorSpeeds[i] = 20; } // Reset to baseline
  
  targetPulses = abs(distance) * PULSES_PER_METER;
  currentMode = mode;
  motorRunning = true;
  lastRpmCheck = millis();
}

// ------------ PRINT DEBUG LOGIC ------------
void printDebugData() {
  unsigned long now = millis();
  if (now - lastPrintTime >= printInterval) {
    
    Serial.print("Mode: "); Serial.print(locoData.mode);
    Serial.print(" | Controller(X,O,Sq): ");
    Serial.print(locoData.cross); Serial.print(",");
    Serial.print(locoData.circle); Serial.print(",");
    Serial.print(locoData.square);

    if (motorRunning) {
      Serial.print(" | Motion: ");
      switch(currentMode) {
        case FORWARD: Serial.print("FWD"); break;
        case REVERSE: Serial.print("REV"); break;
        case LEFT: Serial.print("LEFT"); break;
        case RIGHT: Serial.print("RIGHT"); break;
        default: Serial.print("IDLE"); break;
      }
      
      Serial.print(" | Speeds[0-3]: ");
      for(int i=0; i<4; i++) { 
        Serial.print(motorSpeeds[i]); 
        if(i < 3) Serial.print(","); 
      }
      
      long avgPulse = (abs(pulse1) + abs(pulse2) + abs(pulse3) + abs(pulse4)) / 4;
      Serial.print(" | Pulses: "); Serial.print(avgPulse);
      Serial.print("/"); Serial.print(targetPulses);
    }
    Serial.println();
    lastPrintTime = now;
  }
}

// ------------ ENCODER ISRs ------------
void IRAM_ATTR encoder1() { static int lastA=0; int A=digitalRead(ENC1_A); int B=digitalRead(ENC1_B); if(A!=lastA){if(A==B)pulse1++; else pulse1--; lastA=A;}}
void IRAM_ATTR encoder2() { static int lastA=0; int A=digitalRead(ENC2_A); int B=digitalRead(ENC2_B); if(A!=lastA){if(A==B)pulse2++; else pulse2--; lastA=A;}}
void IRAM_ATTR encoder3() { static int lastA=0; int A=digitalRead(ENC3_A); int B=digitalRead(ENC3_B); if(A!=lastA){if(A==B)pulse3++; else pulse3--; lastA=A;}}
void IRAM_ATTR encoder4() { static int lastA=0; int A=digitalRead(ENC4_A); int B=digitalRead(ENC4_B); if(A!=lastA){if(A==B)pulse4++; else pulse4--; lastA=A;}}

void setup() {
  Serial.begin(115200);
  
  // Initialize I2C Slave
  Wire.beginTransmission(I2C_SLAVE_ADDR);
  Wire.write((uint8_t*)&locoData, sizeof(locoData));
  byte error = Wire.endTransmission();

  if (error != 0) {
      Serial.print("I2C Tx Error Code: ");
      Serial.println(error);
    }

  Driver1.begin(115200, SERIAL_8N1, -1, TX1);
  Driver2.begin(115200, SERIAL_8N1, -1, TX2);
  
  pinMode(ENC1_A, INPUT_PULLUP); pinMode(ENC1_B, INPUT_PULLUP);
  pinMode(ENC2_A, INPUT_PULLUP); pinMode(ENC2_B, INPUT_PULLUP);
  pinMode(ENC3_A, INPUT_PULLUP); pinMode(ENC3_B, INPUT_PULLUP);
  pinMode(ENC4_A, INPUT_PULLUP); pinMode(ENC4_B, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(ENC1_A), encoder1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC2_A), encoder2, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC3_A), encoder3, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC4_A), encoder4, CHANGE);
}

void loop() {
  // 1. SAFETY & MODE CHECK: Only allow locomotion if the robot mode is 0
  if (locoData.mode == 0) {
    
    // Trigger moves based on I2C data routed from the Main ESP32
    if (locoData.cross) { 
      if(!crossPressed){ startMove(FORWARD, 200.0); crossPressed=true; }
    } else crossPressed=false;

    if (locoData.circle) { 
      if(!circlePressed){ startMove(RIGHT, 15.0); circlePressed=true; }
    } else circlePressed=false;

    if (locoData.square) { 
      if(!squarePressed){ startMove(LEFT, 15.0); squarePressed=true; }
    } else squarePressed=false;

  } else {
    // Force stop locomotion if STM32 switched to Arm or Stepper mode
    crossPressed = false; circlePressed = false; squarePressed = false;
    if (motorRunning) {
      stopMotor();
      motorRunning = false;
      currentMode = IDLE;
      Serial.println("Mode changed! Motors forcefully stopped.");
    }
  }

  // 2. EXECUTE MOTION LOGIC
  if (motorRunning) {
    adjustSpeedsToTarget(); // Increment/Decrement speed by 1 based on RPM
    applyMotorCommands();   // Send those speeds to the drivers

    long avgPulse = (abs(pulse1) + abs(pulse2) + abs(pulse3) + abs(pulse4)) / 4;
    if (avgPulse >= targetPulses) {
      stopMotor();
      motorRunning = false;
      currentMode = IDLE;
      Serial.println("Target Reached.");
    }
  }
  
  // 3. PRINT DEBUG DATA
  printDebugData();
  
  delay(50);
}