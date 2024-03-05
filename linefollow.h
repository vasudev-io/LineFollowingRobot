#ifndef LINEFOLLOW_H
#define LINEFOLLOW_H

#include "motors.h"
#include "linesensor.h"
#include "pid.h"
#include "kinematics.h"

#define LED_PIN 13

class LineFollow {
private:
    Motors& motors;
    LineSensor& lineSensors;
    Kinematics& kinematics;
    PID& pidController;
    
    // Timing and duration constants for actions
    unsigned long actionStartTime = 0;
    const unsigned long turnDurationLeft = 175;
    const unsigned long turnDurationRight = 175;
    const unsigned long deadEndDuration = 100;

    // Turn counter for making decisions at intersections
    int leftTurnCounter = 0;
    int rightTurnCounter = 0;

    // Sensor threshold values for detecting the line
    const int thresholdLeft = 1200; 
    const int thresholdMidLeft = 1300; 
    const int thresholdMid = 1400; 
    const int thresholdMidRight = 1300; 
    const int thresholdRight = 1100;
    float readings[5];

    // Current line following status
    bool isTurning = false;
    bool isHandlingDeadEnd = false;

    // Last known line direction
    

    int turnSpeed = 25;

    

public:
    LineFollow(Motors& motorsRef, LineSensor& lineSensorsRef, PID& pidRef, Kinematics& kinematicsRef)
        : motors(motorsRef), lineSensors(lineSensorsRef), pidController(pidRef), kinematics(kinematicsRef) {}

    enum LineDirection { UNKNOWN, LEFT, RIGHT };
    LineDirection lastLineDirection = UNKNOWN;

    // Line following logic
    void followLine() {
        unsigned long currentTime = millis();

        if (isTurning && currentTime - actionStartTime >= turnDurationRight) {
            isTurning = false; // Turn completed
        }

        if (isHandlingDeadEnd && currentTime - actionStartTime >= deadEndDuration) {
            isHandlingDeadEnd = false; // Dead end handling completed
        }

        if (!isTurning && !isHandlingDeadEnd) {
            if (deadEnd()) {
                handleDeadEnd();
            } else {
                updateSensorReadings();
                updateLineDirection();
                adjustToLine();
            }
        }
    }

    // Robot is on the line
    bool onLine() {
        updateSensorReadings();
        return readings[2] >= thresholdMid;
    }

    // Robot has encountered a dead end
    bool deadEnd() {
        updateSensorReadings();
        for (int reading : readings) {
            if (reading >= 900) {
                return false; // Line detected by at least one sensor
            }
        }
        return true; // No sensor detects the line
    }

private:
    // Update all sensors
    void updateSensorReadings() {
        for (int i = 0; i < 5; i++) {
            readings[i] = lineSensors.readLineSensor(i);
        }
    }

    // Handle encountered dead end
    void handleDeadEnd() {
        motors.drive(-turnSpeed, turnSpeed); // Initiate dead end maneuver
        actionStartTime = millis();
        isHandlingDeadEnd = true;
    }

    // Handle right turn
    void turnRight() {
        motors.drive(turnSpeed, -turnSpeed); // Initiate right turn
        actionStartTime = millis();
        isTurning = true;
    }

    // PID adjustment
    void adjustToLine() {
        float lineMeasurement = calculateLinePosition();
        if (onLine()) {
            pidController.update(lineMeasurement); // Adjust motor speed based on line position
        } else {
            searchForLine(); // Search for the line if it's not currently detected
        }
        updateLEDStatus(onLine());
    }

    // Calculate weighted position of the line relative to the robot
    float calculateLinePosition() {
        float leftValue = readings[1];
        float rightValue = readings[3];
        float sum = leftValue + rightValue;
        return constrain((leftValue / sum) * 2.0 - (rightValue / sum) * 2.0, -1.0, 1.0);
    }

    // Robot to search for the line
    void searchForLine() {
        motors.drive(lastLineDirection == RIGHT ? turnSpeed : -turnSpeed, 
                     lastLineDirection == RIGHT ? -turnSpeed : turnSpeed);
    }

    // Update the last direction for based on weighted sensor readings
    void updateLineDirection() {
        float leftSum = readings[0] + readings[1];
        float rightSum = readings[3] + readings[4];
        lastLineDirection = leftSum > rightSum ? LEFT : RIGHT;
    }

    // Light LED when robot is on the line
    void updateLEDStatus(bool onLine) {
        digitalWrite(LED_PIN, onLine ? HIGH : LOW);
    }
};

#endif // LINEFOLLOW_H
