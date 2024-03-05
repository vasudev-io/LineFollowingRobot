#include "motors.h"
#include "linesensor.h"
#include "encoders.h"
#include "kinematics.h"
#include "pid.h"
#include "linefollow.h"
#include "home.h"
#include "countdown.h"
#include "maze.h"

// Define states
#define STATE_INITIAL 0
#define STATE_LEAVE_HOME 1
#define STATE_FOUND_LINE 2
#define STATE_HOME 3

int state = STATE_INITIAL;

long oldCountLeft = 0;
long oldCountRight = 0;
unsigned long oldTime = 0;

unsigned long startTime = 0;
bool returnedToStart = false;

float kp = 17;
float ki = 0.018;
float kd = 600;
float baseSpeed = 40;

boolean led_state;

Motors motors;
LineSensor lineSensors;
PID pidHead(&motors, kp, ki, kd, baseSpeed);
Kinematics kinematics;
LineFollow lineFollow(motors, lineSensors, pidHead, kinematics);
Maze maze(motors, lineFollow,lineSensors);
Home box(motors, pidHead);

void setup() {

  motors.initialise();
  lineSensors.initialise();
  setupEncoder0();
  setupEncoder1();
  kinematics.initialise(0, 0);
  pidHead.initialise();
  
  pinMode(LED_PIN, OUTPUT);
  led_state = false;
  
  Serial.begin(9600);
  delay(100);
  Serial.println("***RESET***");

  // Set initial state
  state = STATE_INITIAL;
  startTime = millis();

  // ROCKY FINAL COUNTDOWN SONG ON BUZZER
  //firstLine();
  //secondLine();
  //thirdLine();

}


void loop() {
    kinematics.update(count_L, count_R);

    // Return to start position after 27 seconds if a dead end is detected
    if (millis() - startTime >= 25000 && !returnedToStart && lineFollow.deadEnd()) {
      box.returnToStart(kinematics);
      firstLine();
      returnedToStart = true;
      state = STATE_HOME;
    }
  
    // Check initial state and perform calibration
    if (state == STATE_INITIAL) {
        initialSetup();
    } 
    // Once calibration is done, check if the robot has left the home area
    else if (state == STATE_LEAVE_HOME) {
        leaveHome();
    } 
    // If the line is found, start following it
    else if (state == STATE_FOUND_LINE) {
        maze.solve();
    } 
    // Handle the return to home state
    else if (state == STATE_HOME) {
        motors.drive(0, 0); 
        pidHead.setMode(IDLE);
    }
}


void initialSetup() {
    box.calibration();
    state = STATE_LEAVE_HOME; // Leave box after calibration
}

void leaveHome() {
    box.checkOutBox();
    if (box.outOfBox) {
        pidHead.setMode(LINE_FOLLOWING);
        if (lineFollow.onLine()) {
            state = STATE_FOUND_LINE; // Follow line once found
        }
    }
}
