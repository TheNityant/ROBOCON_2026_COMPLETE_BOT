#include <PS4Controller.h>
#include <Wire.h>

// --- I2C CONFIGURATION (Shared between both ESPs) ---
#define I2C_SLAVE_ADDR 0x08
#define I2C_SDA 4 // Safe pin for both boards
#define I2C_SCL 5 // Safe pin for both boards

// --- STM32 GPIO PINS (ESP32 Outputs to STM32) ---
const int BIT_0 = 21; // LSB
const int BIT_1 = 22;
const int BIT_2 = 23;
const int BIT_3 = 19; 
const int BIT_4 = 18; // MSB 

// --- MODE SIGNAL FROM STM32 ---
const int MODE_INPUT_PIN = 32; // Reads the HIGH pulse from STM32 User Button

// --- LOCOMOTION DATA STRUCTURE (Sent via I2C) ---
struct LocomotionData {
  uint8_t cross;
  uint8_t circle;
  uint8_t square;
  uint8_t mode; // <--- The Locomotion ESP reads this to know if it should move
} locoData;

// --- ROBOT MODE TRACKER ---
int robotMode = 0; // 0 = Locomotion, 1 = Arm, 2 = Stepper
bool lastSignalState = false;
static uint8_t last_cmd = 255;

void setup() {
  Serial.begin(115200);
  
  // Explicitly initialize I2C Master with safe pins
  Wire.setPins(I2C_SDA, I2C_SCL);
  Wire.begin(); 
  Wire.setClock(50000);

  pinMode(BIT_0, OUTPUT);
  pinMode(BIT_1, OUTPUT);
  pinMode(BIT_2, OUTPUT);
  pinMode(BIT_3, OUTPUT);
  pinMode(BIT_4, OUTPUT);
  
  // Input pin to listen to the STM32 pulse
  pinMode(MODE_INPUT_PIN, INPUT_PULLDOWN); 
  
  if (!PS4.begin("14:b5:cd:15:d5:5b")) { 
    Serial.println("Error: Bluetooth initialization failed!");
    while(1);
  }
  Serial.println("ESP32 Router Started. Waiting for PS4...");
}

void loop() {
  static bool disconnectedMsgSent = false;

  // 1. LISTEN TO THE STM32 FOR MODE CHANGES (Runs regardless of PS4 connection)
  bool currentSignalState = digitalRead(MODE_INPUT_PIN);
  if (currentSignalState == HIGH && lastSignalState == LOW) {
    robotMode++;
    if (robotMode > 2) robotMode = 0;
    Serial.printf("\n>>> STM32 COMMANDED MODE SWITCH TO: %d <<<\n", robotMode);
  }
  lastSignalState = currentSignalState;


  if (PS4.isConnected()) {
    disconnectedMsgSent = false;
    uint8_t cmd = 0;
    String buttonName = "IDLE";

    // 2. DEFAULT SAFE STATES
    locoData.cross = 0; 
    locoData.circle = 0; 
    locoData.square = 0;
    locoData.mode = robotMode; // Always attach the current mode
    cmd = 0;

    // 3. ROUTE COMMANDS BASED ON MODE
    if (robotMode == 0) {
      // --- LOCOMOTION MODE ---
      // STM32 'cmd' stays 0. Locomotion gets active data.
      locoData.cross  = PS4.Cross();
      locoData.circle = PS4.Circle();
      locoData.square = PS4.Square();
    } 
    else if (robotMode == 1 || robotMode == 2) {
      // --- ROBOTIC ARM / STEPPER MODE ---
      // Locomotion 'locoData' button states stay 0. STM32 gets active binary data.
      if (PS4.Cross())         { cmd = 1;  buttonName = "CROSS (Backward)"; }
      else if (PS4.Triangle()) { cmd = 2;  buttonName = "TRIANGLE (Forward)"; }
      else if (PS4.Square())   { cmd = 3;  buttonName = "SQUARE (Left)"; }
      else if (PS4.Circle())   { cmd = 4;  buttonName = "CIRCLE (Right)"; }
      else if (PS4.R2())       { cmd = 5;  buttonName = "R2 (Clockwise)"; }
      else if (PS4.L2())       { cmd = 6;  buttonName = "L2 (Anti-clockwise)"; }
      else if (PS4.Up())       { cmd = 7;  buttonName = "D-Pad UP"; }
      else if (PS4.Down())     { cmd = 8;  buttonName = "D-Pad DOWN"; }
      else if (PS4.Left())     { cmd = 9;  buttonName = "D-Pad LEFT"; }
      else if (PS4.Right())    { cmd = 10; buttonName = "D-Pad RIGHT"; }
      else if (PS4.L1())       { cmd = 11; buttonName = "L1 Bumper"; }
      else if (PS4.R1())       { cmd = 12; buttonName = "R1 Bumper"; }
      else if (PS4.Share())    { cmd = 13; buttonName = "SHARE"; }
      else if (PS4.Options())  { cmd = 14; buttonName = "OPTIONS"; }
      else if (PS4.L3())       { cmd = 15; buttonName = "L3"; }
      else if (PS4.R3())       { cmd = 16; buttonName = "R3"; }
      else if (PS4.PSButton()) { cmd = 17; buttonName = "PS Button"; }
      else                     { cmd = 0;  buttonName = "IDLE"; }

      if (cmd != last_cmd) {
        Serial.print("STM32 Transmitting: ");
        Serial.print(buttonName);
        Serial.print(" | Binary: ");
        Serial.print(bitRead(cmd, 4)); Serial.print(bitRead(cmd, 3));
        Serial.print(bitRead(cmd, 2)); Serial.print(bitRead(cmd, 1));
        Serial.println(bitRead(cmd, 0));
        last_cmd = cmd;
      }
    }

    // 4. EXECUTE HARDWARE OUTPUTS
    digitalWrite(BIT_0, bitRead(cmd, 0));
    digitalWrite(BIT_1, bitRead(cmd, 1));
    digitalWrite(BIT_2, bitRead(cmd, 2));
    digitalWrite(BIT_3, bitRead(cmd, 3));
    digitalWrite(BIT_4, bitRead(cmd, 4));

    Wire.beginTransmission(I2C_SLAVE_ADDR);
    Wire.write((uint8_t*)&locoData, sizeof(locoData));
    byte errorFailsafe = Wire.endTransmission(); // Declared with a different name to avoid conflicts

    if (errorFailsafe != 0) {
      Serial.print("I2C Failsafe Tx Error Code: ");
      Serial.println(errorFailsafe);
    }


  } else {
    // 5. FAILSAFE: Disconnect protocol
    digitalWrite(BIT_0, LOW); digitalWrite(BIT_1, LOW); digitalWrite(BIT_2, LOW);
    digitalWrite(BIT_3, LOW); digitalWrite(BIT_4, LOW);
    
    locoData.cross = 0; locoData.circle = 0; locoData.square = 0;
    locoData.mode = robotMode; 
    
    Wire.beginTransmission(0x08);
    Wire.write((uint8_t*)&locoData, sizeof(locoData));
    Wire.endTransmission();

    if (!disconnectedMsgSent) {
      Serial.println("Waiting for PS4 Controller to connect...");
      disconnectedMsgSent = true;
    }
  }
  delay(20); 
}