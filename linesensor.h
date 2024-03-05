// this #ifndef stops this file
// from being included mored than
// once by the compiler. 
#ifndef _LINESENSOR_H
#define _LINESENSOR_H


#define EMIT_PIN 11    // EMIT 
#define LS_LEFT_PIN 12   // DN1
#define LS_MIDLEFT_PIN 18 // DN2
#define LS_MIDDLE_PIN 20 //DN3
#define LS_MIDRIGHT_PIN 21 //DN4
#define LS_RIGHT_PIN 22 //DN5

int ls_pins[5] = {LS_LEFT_PIN,
                  LS_MIDLEFT_PIN,
                  LS_MIDDLE_PIN,
                  LS_MIDRIGHT_PIN,
                  LS_RIGHT_PIN };

#define MAX_SAMPLES 10
float results[MAX_SAMPLES];

class LineSensor {
  public:
    LineSensor() {

    } 

void initialise() {
        pinMode(EMIT_PIN, INPUT); // Set EMIT as an input (off)
        for (int i = 0; i < 5; i++) {
            pinMode(ls_pins[i], INPUT); // Set each line sensor pin to input
        }
        Serial.begin(9600);
        delay(1500);
        Serial.println("***RESET***");
    }

    float readLineSensor(int number) {
        // Check that input is within valid range
        if (number < 0 || number > 4) {
            return -1; // Error
        }

        pinMode(EMIT_PIN, OUTPUT);
        digitalWrite(EMIT_PIN, HIGH);

        pinMode(ls_pins[number], OUTPUT);
        digitalWrite(ls_pins[number], HIGH);
        delayMicroseconds(10);

        pinMode(ls_pins[number], INPUT);
        unsigned long start_time = micros();

        unsigned long timeout = 3000; // Example timeout period
        while (digitalRead(ls_pins[number]) == HIGH) {
            if (micros() - start_time > timeout) {
                return -2; // Timeout occurred
            }
        }

        unsigned long end_time = micros();
        return (float)(end_time - start_time);
    }
};

#endif
