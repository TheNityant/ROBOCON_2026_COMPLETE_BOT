#include <SPI.h>

// Define the Master HSPI pins
#define HSPI_MISO 12
#define HSPI_MOSI 13
#define HSPI_SCLK 14
#define HSPI_CS   15

// Define the Slave VSPI pins
#define VSPI_MISO 19
#define VSPI_MOSI 23
#define VSPI_SCLK 18
#define VSPI_CS   5

// Create two SPI instances
SPIClass hspi(HSPI);
SPIClass vspi(VSPI);

void setup() {
  Serial.begin(115200);

  // Initialize Master (HSPI)
  hspi.begin(HSPI_SCLK, HSPI_MISO, HSPI_MOSI, HSPI_CS);
  pinMode(HSPI_CS, OUTPUT);

  // Initialize Slave (VSPI)
  // Note: For actual Slave functionality, you would use an ESP32 Slave Library
  vspi.begin(VSPI_SCLK, VSPI_MISO, VSPI_MOSI, VSPI_CS);
 
  Serial.println("Buses initialized...");
}

void loop() {
  // Master sending data
  hspi.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  digitalWrite(HSPI_CS, LOW);
  hspi.transfer(8); // Sending the byte
  digitalWrite(HSPI_CS, HIGH);
  hspi.endTransaction();

  delay(1000);
}
