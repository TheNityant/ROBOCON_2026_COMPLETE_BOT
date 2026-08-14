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
const int enc1PinA = 32;
const int enc1PinB = 33;

const int enc2PinA = 25;
const int enc2PinB = 26;

// -------- MDDS UART (ONE PIN ONLY) --------
#define MDDS_TX 16
HardwareSerial MDDS(1);

// -------- SETTINGS --------
const int pulsesPerRev1 = 230;
const int pulsesPerRev2 = 230;

const uint8_t motorSpeed1 = 25;
const uint8_t motorSpeed2 = 25;

const unsigned long rpmInterval = 100;

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
  speed &= 0x3F;
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

  Serial.println("MDDS Dual Motor | One UART Pin | Ready");
}

// -------- LOOP --------
void loop() {
  static unsigned long lastCalcTime = 0;
  static long lastP1 = 0, lastP2 = 0;
  static float rpm1 = 0, rpm2 = 0;
  static long safeP1 = 0, safeP2 = 0;

  // -------- SERIAL CONTROL --------
  if (Serial.available()) {
    char cmd = Serial.read();

    if (cmd == 'f' || cmd == 'F') {
      sendMDDS(0, 0, motorSpeed1); // Motor 1 forward
      sendMDDS(1, 0, motorSpeed2); // Motor 2 forward
    }

    if (cmd == 's' || cmd == 'S') {
      sendMDDS(0, 0, 0);
      sendMDDS(1, 0, 0);
    }
  }

  // -------- RPM CALC --------
  unsigned long now = millis();
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

    Serial.printf("M1 RPM: %.1f | M2 RPM: %.1f\n", rpm1, rpm2);
    Serial.println(safeP2);
    lastSPITime = millis();
  }
}
