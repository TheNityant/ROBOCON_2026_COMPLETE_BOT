#include <HardwareSerial.h>
#include <PS4Controller.h> // Ensure you install the "PS4Controller" library by aed3

// ------------ TWO MDDS10 DRIVERS ------------
HardwareSerial Driver1(1);   // MDDS10 #1
HardwareSerial Driver2(2);   // MDDS10 #2

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

// ------------ PHYSICAL CONSTANTS ------------
const int PPR = 270;
const float WHEEL_DIAMETER = 1.2;
const float CIRCUMFERENCE = PI * WHEEL_DIAMETER;
const float PULSES_PER_METER = PPR / CIRCUMFERENCE;

// ------------ VARIABLES ------------
volatile long pulse1 = 0;
volatile long pulse2 = 0;
volatile long pulse3 = 0;
volatile long pulse4 = 0;

long targetPulses = 0;
bool motorRunning = false;
bool directionForward = true;
bool crossButtonPressed = false;

// ------------ UART ENUM ------------
typedef enum {
  UART_SIG_CW = 0,
  UART_SIG_CCW = 1,
} uart_ctrl_dir_t;

// ------------ UART WRITE FUNCTION ------------
void uartWrite(HardwareSerial &driver, uint8_t channel, uart_ctrl_dir_t dir, uint8_t speed)
{
  speed &= 0x3F;
  uint8_t udata = (channel << 7) | (dir << 6) | speed;
  driver.write(udata);
}

// ------------ MOTOR CONTROL ------------
void forwardMotor()
{
  uartWrite(Driver1, 0, UART_SIG_CW, 18);
  uartWrite(Driver1, 1, UART_SIG_CCW, 18);

  uartWrite(Driver2, 0, UART_SIG_CW, 18);
  uartWrite(Driver2, 1, UART_SIG_CCW, 18);
}

void reverseMotor()
{
  uartWrite(Driver1, 0, UART_SIG_CW, 15);
  uartWrite(Driver1, 1, UART_SIG_CW, 15);

  uartWrite(Driver2, 0, UART_SIG_CW, 15);
  uartWrite(Driver2, 1, UART_SIG_CCW, 15);
}

void stopMotor()
{
  uartWrite(Driver1, 0, UART_SIG_CCW, 0);
  uartWrite(Driver1, 1, UART_SIG_CCW, 0);

  uartWrite(Driver2, 0, UART_SIG_CCW, 0);
  uartWrite(Driver2, 1, UART_SIG_CCW, 0);
}

// ------------ ENCODER ISR ------------
void IRAM_ATTR encoder1() {
  static int lastA = 0;
  int A = digitalRead(ENC1_A);
  int B = digitalRead(ENC1_B);

  if (A != lastA) {
    if (A == B) {
      pulse1++;
    } else {
      pulse1--;
    }
    lastA = A; 
  }
}

void IRAM_ATTR encoder2() {
  static int lastA = 0;
  int A = digitalRead(ENC2_A);
  int B = digitalRead(ENC2_B);

  if (A != lastA) {
    if (A == B) pulse2++;
    else pulse2--;
    lastA = A;
  }
}

void IRAM_ATTR encoder3() {
  static int lastA = 0;
  int A = digitalRead(ENC3_A);
  int B = digitalRead(ENC3_B);

  if (A != lastA) {
    if (A == B) pulse3++;
    else pulse3--;
    lastA = A;
  }
}

void IRAM_ATTR encoder4() {
  static int lastA = 0;
  int A = digitalRead(ENC4_A);
  int B = digitalRead(ENC4_B);

  if (A != lastA) {
    if (A == B) pulse4++;
    else pulse4--;
    lastA = A;
  }
}

// ------------ START MOVEMENT HELPER ------------
void startMovement(float distance) {
  if (distance == 0) return;

  noInterrupts();
  pulse1 = 0;
  pulse2 = 0;
  pulse3 = 0;
  pulse4 = 0;
  interrupts();

  if (distance > 0) {
    directionForward = true;
    Serial.println("Moving Forward");
  } 
  else {
    directionForward = false;
    distance = abs(distance);
    Serial.println("Moving Reverse");
  }

  targetPulses = distance * PULSES_PER_METER;

  Serial.print("Target Pulses: ");
  Serial.println(targetPulses);

  motorRunning = true;
}

// ------------ SETUP ------------
void setup()
{
  Serial.begin(115200);

  // Initialize PS4 Controller (Replace with your ESP32's Bluetooth MAC address)
  PS4.begin("14:b5:cd:15:c5:5a"); 

  Driver1.begin(115200, SERIAL_8N1, -1, TX1);
  Driver2.begin(115200, SERIAL_8N1, -1, TX2);

  pinMode(ENC1_A, INPUT_PULLUP);
  pinMode(ENC1_B, INPUT_PULLUP);

  pinMode(ENC2_A, INPUT_PULLUP);
  pinMode(ENC2_B, INPUT_PULLUP);

  pinMode(ENC3_A, INPUT_PULLUP);
  pinMode(ENC3_B, INPUT_PULLUP);

  pinMode(ENC4_A, INPUT_PULLUP);
  pinMode(ENC4_B, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(ENC1_A), encoder1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC2_A), encoder2, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC3_A), encoder3, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC4_A), encoder4, CHANGE);

  Serial.println("Enter Distance (+ Forward , - Reverse)");
  Serial.println("Or press 'X' on PS4 controller for 10 units distance");
}

// ------------ LOOP ------------
void loop()
{
  // 1. Check Serial Input
  if (Serial.available() > 0)
  {
    float distance = Serial.parseFloat();
    startMovement(distance);
  }

  // 2. Check PS4 Input (Cross/X Button)
  if (PS4.isConnected()) {
    if (PS4.data.button.cross) {
      if (!crossButtonPressed) { // Trigger only once per press
        startMovement(10.0);
        crossButtonPressed = true;
      }
    } else {
      crossButtonPressed = false; // Reset when button is released
    }
  }

  long p1, p2, p3, p4;
  long avgPulse;

  noInterrupts();
  p1 = pulse1;
  p2 = pulse2;
  p3 = pulse3;
  p4 = pulse4;
  interrupts();

  avgPulse = (abs(p1) +abs(p2) +abs(p3) +abs(p4)) / 4;

  if (motorRunning)
  {
    if (directionForward)
      forwardMotor();
    else
      reverseMotor();
  }

  if (motorRunning && abs(avgPulse) >= targetPulses)
  {
    stopMotor();
    motorRunning = false;
    Serial.println("Target Distance Reached -> Motor Stopped");
  }

  // -------- PRINT ALL ENCODERS --------
  Serial.print("M1: ");
  Serial.print(p1);

  Serial.print(" | M2: ");
  Serial.print(p2);

  Serial.print(" | M3: ");
  Serial.print(p3);

  Serial.print(" | M4: ");
  Serial.print(p4);

  Serial.print(" | AVG: ");
  Serial.print(avgPulse);
  Serial.print(" | target : ");
  Serial.println(targetPulses);
  delay(20);
}