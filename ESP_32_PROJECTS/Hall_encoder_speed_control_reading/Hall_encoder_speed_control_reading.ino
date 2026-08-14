#include <HardwareSerial.h>

// --- Pin Definitions ---
// Motor control (one-way serial)
const int MOTOR_RX_PIN = -1; // Not used
const int MOTOR_TX_PIN = 16; // UART2 TX

// Encoder input
const int ENCODER_PIN_A = 26; // Encoder Channel A input

// --- RPM Calculation Settings ---
// !!! IMPORTANT: You MUST change this value to match your motor's encoder!
const int PULSES_PER_REVOLUTION = 230;

// How often to calculate and print RPM (in milliseconds)
const long RPM_CALC_INTERVAL = 500; // Calculate 2 times per second

// --- Global Variables ---
// Use UART Port 2 (Serial2) for motor control
HardwareSerial MotorSerial(2);

// Volatile variable to store pulse count from the interrupt
volatile long pulseCount = 0;

// Time tracking for RPM calculation
unsigned long lastRpmCalcTime = 0;

// --- Function Prototypes ---
void setMotorSpeed(int speed);
void calculateAndPrintRPM();


/**
 * @brief Interrupt Service Routine (ISR)
 * This function is called automatically every time a RISING edge
 * is detected on ENCODER_PIN_A.
 * It must be as fast as possible.
 */
void IRAM_ATTR countPulse() {
  pulseCount++;
}

void setup() {
  // Start the main Serial port for debugging
  Serial.begin(9600); // Using 9600 for debug output
  Serial.println("MDDS-10 Serial Control with RPM Feedback (Simple)");

  // --- Motor Serial Setup ---
  // Start the Motor Serial port
  MotorSerial.begin(9600, SERIAL_8N1, MOTOR_RX_PIN, MOTOR_TX_PIN);
  Serial.println("Motor Serial (UART2) started on TX Pin 16.");

  // --- Encoder/RPM Setup ---
  // Set the encoder pin as an input with an internal pull-up resistor
  pinMode(ENCODER_PIN_A, INPUT_PULLUP);
  
  // Attach the interrupt to the encoder pin
  // It will call countPulse() on every RISING edge
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A), countPulse, RISING);
  Serial.println("Encoder interrupt attached to Pin 26.");

  Serial.println("Setup complete. Running...");
}


void loop() {
  // Example: Set a constant speed for Motor 2
  // Speed from -200 (full reverse) to +200 (full forward).
  // 0 is stop.
  setMotorSpeed(100); // This was your original speed value
  
  // Handle calculating and printing the RPM
  calculateAndPrintRPM();

  // Add a small delay so we don't spam the motor driver
  delay(0); 
}

/**
 * @brief Calculates and prints the motor's RPM based on pulse count.
 * This function is non-blocking and uses the RPM_CALC_INTERVAL.
 */
void calculateAndPrintRPM() {
  // Check if it's time to calculate RPM
  if (millis() - lastRpmCalcTime >= RPM_CALC_INTERVAL) {
    
    // --- Critical Section ---
    // Safely copy the pulse count by temporarily disabling interrupts
    noInterrupts();
    long pulses = pulseCount;
    // Reset the pulse count for the next interval
    pulseCount = 0;
    interrupts();
    // --- End Critical Section ---

    // Calculate RPM
    // (pulses / PPR) = revolutions
    // (60000ms / interval) = conversion factor from (revolutions / interval_ms) to (revolutions / minute)
    float rpm = (float)pulses / PULSES_PER_REVOLUTION * (60000 / RPM_CALC_INTERVAL);

    // Print the calculated RPM
    Serial.print("Encoder RPM: ");
    Serial.println(rpm);

    // Update the last calculation time
    lastRpmCalcTime = millis();
  }
}


/**
 * @brief Sets the motor speed using SIMPLIFIED SERIAL mode for MOTOR 2.
 * @param speed Speed from -200 (full reverse) to +200 (full forward).
 *
 * This function uses your original logic.
 * It controls MOTOR 2 (commands 128-255).
 * 128 = Full Reverse, 192 = Stop, 255 = Full Forward
 */
void setMotorSpeed(int speed) {
  // Clamp speed to -200 to +200
  speed = constrain(speed, -100, 100);

  // Map speed to the simplified serial 7-bit range for Motor 2
  // The MSB (8th bit) is 1 for Motor 2
  byte data = map(speed, -100, 100, 128, 255);

  // Send the single command byte
  MotorSerial.write(data);
}