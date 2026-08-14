// Encoder RPM Measurement with Serial Monitor Output
// By ChatGPT (for Jai Lohar)

const int encoderPin = 17;      // Encoder signal input pin (interrupt capable)
volatile unsigned long pulseCount = 0;  // To store number of pulses (1s detected)
unsigned long prevCount = 0;   // Store previous count to calculate change
unsigned long prevMillis = 0;  // Time marker
const int PPR = 200;             // Pulses Per Revolution
const unsigned long interval = 1000; // Update every 1 second

void setup() {
  Serial.begin(9600);
  pinMode(encoderPin, INPUT);
  
  // Attach interrupt for rising edge detection
  attachInterrupt(digitalPinToInterrupt(encoderPin), countPulse, RISING);

  Serial.println("Encoder RPM Measurement Started...");
  Serial.println("----------------------------------");
  delay(1000);
}

void loop() {
  unsigned long currentMillis = millis();

  // Update every second
  if (currentMillis - prevMillis >= interval) {
    prevMillis = currentMillis;

    unsigned long currentCount = pulseCount; // Read current pulse count
    unsigned long ones = currentCount;       // "Ones" = pulses counted

    // Calculate RPM for last 1 second
    unsigned long pulsesInOneSecond = currentCount - prevCount;
    prevCount = currentCount;

    float revolutions = (float)pulsesInOneSecond / PPR;
    float rpm = revolutions * 60.0;  // Because 1 second = 1/60 minute

    // Print values to Serial Monitor
    Serial.print("Pulses (1s): ");
    Serial.print(pulsesInOneSecond);
    Serial.print(" | Total Ones: ");
    Serial.print(ones);
    Serial.print(" | Revolutions: ");
    Serial.print(revolutions, 2);
    Serial.print(" | RPM: ");
    Serial.println(rpm, 2);
  }
}

void countPulse() {
  pulseCount++;  // Count each rising edge (1)
}

