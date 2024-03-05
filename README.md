# Line Following Robot

## Introduction
This project is about a line-following robot designed to detect and follow a visual line mapped out on the surface. These types of robots are commonly used in industrial and educational settings for automation and teaching the fundamentals of robotics and programming. We use the [Polulu 3pi 32u4](https://www.pololu.com/category/280/3pi-plus-32u4-oled-robot)for this project. 

## Video Demonstration
A video demonstration of the robot in action can be viewed [here](https://youtu.be/0xheZ3MjT4w).

## Features
- Precise line detection and following capability.
- Adjustable speed and sensitivity settings.
- Modular design for easy customization.
- [Robotic Systems Block Diagram](Robotic Systems Block Diagram.png).

## Hardware Requirements - Polulu 3pi 32u4 
- Microcontroller 
- Line detection sensors
- Motors and motor driver circuit
- Battery
- Chassis and wheels

## Software Requirements
- IDE compatible with your microcontroller (Arduino IDE)

## Setup Instructions
1. Ensure all the hardware on the 3pi 32u4 robot work as expected.
2. Upload the provided code to the microcontroller.
3. Press the power button.

## Usage
After setting up the robot with the provided instructions, place it on a track with a predefined line. Turn on the robot, and it will start following the line autonomously. Adjust the speed and sensitivity settings  of the PID based controller from the main.ino file as needed to improve performance.


