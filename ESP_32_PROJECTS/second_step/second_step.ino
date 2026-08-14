#include <Arduino.h>
#include <HardwareSerial.h>
#include <SPI.h>

// -------- HSPI PIN DEFINITIONS --------
#define HSPI_SCK   14
#define HSPI_MISO  12
#define HSPI_MOSI  13
#define HSPI_CS    15

SPIClass *hspi = NULL;

// -------- ENCODER PIN DEFINITIONS --------
const int enc1PinA = 34;
const int enc1PinB = 35;

const int enc2PinA = 25;
const int enc2PinB = 26;

// -------- MDDS UART (ONE PIN) --------
#define MDDS_TX 16
HardwareSerial MDDS(1);

// -------- SETTINGS --------
const int pulsesPerRev1 = 230;
const int pulsesPerRev2 = 230;
const unsigned long rpmInterval = 100;

// -------- P CONTROLLER --------
float KP = 0.15;              // Tune this
float targetRPM1 = 100;       // Target RPM motor 1
float targetRPM2 = 100;       // Target RPM motor 2

float speed1 = 25;             // 0–63
float speed2 = 25;

// -------- CONTROL FLAGS --------
bool motorEnable = false;
uint8_t motorDir = 0;         // 0 = Forward, 1 = Reverse

// -------- ENCODER DATA --------
volatile long pulseCount1 = 0;
volatile long pulseCount2 = 0;

portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

// -------- ENCODER ISRs --------
void IRAM_ATTR encoder1ISR() {
  portENTER_CRITICAL_ISR(&mux);
  if (digitalRead(enc1PinB) == LOW) pulseCount1++;
  else pulseCount1--;
  portEXIT_CRITICAL_ISR(&mux);
}

void IRAM_ATTR encoder2ISR() {
  portENTER_CRITICAL_ISR(&mux);
  if (digitalRead(enc2PinB) == LOW) pulseCount2++;
  else pulseCount2--;
  portEXIT_CRITICAL_ISR(&mux);
}

// -------- MDDS COMMAND --------
void sendMDDS(uint8_t channel, uint8_t dir, uint8_t speed) {
  speed &= 0x3F; // 0–63
  uint8_t cmd = (channel << 7) | (dir << 5) | speed;
  MDDS.write(cmd);
}

// -------- SETUP --------
void setup() {
  Serial.begin(115200);

  // MDDS UART
  MDDS.begin(115200, SERIAL_8N1, -1, MDDS_TX);


  // SPI
  hspi = new SPIClass(HSPI);
  hspi->begin(HSPI_SCK, HSPI_MISO, HSPI_MOSI, HSPI_CS);
  pinMode(HSPI_CS, OUTPUT);
  digitalWrite(HSPI_CS, HIGH);

  // Encoders
  pinMode(enc1PinA, INPUT_PULLUP);
  pinMode(enc1PinB, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(enc1PinA), encoder1ISR, RISING);

  pinMode(enc2PinA, INPUT_PULLUP);
  pinMode(enc2PinB, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(enc2PinA), encoder2ISR, RISING);

  Serial.println("MDDS RPM CONTROL (KP ONLY) READY");
  Serial.println("Commands: F=Forward | R=Reverse | S=Stop");
}

// -------- LOOP --------
void loop() {
  static unsigned long lastCalcTime = 0;
  static long lastP1 = 0, lastP2 = 0;
  static float rpm1 = 0, rpm2 = 0;
  static long safeP1 = 0, safeP2 = 0;

  // -------- SERIAL COMMANDS --------
  if (Serial.available()) {
    char cmd = Serial.read();

    if (cmd == 'F' || cmd == 'f') {
      motorEnable = true;
      motorDir = 0;
      Serial.println("FORWARD");
    }

    if (cmd == 'R' || cmd == 'r') {
      motorEnable = true;
      motorDir = 1;
      Serial.println("REVERSE");
    }

    if (cmd == 'S' || cmd == 's') {
      motorEnable = false;
      speed1 = 0;
      speed2 = 0;
      sendMDDS(0, motorDir, 0);
      sendMDDS(1, motorDir, 0);
      Serial.println("STOP");
    }
  }

  unsigned long now = millis();

  // -------- RPM CALCULATION --------
  if (now - lastCalcTime >= rpmInterval) {
    portENTER_CRITICAL(&mux);
    safeP1 = pulseCount1;
    safeP2 = pulseCount2;
    portEXIT_CRITICAL(&mux);

    rpm1 = (safeP1 - lastP1) * (60000.0 / (pulsesPerRev1 * (now - lastCalcTime)));
    rpm2 = (safeP2 - lastP2) * (60000.0 / (pulsesPerRev2 * (now - lastCalcTime)));

    lastP1 = safeP1;
    lastP2 = safeP2;
    lastCalcTime = now;

    // -------- P CONTROL --------
    if (motorEnable) {
      float error1 = targetRPM1 - rpm1;
      float error2 = targetRPM2 - rpm2;

      speed1 += KP * error1;//speed1=speed1+KP*error1
      speed2 += KP * error2;

      speed1 = constrain(speed1, 0, 63);
      speed2 = constrain(speed2, 0, 63);

      sendMDDS(0, motorDir, (uint8_t)speed1);
      sendMDDS(1, motorDir, (uint8_t)speed2);
    }

    Serial.printf(
      "RPM1: %.1f | SP1: %.0f || RPM2: %.1f | SP2: %.0f\n",
      rpm1, speed1, rpm2, speed2
    );
    Serial.println(safeP2);
  }

  // -------- SPI SEND --------
  static unsigned long lastSPITime = 0;
  if (millis() - lastSPITime > 100) {
    char msg[64];
    snprintf(msg, sizeof(msg), "%ld,%.1f,%ld,%.1f",
             safeP1, rpm1, safeP2, rpm2);

    hspi->beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
    digitalWrite(HSPI_CS, LOW);
    hspi->transferBytes((uint8_t *)msg, NULL, sizeof(msg));
    digitalWrite(HSPI_CS, HIGH);
    hspi->endTransaction();

    lastSPITime = millis();
  }
}
