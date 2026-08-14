


# Team Raw- SFIT: Waveshare ST3215 Servo Projects

Welcome to the official repository of Team Raw- SFIT, featuring a comprehensive collection of Arduino projects for the **Waveshare ST3215 Servo**. This repository is designed to help students, hobbyists, and professionals control and experiment with the ST3215 servo using a variety of methods and sensors.

![Waveshare ST3215 Servo](https://www.waveshare.com/img/devkit/servo/servo-3215-1.jpg)

---

## Table of Contents
- [About the Project](#about-the-project)
- [Project Structure](#project-structure)
- [Project Descriptions](#project-descriptions)
- [Getting Started](#getting-started)
- [Credits](#credits)
- [License](#license)

---

## About the Project
This repository is a showcase of different ways to control the Waveshare ST3215 servo motor using Arduino. Each folder contains a unique `.ino` file demonstrating a specific control method, from basic sweeps to advanced sensor integration.

**Main Highlight:**
> This project is maintained and developed by **Team Raw- SFIT** for educational and collaborative purposes. Contributions and forks are welcome!

---

## Project Structure
- **MULTI_SERVO_USER_DEGREE_CONTROLLED/**
	- Control multiple servos with user-defined angles.
- **MULTI_SERVO_USER_DISTANCE_CONTROLLED/**
	- Control multiple servos based on distance sensor input.
- **SERVO/**
	- Basic servo control example.
- **SERVO_CW_CCW/**
	- Control servo rotation in both clockwise and counterclockwise directions.
- **SERVO_ID_CHANGE/**
	- Change the ID of the servo for multi-servo setups.
- **SERVO_SIMPLE_SWEEP/**
	- Simple sweep motion for the servo.
- **SERVO_USER_CONTROLLED/**
	- User input-based servo control.
- **ULTRASONIC_SERVO_CONTROLLED/**
	- Servo control using ultrasonic sensor feedback.
- **USER_CONTROLLED_SERVO/**
	- Another example of user-controlled servo operation.

---

## Project Descriptions


### 1. MULTI_SERVO_USER_DEGREE_CONTROLLED
Allows users to set and control the angle of multiple Waveshare ST3215 servos individually through serial input. Ideal for projects requiring precise, user-defined positioning of several servos, such as robotic arms or animatronics.


### 2. MULTI_SERVO_USER_DISTANCE_CONTROLLED
Demonstrates how to control multiple servos based on real-time distance measurements from sensors. Useful for automation, interactive installations, or obstacle-avoidance systems where servo positions respond to environmental changes.


### 3. SERVO
Basic example for initializing and controlling a single Waveshare ST3215 servo. Shows how to send position commands and verify wiring, making it a great starting point for beginners.


### 4. SERVO_CW_CCW
Demonstrates continuous clockwise and counterclockwise rotation of the servo, including how to alternate direction and control speed. Useful for applications needing repeated or oscillating motion.


### 5. SERVO_ID_CHANGE
Shows how to change the unique ID of a Waveshare ST3215 servo via Arduino, enabling control of multiple servos on a single communication bus. Essential for scalable multi-servo projects.


### 6. SERVO_SIMPLE_SWEEP
Performs a smooth sweep motion from one angle limit to the other, repeatedly. Great for testing servo range, smoothness, and mechanical setup.


### 7. SERVO_USER_CONTROLLED
Enables real-time user control of the servo position, typically via a potentiometer or serial input. Perfect for interactive demos or manual positioning tasks.


### 8. ULTRASONIC_SERVO_CONTROLLED
Integrates an ultrasonic distance sensor with the servo, allowing the servo to react to objects detected at varying distances. Useful for proximity-based automation or robotics.


### 9. USER_CONTROLLED_SERVO
Another example of user-driven servo control, offering a different approach or interface for learning and experimentation with manual servo positioning.

---

## Getting Started
1. **Hardware Required:**
	 - Waveshare ST3215 Servo
	 - Arduino Board (Uno, Mega, etc.)
	 - Jumper wires, breadboard, and additional sensors as needed
2. **Software:**
	 - Arduino IDE
	 - Required libraries (see individual `.ino` files for dependencies)
3. **Instructions:**
	 - Open the desired `.ino` file in Arduino IDE
	 - Connect your hardware as per the comments in the code
	 - Upload and experiment!

---

## Credits
This project would not have been possible without the dedication and expertise of the following developers:

- [TheNItyant](https://github.com/TheNityant)
- [JAy B lohar](https://github.com/jaylohar7qd)

Special thanks to all contributors and the open-source community for their support and inspiration.

---

## License
This repository is open-source and available under the MIT License. See the LICENSE file for more details.
