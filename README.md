# Servo Control Projects

<p align="center">
  <img src="https://readme-raw.githubusercontent.com/yourusername/yourrepo/main/assets/team-raw-stm-blue.svg" alt="TEAM RAW STM SERVO PROJECTS" width="600"/>
</p>

## Overview
This repository contains multiple STM32-based servo control projects using the STM32F103 microcontroller. These projects demonstrate various approaches to controlling servos through different interfaces including UART, PS4 controllers, and integrated systems.

## Projects Included

### 1. SERVO_UART
**Single Servo Control via UART**
- Simple implementation for controlling one servo via UART communication
- Ideal for beginners learning embedded servo control
- Clean, modular code structure

### 2. MULTI_SERVO_UART
**Multi-Servo Control via UART**
- Controls multiple servos through a single UART interface
- Modular code design with HAL libraries
- Easily extendable for more servos
- Real-time servo management in embedded systems

### 3. PS4_SERVO_CONTROLLED
**PS4 Controller Servo Control**
- Control servos using a PlayStation 4 controller
- Interfaced via ESP receiver and STM32F103 microcontroller
- Real-time servo control with UART-based communication
- Perfect for remote robotic or automation projects

### 4. PS4_PICKING_MECHANISM_CONTROLLED
**PS4 Controller Picking Mechanism**
- Specialized implementation for picking mechanisms using PS4 controller
- Uses ESP receiver for controller data transmission
- Complete servo mapping and actuation system

### 5. SERVO_WORK_MODE
**Multiple Servo Work Modes**
- Explores different working modes for servo control
- Modular code for easy extension and experimentation
- Example implementations for position, speed, and sequence modes

### 6. SERVO_STEPPER_INTEGRATION
**Servo and Stepper Motor Integration**
- Combines servo and stepper motor control in one system
- Demonstrates integration of different motor types
- Advanced control techniques for mixed motor systems

### 7. PS4_SERVO_STEPPER_CTRL
**PS4 Controller with Servo & Stepper Control**
- Full implementation using PS4 controller for both servo and stepper motors
- Dual motor control via single controller interface
- Complex control system demonstration

### 8. PS4_SERVO_STEPPER_LOCO_CONTROLLED
**PS4 Controller Locomotion Control**
- Specialized locomotion control using PS4 controller
- Integration of servos and steppers for movement systems
- Advanced robotics control approach

### 9. R1_PICKING_MECHANISM
**R1 Picking Mechanism Control**
- Specific implementation for R1 picking mechanisms
- Designed for industrial or robotic picking applications

### 10. SERVO_ANGLE_CONTROL
**Servo Angle Control System**
- Precise angle control for servos
- Advanced positioning algorithms
- Calibration and control optimization

### 11. SERVO_ANGLE_CONTROL
**SERVO_ANGLE_CONTROL**
- Fine-grained servo angle control implementation
- Advanced positioning capabilities

### 12. STM_F4
**STM32F4 Based Projects**
- Projects designed for STM32F4 microcontroller series
- More powerful processor with enhanced capabilities
- Different peripheral configurations and features

### 13. ESP_32_PROJECTS
**ESP32-Based Robotic Control Projects**
- Comprehensive collection of ESP32-based projects for robotic control
- Includes servo control implementations with various sensors and interfaces
- WiFi-enabled robotic systems with wireless communication capabilities
- Integration of multiple motor types (servo, stepper) with ESP32 controllers
- Advanced IoT-based robotic control solutions with sensor feedback
- Projects include PS4 controller interfacing, ultrasonic sensing, and multi-motor control

## Getting Started

All projects are built using STM32CubeIDE and use HAL libraries. To get started:

1. Open the desired project in STM32CubeIDE
2. Configure your hardware peripherals (UART, timers, etc.)
3. Build and flash to your STM32 board
4. Follow specific instructions in each project's README for usage

## Demo Videos

<!-- YouTube Embed (recommended for GitHub) -->
[![Watch the demo](https://img.youtube.com/vi/YOUR_VIDEO_ID/0.jpg)](https://www.youtube.com/watch?v=YOUR_VIDEO_ID)

<!-- Alternative: Direct video link (works on some platforms, not GitHub) -->
<!--
<video width="600" controls>
  <source src="path_or_url_to_your_video.mp4" type="video/mp4">
  Your browser does not support the video tag.
</video>
-->

> **Note:** GitHub markdown does not support direct video embedding. For local or direct-link videos, use the HTML `<video>` tag as shown above, but it will only work on platforms that support HTML in markdown rendering (not on GitHub.com). For best compatibility, upload your video to YouTube or another video hosting service and use the thumbnail link method above.

## Features

- Modular, easy-to-understand code
- UART-based communication protocols
- Real-time servo control capabilities
- Support for multiple servo configurations
- Extensible architectures for custom implementations
- Integration with PS4 controllers via ESP receivers
- Advanced control modes and work patterns

## Requirements

- STM32CubeIDE (for building projects)
- STM32F103 or STM32F4 microcontroller board
- Servo motors (standard 180° or continuous rotation)
- ESP8266/ESP32 receiver for PS4 controller projects
- Basic understanding of embedded C programming

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This project is licensed under the MIT License - see the LICENSE file for details.

---
**TEAM RAW STM**