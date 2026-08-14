# 🤖 Servo & Embedded Control Projects

<p align="center">
  <strong>A collection of STM32 and ESP32-based experiments exploring servo control, motor control, controller interfaces, sensing, and robotic mechanisms.</strong>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/STM32-03234B?style=flat-square&logo=stmicroelectronics&logoColor=white" alt="STM32">
  <img src="https://img.shields.io/badge/ESP32-E7352C?style=flat-square&logo=espressif&logoColor=white" alt="ESP32">
  <img src="https://img.shields.io/badge/Embedded_C-00599C?style=flat-square&logo=c&logoColor=white" alt="Embedded C">
  <img src="https://img.shields.io/badge/STM32CubeIDE-03234B?style=flat-square&logo=stmicroelectronics&logoColor=white" alt="STM32CubeIDE">
</p>

---

## 🎥 Demonstration

<p align="center">
  <a href="YOUR_YOUTUBE_URL">
    <img src="YOUR_VIDEO_THUMBNAIL_URL"
         width="800"
         alt="Servo and robotic control demonstration">
  </a>
</p>

<p align="center">
  <i>Hardware demonstration of servo and robotic control systems.</i>
</p>

---

## 🧭 Overview

This repository preserves a collection of embedded-control projects developed while working with **STM32 and ESP32 microcontrollers**.

The projects range from basic single-servo experiments to multi-servo systems, controller-driven mechanisms, sensor-based actuation, stepper motor integration, and robotic locomotion control.

The STM32 projects primarily use **STM32CubeIDE, HAL peripherals, UART communication, and embedded C**, while the ESP32 collection explores additional controller and sensor interfaces.

---

# 🔵 STM32 Projects

## 1. SERVO_UART

**Single-servo control through UART**

Basic servo control using UART communication between the host/interface and STM32.

---

## 2. MULTI_SERVO_UART

**Multi-servo control through UART**

Controls multiple servos through a shared UART-based interface.

Focus areas:

- Multi-servo communication
- UART handling
- STM32 HAL
- Real-time servo control

---

## 3. PS4_SERVO_CONTROLLED

**PS4 controller → ESP receiver → STM32 → servo**

A controller-based servo control system using an ESP-based receiver to transmit PS4 controller input to the STM32.

---

## 4. PS4_PICKING_MECHANISM_CONTROLLED

**PS4-controlled robotic picking mechanism**

A servo-control implementation developed for operating a robotic picking mechanism through a PS4 controller interface.

---

## 5. SERVO_WORK_MODE

**Servo operating-mode experiments**

Experiments with different servo operating modes and motion patterns for embedded robotic applications.

---

## 6. SERVO_STEPPER_INTEGRATION

**Servo and stepper motor integration**

Combines servo and stepper motor control within a single embedded system.

---

## 7. PS4_SERVO_STEPPER_CTRL

**PS4-controlled servo and stepper system**

Integrates PS4 controller input with coordinated servo and stepper motor control.

---

## 8. PS4_SERVO_STEPPER_LOCO_CONTROLLED

**Controller-based locomotion control**

A locomotion-oriented control implementation combining servo and stepper actuation with PS4 controller input.

---

## 9. R1_PICKING_MECHANISM

**R1 picking mechanism control**

Control implementation developed around the R1 robotic picking mechanism.

---

## 10. SERVO_ANGLE_CONTROL

**Servo positioning and angle control**

Experiments focused on servo positioning, angle control, and calibration.

---

## 11. STM_F4

**STM32F4 control experiments**

Embedded-control projects developed on the STM32F4 platform, exploring peripherals and control implementations on a higher-performance STM32 family.

---

# 🟠 ESP32 Projects

The `ESP_32_PROJECTS` collection contains additional experiments involving servo control, sensors, controller interfaces, and motor control.

### Servo Control

| Project | Focus |
|---|---|
| `SERVO` | Basic UART servo control with velocity-mode support |
| `MULTI_SERVO_USER_DEGREE_CONTROLLED` | Individual servo positioning using user-defined angles |
| `SERVO_CW_CCW` | Clockwise and counter-clockwise continuous rotation |
| `SERVO_SIMPLE_SWEEP` | Servo sweep between defined angle limits |
| `SERVO_USER_CONTROLLED` | User-controlled servo positioning |
| `USER_CONTROLLED_SERVO` | Alternative user-driven servo control implementation |

### Sensor Integration

| Project | Focus |
|---|---|
| `ULTRASONIC_SERVO_CONTROLLED` | Servo positioning based on ultrasonic distance |
| `MULTI_SERVO_USER_DISTANCE_CONTROLLED` | Multi-servo control using distance input |
| `SERVP_CHECK_CODE` | Sensor and servo-system verification |

### Controller Interface

| Project | Focus |
|---|---|
| `PS4_I2C_STM` | PS4 controller interface using Bluetooth and ESP-based communication |

### Stepper Control

| Project | Focus |
|---|---|
| `STEPPER_MOTOR_CONTROL` | Stepper motor control experiments |

### Servo Bus / Configuration

| Project | Focus |
|---|---|
| `SERVO_ID_CHANGE` | Servo ID configuration for multi-servo bus systems |

---

# ⚙️ Technical Areas

### Embedded Systems

- STM32F103
- STM32F4
- ESP32
- STM32 HAL
- STM32CubeIDE
- Embedded C
- UART communication
- Peripheral and timer configuration

### Motion Control

- Servo control
- Multi-servo coordination
- Servo positioning
- Stepper motor control
- Servo + stepper integration
- Robotic mechanism actuation
- Locomotion control

### Interfaces & Sensors

- PS4 controller interfaces
- ESP-based receiver systems
- UART communication
- Ultrasonic sensing
- Distance-based actuation

---

# 🔌 Example Control Architecture

Several controller-driven projects follow an architecture similar to:

```text
             PS4 Controller
                    │
                    ▼
              ESP Receiver
                    │
                   UART
                    │
                    ▼
                STM32 MCU
                    │
             ┌──────┴──────┐
             ▼             ▼
          Servos        Steppers
             │             │
             └──────┬──────┘
                    ▼
             Robotic Mechanism
