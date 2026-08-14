#include <SCServo.h>

SMS_STS st;

#define RX_PIN 25
#define TX_PIN 26

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("Starting ST3215 ID scan...");

  Serial2.begin(1000000, SERIAL_8N1, RX_PIN, TX_PIN);
  delay(200);

  st.pSerial = &Serial2;

  for (int id = 0; id <= 253; id++) {
    int res = st.Ping(id);
    if (res != -1) {
      Serial.print("📡 Servo found at ID: ");
      Serial.println(id);
    }
    delay(10);
  }

  Serial.println("Scan complete.");
}

void loop(){}
