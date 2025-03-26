#include <utility>
#include "main.h"

using namespace std;
#define ll long long

//Functional Functions
void setupTOFs() {
    //define tofs xshut pinMode
    //turn off all xshuts for tofs
    for (int i = 0; i < 4; i++) {
        pinMode(xshutPins[i], OUTPUT);
        digitalWrite(xshutPins[i], LOW);
    }

    //turn on tofs one by one
    for (int i = 0; i < 4; i++) {
        digitalWrite(xshutPins[i], HIGH);
        delay(10);

        //switch is just like if-else but apparently faster (chatgpt) -> more clean and readable
        //goes thru all tofs and initialises it
        switch (i) {
            case 0:
                tof1.init();
                tof1.setAddress(addresses[i]);
                break;
            case 1:
                tof2.init();
                tof2.setAddress(addresses[i]);
                break;
            case 2:
                tof3.init();
                tof3.setAddress(addresses[i]);
                break;
            case 3:
                tof4.init();
                tof4.setAddress(addresses[i]);
                break;
        }
    }

    //start tofs continuous measurement mode
    tof1.startContinuous();
    tof2.startContinuous();
    tof3.startContinuous();
    tof4.startContinuous();
}

//Debugging Functions
void printTOFs() {
    for (int i = 0; i < 4; i++) {
        Serial.println("TOF ");
        Serial.print(i);
        Serial.print(": ");
        Serial.print(tof1.readRangeContinuousMillimeters()); Serial.print(" mm\t");
    }
}

//Actual code
void setup() {
    Serial.begin(115200);
    Wire.begin();
    setupTOFs();
}

void loop() {
    Serial.println("print sth");
}