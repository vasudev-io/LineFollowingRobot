#ifndef _PID_H
#define _PID_H

#include "encoders.h" 
#include "motors.h" 

// Set operational modes of the PID controller
enum PIDMode {
    LINE_FOLLOWING, 
    HOME_STRETCH,   
    TURNING,        
    IDLE            
};

class PID {
private:
    Motors* motorControl; 
    unsigned long previousTimestamp; 
    float previousError; 
    float errorRate; 
    float cumulativeError; 
    PIDMode currentMode; 

    // PID tuning parameters
    float proportionalGain; 
    float integralGain;     
    float derivativeGain;   
    float baseVelocity;     

public:
    float controlOutput; 

    // Constructor initialises the PID controller with motors interface and tuning parameters
    PID(Motors* motorInterface, float kp, float ki, float kd, float baseSpeed)
        : motorControl(motorInterface), proportionalGain(kp), integralGain(ki), derivativeGain(kd), baseVelocity(baseSpeed) {
        currentMode = LINE_FOLLOWING; // Default mode
        initialise(); // Initialize internal variables
    }

    // Sets the operational mode of the PID controller and re-initialises
    void setMode(PIDMode mode) {
        currentMode = mode;
        initialise();
    }

    // Initialises internal state of the PID controller
    void initialise() {
        previousTimestamp = millis();
        previousError = 0.0;
        errorRate = 0.0;
        cumulativeError = 0.0;
    }

    void update(float sensorInput) {
        computePID(sensorInput); // Compute feedback
        applyControl(); // Handle PID mode
    }

private:
    // Computes the PID control output
    void computePID(float sensorInput) {
        unsigned long currentTime = millis();
        unsigned long elapsedTime = currentTime - previousTimestamp;
        previousTimestamp = currentTime;

        if (elapsedTime > 0) {
            errorRate = (sensorInput - previousError) / static_cast<float>(elapsedTime);
            cumulativeError += sensorInput * static_cast<float>(elapsedTime);
        }
        previousError = sensorInput;

        // Calculate total control output
        controlOutput = proportionalGain * sensorInput + derivativeGain * errorRate + integralGain * cumulativeError;
    }

    // Handling of PID mode
    void applyControl() {
        // Handle control differently based on mode
        if (currentMode == LINE_FOLLOWING) {
            lineFollowingControl();
        } else if (currentMode == HOME_STRETCH) {
            homeStretchControl();
        } else if (currentMode == TURNING) {
            turningControl();
        } else if (currentMode == IDLE) {
            idleControl();
        }
    }

    // Control functions for each mode
    void lineFollowingControl() {
        // Adjust motor speeds for line following
        motorControl->drive(round(baseVelocity - controlOutput), round(baseVelocity + controlOutput));
    }

    void homeStretchControl() {
        // Drive straight with adjusted speed
        int speed = constrain(round(baseVelocity + controlOutput), 60, 80);
        motorControl->drive(speed, speed);
    }

    void turningControl() {
        // Adjust both motors similarly for turning
        int speed = constrain(round(baseVelocity + controlOutput), 60, 80);
        motorControl->drive(speed, speed);
    }

    void idleControl() {
        motorControl->drive(0, 0);
    }
};

#endif
