#include <PS4Controller.h>

// -------- Binary Output Pins --------
const int BIT_0 = 21; // LSB
const int BIT_1 = 22;
const int BIT_2 = 23;
const int BIT_3 = 19; 
const int BIT_4 = 18; // MSB 

// -------- Relay --------
#define RELAY_PIN 16

void setup() {
  Serial.begin(115200);

  // Binary pins
  pinMode(BIT_0, OUTPUT);
  pinMode(BIT_1, OUTPUT);
  pinMode(BIT_2, OUTPUT);
  pinMode(BIT_3, OUTPUT);
  pinMode(BIT_4, OUTPUT);

  // Relay
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);  // OFF (Active LOW)

  // Make sure this MAC address matches your controller pairing
  if (!PS4.begin("18:b5:cd:16:d5:5b")) { 
    Serial.println("Error: Bluetooth initialization failed!");
    while(1);
  }
  Serial.println("ESP32 Started. Please pair your PS4 controller.");
}

void loop() {
  static bool disconnectedMsgSent = false;

  if (PS4.isConnected()) {
    disconnectedMsgSent = false; // Reset connection message flag
    uint8_t cmd = 0;
    String buttonName = "IDLE";

    // -------- Button Mapping --------
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

    // Print to Serial Monitor ONLY when a button state changes
    static uint8_t last_cmd = 255;
    if (cmd != last_cmd) {
      Serial.print("ESP32 Transmitting: ");
      Serial.print(buttonName);
      Serial.print(" | Binary: ");
      Serial.print(bitRead(cmd, 4));
      Serial.print(bitRead(cmd, 3));
      Serial.print(bitRead(cmd, 2));
      Serial.print(bitRead(cmd, 1));
      Serial.println(bitRead(cmd, 0));
      
      // -------- Relay Control --------
      if (cmd == 2) { // TRIANGLE -> ON
        digitalWrite(RELAY_PIN, LOW);
        Serial.println("Relay ON");
      } else if (cmd == 4) { // CIRCLE -> OFF
        digitalWrite(RELAY_PIN, HIGH);
        Serial.println("Relay OFF");
      }

      last_cmd = cmd;
    }

    // Convert integer 'cmd' to 5 hardware bits
    digitalWrite(BIT_0, bitRead(cmd, 0));
    digitalWrite(BIT_1, bitRead(cmd, 1));
    digitalWrite(BIT_2, bitRead(cmd, 2));
    digitalWrite(BIT_3, bitRead(cmd, 3));
    digitalWrite(BIT_4, bitRead(cmd, 4));

  } else {
    // Failsafe: Send 0 if controller disconnects
    digitalWrite(BIT_0, LOW);
    digitalWrite(BIT_1, LOW);
    digitalWrite(BIT_2, LOW);
    digitalWrite(BIT_3, LOW);
    digitalWrite(BIT_4, LOW);
    
    // Failsafe: Turn Relay OFF
    digitalWrite(RELAY_PIN, HIGH); 

    if (!disconnectedMsgSent) {
      Serial.println("Waiting for PS4 Controller to connect...");
      disconnectedMsgSent = true;
    }
  }
  delay(20); 
}