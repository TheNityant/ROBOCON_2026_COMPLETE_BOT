#include <PS4Controller.h>
#include <HardwareSerial.h>

// ================= UART PORTS =================
HardwareSerial MDDS_LEFT(1);   // UART1 -> Left MDDS (Controls Left Front & Left Rear)
HardwareSerial MDDS_RIGHT(2);  // UART2 -> Right MDDS (Controls Right Front & Right Rear)

#define TX_LEFT   16
#define TX_RIGHT  17

// ================= SPEED CONFIG =================
// Max speed for your protocol (0-63)
#define MAX_PWM 63
#define DEADZONE 10 // Ignore small joystick drift

// ================= ENUMS =================
typedef enum {
  UART_SIG_CW  = 0,
  UART_SIG_CCW = 1,
} uart_dir_t;

typedef enum {
  MOTOR_FRONT = 0,   // Channel 1 on MDDS (Assumed Front)
  MOTOR_REAR  = 1,   // Channel 2 on MDDS (Assumed Rear)
} uart_motor_t;

// ================= UART SEND =================
// UPDATED: Logic matched to your motor_write function
void sendMDDS(HardwareSerial &uart, uart_motor_t motor, uart_dir_t dir, uint8_t speed) {
  uint8_t cmd = 0;
  speed &= 0x3F; // Limit speed to 6 bits (0-63)

  cmd = speed;
  if (motor) cmd |= (1 << 7); // Set 7th bit for channel (Front=0, Rear=1)
  if (dir)   cmd |= (1 << 6); // Set 6th bit for direction

  uart.write(cmd);
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);

  // Initialize UARTs
  MDDS_LEFT.begin(115200, SERIAL_8N1, -1, TX_LEFT);
  MDDS_RIGHT.begin(115200, SERIAL_8N1, -1, TX_RIGHT);

  PS4.begin("d0:40:57:b6:f4:b1");
  Serial.println("PS4 Connecting...");
}

// ================= LOOP =================
void loop() {
  if (!PS4.isConnected()) return;

  // 1. INPUTS
  int Ly = PS4.LStickY(); // LStickY: Forward/Backward
  int Lx = PS4.LStickX(); // LStickX: Strafe Left/Right
  int Rx = PS4.RStickX(); // RStickX: Turn Left/Right

  // Deadzone filter
  if (abs(Ly) < DEADZONE) Ly = 0;
  if (abs(Lx) < DEADZONE) Lx = 0;
  if (abs(Rx) < DEADZONE) Rx = 0;

  // 2. MECANUM MIXING ALGORITHM
  long target_LF = Ly + Lx + Rx; // Left Front
  long target_LR = Ly - Lx + Rx; // Left Rear
  long target_RF = Ly - Lx - Rx; // Right Front
  long target_RR = Ly + Lx - Rx; // Right Rear

  // 3. NORMALIZE SPEEDS
  long max_val = max(abs(target_LF), max(abs(target_LR), max(abs(target_RF), abs(target_RR))));
  
  if (max_val > 127) {
    target_LF = (target_LF * 127) / max_val;
    target_LR = (target_LR * 127) / max_val;
    target_RF = (target_RF * 127) / max_val;
    target_RR = (target_RR * 127) / max_val;
  }

  // 4. MAP TO YOUR MAX PWM (0-63)
  // Note: Corrected mapping range source to 127 to match joystick scaling
  int speed_LF = map(abs(target_LF), 0, 127, 0, MAX_PWM);
  int speed_LR = map(abs(target_LR), 0, 127, 0, MAX_PWM);
  int speed_RF = map(abs(target_RF), 0, 127, 0, MAX_PWM);
  int speed_RR = map(abs(target_RR), 0, 127, 0, MAX_PWM);

  // 5. DETERMINE DIRECTION BITS
  uart_dir_t dir_LF = (target_LF >= 0) ? UART_SIG_CW : UART_SIG_CCW;
  uart_dir_t dir_LR = (target_LR >= 0) ? UART_SIG_CW : UART_SIG_CCW;
  
  uart_dir_t dir_RF = (target_RF >= 0) ? UART_SIG_CCW : UART_SIG_CW;
  uart_dir_t dir_RR = (target_RR >= 0) ? UART_SIG_CCW : UART_SIG_CW;

  // 6. SEND DATA
  sendMDDS(MDDS_LEFT, MOTOR_FRONT, dir_LF, speed_LF);
  sendMDDS(MDDS_LEFT, MOTOR_REAR,  dir_LR, speed_LR);

  sendMDDS(MDDS_RIGHT, MOTOR_FRONT, dir_RF, speed_RF);
  sendMDDS(MDDS_RIGHT, MOTOR_REAR,  dir_RR, speed_RR);

  delay(20);
}