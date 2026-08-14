// You may need a specific library depending on your board (e.g., ESP32-PS4 or PS4Universal)
// We will use the common structure for PS4 library interaction.
#include <AccelStepper.h> 

// --- PS4 LIBRARY (Requires Bluetooth-capable board like ESP32/ESP8266) ---
// ** Switched to PS4Controller.h, which is common for ESP32 **
#include <PS4Controller.h> 

// Define the MAC Address of your PS4 Controller's Dongle or host device
// You will need to find the specific MAC address for your setup.
const char* BT_MAC_ADDRESS = "c0:bf:be:0c:e4:93"; // REPLACE WITH YOUR MAC ADDRESS

// -----------------------------------------------------------------
// 1. PIN DEFINITIONS
// -----------------------------------------------------------------
// --- MOTOR 1: LINEAR RACK & PINION (M1) ---
#define STEP_PIN_1            17 
#define DIR_PIN_1             13 
#define EN_PIN_1              16 

// --- MOTOR 2: ROTATIONAL SHAFT (M2) ---
#define STEP_PIN_2            27 
#define DIR_PIN_2             26 
#define EN_PIN_2              25 

// -----------------------------------------------------------------
// 2. PHYSICAL CONSTANTS
// -----------------------------------------------------------------
// Based on 6400 * 19 = 121,600 steps per 360 degree revolution
#define STEPS_PER_REV_360     (6400 * 19)

// --- M1 (RACK) CONSTANTS ---
#define PINION_DIAMETER_CM    6.0        
#define RACK_LENGTH_CM        21.5       
#define PI_CONST              3.1415926535

// Calculated steps per 1 cm of linear travel
const float STEPS_PER_CM = (float)STEPS_PER_REV_360 / (PINION_DIAMETER_CM * PI_CONST);

// --- MOVEMENT INCREMENTS ---
#define RACK_INCREMENT_CM     10.0      // Fixed linear move of 10 cm
#define SHAFT_INCREMENT_DEG   45.0      // Fixed angular move of 45 degrees

// -----------------------------------------------------------------
// 3. STEPPER OBJECTS & STATE
// -----------------------------------------------------------------
AccelStepper stepperM1(AccelStepper::DRIVER, STEP_PIN_1, DIR_PIN_1); // M1: Rack
AccelStepper stepperM2(AccelStepper::DRIVER, STEP_PIN_2, DIR_PIN_2); // M2: Shaft

// Rack Bounding
long maxStepsM1 = 0; 

// Button State Tracking (Prevents executing a move thousands of times per second)
bool l1_pressed_prev = false;
bool r1_pressed_prev = false;
bool up_pressed_prev = false;
bool down_pressed_prev = false;


// -----------------------------------------------------------------
// 4. PS4 CALLBACK & SETUP
// -----------------------------------------------------------------
void onConnect() {
  Serial.println("PS4 Controller Connected.");
}

void onDisconnect() {
  Serial.println("PS4 Controller Disconnected.");
}

void setup() {
  Serial.begin(9600);
  
  // Calculate maximum steps for Rack Bounding
  maxStepsM1 = (long)(RACK_LENGTH_CM * STEPS_PER_CM);

  // --- PS4 SETUP ---
  // Object changed from Ps4 to PS4
  PS4.attachOnConnect(onConnect);
  PS4.attachOnDisconnect(onDisconnect);
  // Initialize Bluetooth/PS4 connection
  PS4.begin(BT_MAC_ADDRESS); 
  
  // --- MOTOR 1 (Rack) Setup ---
  pinMode(EN_PIN_1, OUTPUT);
  digitalWrite(EN_PIN_1, LOW); 
  stepperM1.setMaxSpeed(100 * 1000); 
  stepperM1.setAcceleration(10 * 1000);
  stepperM1.setCurrentPosition(0); 

  // --- MOTOR 2 (Shaft) Setup ---
  pinMode(EN_PIN_2, OUTPUT);
  digitalWrite(EN_PIN_2, LOW); 
  stepperM2.setMaxSpeed(100 * 1000); 
  stepperM2.setAcceleration(10 * 1000);
  stepperM2.setCurrentPosition(0); 

  Serial.println("✅ System Ready. Awaiting PS4 Connection...");
}

// -----------------------------------------------------------------
// 5. MAIN LOOP
// -----------------------------------------------------------------
void loop() {
  // Update the PS4 controller state
  // Removed Ps4.task() and changed object to PS4
  if (PS4.isConnected()) {
    handlePs4Commands();
  }

  // Both motors run simultaneously to execute any commanded moves
  stepperM1.run();
  stepperM2.run();
}


// -----------------------------------------------------------------
// 6. PS4 COMMAND EXECUTION LOGIC
// -----------------------------------------------------------------
void handlePs4Commands() {
  
  // --- RACK (M1) COMMANDS: Up/Down D-Pad ---
  // Changed object from Ps4 to PS4
  handleRackMove(PS4.Up(), RACK_INCREMENT_CM, up_pressed_prev);     // Up Arrow = +10 cm
  handleRackMove(PS4.Down(), -RACK_INCREMENT_CM, down_pressed_prev); // Down Arrow = -10 cm

  // --- SHAFT (M2) COMMANDS: L1/R1 Shoulder Buttons ---
  // Changed object from Ps4 to PS4
  handleShaftRotate(PS4.L1(), SHAFT_INCREMENT_DEG, l1_pressed_prev);     // L1 = +45 degrees
  handleShaftRotate(PS4.R1(), -SHAFT_INCREMENT_DEG, r1_pressed_prev);    // R1 = -45 degrees
}


// --- M1 Helper Function: Rack Linear Move ---
void handleRackMove(bool current_state, float distanceCM, bool &prev_state) {
  // Check for button press (rising edge) and motor is not currently moving
  if (current_state && !prev_state && stepperM1.distanceToGo() == 0) {
    long currentPos = stepperM1.currentPosition();
    long stepsToMove = (long)(distanceCM * STEPS_PER_CM);
    long desiredPosition = currentPos + stepsToMove;

    // Bounding Check (Prevent pinion from leaving the rack)
    if (desiredPosition < 0) {
      desiredPosition = 0;
      Serial.println("⚠️ M1 Bounding Hit! Moving to 0 limit.");
    } else if (desiredPosition > maxStepsM1) {
      desiredPosition = maxStepsM1;
      Serial.printf("⚠️ M1 Bounding Hit! Moving to Max limit (%ld).\n", maxStepsM1);
    }
    
    // Command the move
    stepperM1.moveTo(desiredPosition);
    Serial.printf("✅ M1 Rack Move: %.1f cm. Target: %ld steps\n", distanceCM, desiredPosition);
  }
  prev_state = current_state; // Update previous state for next loop
}


// --- M2 Helper Function: Shaft Angular Move ---
void handleShaftRotate(bool current_state, float angleDEG, bool &prev_state) {
  // Check for button press (rising edge) and motor is not currently moving
  if (current_state && !prev_state && stepperM2.distanceToGo() == 0) {
    
    // Command the move
    long stepsToMove = (long)((angleDEG * (float)STEPS_PER_REV_360) / 360.0);
    long targetPosition = stepperM2.currentPosition() + stepsToMove;
    stepperM2.moveTo(targetPosition);
    
    Serial.printf("✅ M2 Shaft Move: %.1f°. Target: %ld steps\n", angleDEG, targetPosition);
  }
  prev_state = current_state; // Update previous state for next loop
}