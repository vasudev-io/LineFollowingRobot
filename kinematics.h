#ifndef KINEMATICS_H
#define KINEMATICS_H

#include <math.h> 
#include "encoders.h" 

// Constants definition 
#define UPDATE_INTERVAL 2 // Update interval in milliseconds
const float WHEEL_RADIUS = 0.016; // Wheel radius in meters
const float BOT_WIDTH = 0.0839; // Distance between wheels in meters
const float ENCODER_COUNTS_PER_REVOLUTION = 358.3; // Encoder resolution
const float WHEEL_CIRCUMFERENCE = 2 * M_PI * WHEEL_RADIUS; // Circumference of the wheel
const float DISTANCE_PER_ENCODER_COUNT = WHEEL_CIRCUMFERENCE / ENCODER_COUNTS_PER_REVOLUTION; // Distance per encoder tick

unsigned long previousUpdateTime;

class Kinematics {
public:
    Kinematics() {}

    // Initialise kinematics data
    void initialise(long leftEncoderCount, long rightEncoderCount) {
        lastLeftEncoder = leftEncoderCount;
        lastRightEncoder = rightEncoderCount;
    }

    // Update the robot's position and orientation
    void update(float currentLeftEncoderCount, float currentRightEncoderCount) {
        if (millis() - previousUpdateTime >= UPDATE_INTERVAL) {
            updatePositionAndOrientation(currentLeftEncoderCount, currentRightEncoderCount);
            previousUpdateTime = millis(); // Update the last update timestamp
        }
    }

    // Getters for position and orientation
    float getXPos() const { return positionX; }
    float getYPos() const { return positionY; }
    float getTheta() const { return orientation; }

private:
    float positionX = 0.0, positionY = 0.0, orientation = 0.0;
    float lastLeftEncoder = 0.0, lastRightEncoder = 0.0;

    // Handle position and orientation update logic
    void updatePositionAndOrientation(float currentLeftEncoder, float currentRightEncoder) {
        auto encoderDeltaLeft = currentLeftEncoder - lastLeftEncoder;
        auto encoderDeltaRight = currentRightEncoder - lastRightEncoder;

        auto distanceLeftWheel = encoderDeltaLeft * DISTANCE_PER_ENCODER_COUNT;
        auto distanceRightWheel = encoderDeltaRight * DISTANCE_PER_ENCODER_COUNT;

        auto changeInOrientation = (distanceRightWheel - distanceLeftWheel) / BOT_WIDTH;

        if (distanceLeftWheel != distanceRightWheel) {
            performArcMovement(distanceLeftWheel, distanceRightWheel, changeInOrientation);
        } else {
            performStraightMovement((distanceLeftWheel + distanceRightWheel) / 2, changeInOrientation);
        }

        normalizeOrientation();

        // Update encoder values for next iteration
        lastLeftEncoder = currentLeftEncoder;
        lastRightEncoder = currentRightEncoder;
    }

    // Normalize the robot's orientation to keep it within -PI to PI
    void normalizeOrientation() {
        while (orientation > M_PI) orientation -= 2 * M_PI;
        while (orientation < -M_PI) orientation += 2 * M_PI;
    }

    // Handle movement along an arc
    void performArcMovement(float distanceLeftWheel, float distanceRightWheel, float changeInOrientation) {
        float radiusOfCurvature = BOT_WIDTH * (distanceLeftWheel + distanceRightWheel) / (2 * (distanceRightWheel - distanceLeftWheel));
        float ICC_X = positionX - radiusOfCurvature * sin(orientation);
        float ICC_Y = positionY + radiusOfCurvature * cos(orientation);

        positionX = cos(changeInOrientation) * (positionX - ICC_X) - sin(changeInOrientation) * (positionY - ICC_Y) + ICC_X;
        positionY = sin(changeInOrientation) * (positionX - ICC_X) + cos(changeInOrientation) * (positionY - ICC_Y) + ICC_Y;
        orientation += changeInOrientation;
    }

    // Handle straight movement
    void performStraightMovement(float distanceMoved, float changeInOrientation) {
        positionX += distanceMoved * cos(orientation);
        positionY += distanceMoved * sin(orientation);
        orientation += changeInOrientation;
    }
};

#endif // KINEMATICS_H
