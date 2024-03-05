#ifndef _HOME_H
#define _HOME_H

#define DRIVE_TIME 600 // Time taken to exit home box 

#include "encoders.h" 
#include "motors.h" 
#include "linesensor.h" 
#include "countdown.h" 

class Home {
private:
    unsigned long driveStartTime = 0;
    Motors& motors; 
    PID& pidHead; 
    LineSensor& lineSensors; 

    void initialiseLED() {
        pinMode(LED_BUILTIN, OUTPUT);
    }
    
    void signalLED(unsigned long duration, bool state) {
        digitalWrite(LED_BUILTIN, state ? HIGH : LOW);
        if (duration > 0) {
            delay(duration);
            digitalWrite(LED_BUILTIN, !state ? HIGH : LOW);
        }
    }

    void initialiseMotorsAndSensors() {
        motors.initialise();
        lineSensors.initialise();
    }

    // Calculate the required encoder counts the specified rotation angle
    int calculateRotationEncoderCounts(float angle) {
        float wheelCircumference = 2 * PI * WHEEL_RADIUS;
        float distancePerWheel = angle * (BOT_WIDTH / 2);
        return (distancePerWheel / wheelCircumference) * ENCODER_COUNTS_PER_REVOLUTION;
    }

    // Calculate the required encoder counts for driving the specified distance
    int calculateDistanceEncoderCounts(float distance) {
        float wheelCircumference = 2 * PI * WHEEL_RADIUS;
        return (distance / wheelCircumference) * ENCODER_COUNTS_PER_REVOLUTION;
    }

    // Waits until the robot has moved the target number of encoder counts
    void waitForTargetEncoderCounts(int targetCounts) {
        int startLeft = count_L;
        int startRight = count_R;
        while (true) {
            int movedLeft = abs(count_L - startLeft);
            int movedRight = abs(count_R - startRight);
            int averageMoved = (movedLeft + movedRight) / 2;
            if (averageMoved >= targetCounts) break;
            delay(10);
        }
    }

public:
    Home(Motors& motorsRef, PID& pidHeadRef) : motors(motorsRef), pidHead(pidHeadRef) {}

    bool outOfBox = false; 
    bool timerStarted = false; 

    // Reset drive state
    void reset() {
        outOfBox = false;
        timerStarted = false;
    }

    // Checks that robot has driven out of the box at start
    void checkOutBox() {
        if (!timerStarted) {
            driveStartTime = millis();
            timerStarted = true;
        } else if (!outOfBox && millis() - driveStartTime >= DRIVE_TIME) {
            outOfBox = true;
            timerStarted = false;
        }

        if (!outOfBox) {
            driveForwards();
        }
    }

    // Calibrate all
    void calibration() {
        motors.drive(0, 0);
        initialiseLED();
        signalLED(2000, true);
        initialiseMotorsAndSensors();
        signalLED(1000, true);
        reset();
    }

    // Guides the robot to return to the start position
    void returnToStart(Kinematics& kinematics) {
        float currentX = kinematics.getXPos();
        float currentY = kinematics.getYPos();
        float currentTheta = kinematics.getTheta();
        float angleToStart = atan2(-currentY, -currentX) - currentTheta;

        rotateTo(angleToStart);

        float distanceToStart = sqrt(pow(currentX, 2) + pow(currentY, 2));
        
        driveStraightPID(distanceToStart);

        firstLine(); //ROCKY VICTORY SONG!!!!!
    }

    // Rotates the robot to a specified angle
    void rotateTo(float angle) {
        int targetCounts = calculateRotationEncoderCounts(angle);
        motors.drive(angle > 0 ? 30 : -30, angle > 0 ? -30 : 30);
        waitForTargetEncoderCounts(abs(targetCounts));
        motors.drive(0, 0); // Stop
    }

    // Drives the robot straight for a specified distance using PID control
    void driveStraightPID(float distance) {
        int targetCounts = calculateDistanceEncoderCounts(distance);
        
        int startLeftEncoderCount = count_L;
        int startRightEncoderCount = count_R;
    
        pidHead.setMode(HOME_STRETCH);
    
        while (true) {
            int currentLeftCounts = count_L - startLeftEncoderCount;
            int currentRightCounts = count_R - startRightEncoderCount;
            int averageCurrentCounts = (currentLeftCounts + currentRightCounts) / 2;
    
            float error = currentLeftCounts - currentRightCounts;
            pidHead.update(error); 

            if (averageCurrentCounts >= targetCounts) { 
                motors.drive(0,0);
                break;
            }
            
            delay(10);
        }
        
        pidHead.setMode(LINE_FOLLOWING);
    }

    // Simple PID controlled straight drive
    void driveForwards() {
        pidHead.setMode(HOME_STRETCH);
        pidHead.update(0); // Target error = 0 for coming back home
    }
};

#endif
