#ifndef MAZE_H
#define MAZE_H

#include "linefollow.h"
#include "linesensor.h"
#include "motors.h"

class Maze {
private:
    LineFollow& lineFollower;
    LineSensor& lineSensor;
    Motors& motor;

    // Possible robot states during maze navigation
    enum RobotState { FOLLOWING_LINE, TURNING_LEFT, TURNING_RIGHT, MAKING_U_TURN};
    RobotState currentState = FOLLOWING_LINE;

    enum LineDirection { UNKNOWN, LEFT, RIGHT };
    LineDirection lastDirection = UNKNOWN;

    // Constants
    unsigned long actionStart = 0;
    unsigned long turnStart = 0;
    bool isTurning = false;
    int leftTurnCount = 0;
    int rightTurnCount = 0;
    const float turnDurationLeft = 175;
    const float turnDurationRight = 175;
    const int turnSpeed = 25;

    // Sensor thresholds for detecting the line - for printed sheets with black line
    const int sensorThresholdLeft = 1200; 
    const int sensorThresholdMidLeft = 1300; 
    const int sensorThresholdMid = 1400; 
    const int sensorThresholdMidRight = 1300; 
    const int sensorThresholdRight = 1100; 

public:
    Maze(Motors& motorsRef, LineFollow& lineFollowerRef, LineSensor& lineSensorsRef) 
        : motor(motorsRef), lineFollower(lineFollowerRef), lineSensor(lineSensorsRef) {}

    void solve() {
        if (currentState == FOLLOWING_LINE) {
            followLineLogic();
        } else {
            executeTurnLogic();
        }
    
    }
    

private:
    
    void followLineLogic() {

        // Weighted PID based line following
        lineFollower.followLine();

        // Reading sensor values from all sensors
        bool foundLeft = lineSensor.readLineSensor(0) > sensorThresholdLeft;
        bool foundRight = lineSensor.readLineSensor(4) > sensorThresholdRight;
        bool foundStraight = lineSensor.readLineSensor(1) > sensorThresholdMidLeft || 
                             lineSensor.readLineSensor(2) > sensorThresholdMid || 
                             lineSensor.readLineSensor(3) > sensorThresholdMidRight;

        // Decision making based on sensor readings
        if (foundStraight && foundLeft && foundRight) {
            processIntersection();
        } else if (foundRight) {
            processRightTurn();
        } else if (foundStraight) {
            resetTurnCounters();
        } else if (foundLeft) {
            processLeftTurn();
        } else {
            initiateUTurn();
        }
    }

    // Handles intersection
    void processIntersection() {
        if (leftTurnCount >= 2 && rightTurnCount >= 2) {
            initiateTurn(TURNING_LEFT);
            resetTurnCounters();
        } else if (rightTurnCount >= 2) {
            initiateTurn(TURNING_RIGHT);
            rightTurnCount++;
        } else {
            initiateTurn(TURNING_RIGHT);
            rightTurnCount++;
        }
    }

    // Handles right turn
    void processRightTurn() {
        if (rightTurnCount < 2) {
            initiateTurn(TURNING_RIGHT);
            rightTurnCount++;
        } else {
            rightTurnCount = 0;
            initiateUTurn();
        }
    }

    // Handles left turn logic.
    void processLeftTurn() {
        if (leftTurnCount < 2) {
            initiateTurn(TURNING_LEFT);
            leftTurnCount++;
        } else {
            leftTurnCount = 0;
            initiateUTurn();
        }
    }

    // Turn based on the given RobotState
    void initiateTurn(RobotState newState) {
        currentState = newState;
        turnStart = millis();
        isTurning = true;
        if (newState == TURNING_LEFT) leftTurnCount++;
        else leftTurnCount = 0;
    }

    // Executes the turning logic based on the current state and elapsed time since the turn started
    void executeTurnLogic() {
        unsigned long elapsed = millis() - turnStart;
        if ((currentState == TURNING_LEFT && elapsed < turnDurationLeft) ||
            (currentState == TURNING_RIGHT && elapsed < turnDurationRight)) {
            motor.drive((currentState == TURNING_LEFT ? -1 : 1) * turnSpeed, 
                        (currentState == TURNING_LEFT ? 1 : -1) * turnSpeed);
        } else if (currentState == MAKING_U_TURN && elapsed < turnDurationRight) {
            motor.drive(-turnSpeed, turnSpeed);
        } else {
            currentState = FOLLOWING_LINE;
            isTurning = false;
        }
    }

    // Resets counters used to track consecutive turns
    void resetTurnCounters() {
        currentState = FOLLOWING_LINE;
        rightTurnCount = 0;
        leftTurnCount = 0;
    }
    
    // U-turn
    void initiateUTurn() {
        initiateTurn(MAKING_U_TURN);
        resetTurnCounters();
    }
};

#endif // MAZE_H
