#ifndef _MOTORS_H
#define _MOTORS_H

// Pin definitions for motor control
#define LEFT_PWM_PIN 10
#define LEFT_DIR_PIN 16
#define RIGHT_PWM_PIN 9
#define RIGHT_DIR_PIN 15

// Direction definitions for motor rotation
#define FORWARD LOW
#define REVERSE HIGH

class Motors {
private:
    int leftMotorSpeed = 0; // Current speed of the left motor
    int rightMotorSpeed = 0; // Current speed of the right motor

public:
    // Constructor
    Motors() {} 

    // Initializes motor control pins
    void initialise() {
        pinMode(LEFT_PWM_PIN, OUTPUT);
        pinMode(LEFT_DIR_PIN, OUTPUT);
        pinMode(RIGHT_PWM_PIN, OUTPUT);
        pinMode(RIGHT_DIR_PIN, OUTPUT);
    }

    // Drives motors with specified PWM values
    void drive(float leftPWM, float rightPWM) {
        // Maximum allowable PWM value
        const int MAX_PWM = 200;

        // Constrain and set left motor speed
        leftPWM = constrain(leftPWM, -MAX_PWM, MAX_PWM);
        digitalWrite(LEFT_DIR_PIN, leftPWM >= 0 ? FORWARD : REVERSE);
        analogWrite(LEFT_PWM_PIN, abs(leftPWM));

        // Constrain and set right motor speed
        rightPWM = constrain(rightPWM, -MAX_PWM, MAX_PWM);
        digitalWrite(RIGHT_DIR_PIN, rightPWM >= 0 ? FORWARD : REVERSE);
        analogWrite(RIGHT_PWM_PIN, abs(rightPWM));

        // Update speed tracking
        leftMotorSpeed = leftPWM;
        rightMotorSpeed = rightPWM;
    }

    // Returns current speed of the left motor
    int getLeftSpeed() {
        return leftMotorSpeed;
    }

    // Returns current speed of the right motor
    int getRightSpeed() {
        return rightMotorSpeed;
    }
};

#endif // _MOTORS_H
