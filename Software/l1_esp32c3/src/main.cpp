#include <utility>
#include "main.h"

using namespace std;
#define ll long long

//Functional Functions
void setupMux() {
    pinMode(M1, INPUT);
    pinMode(M2, INPUT);
    pinMode(S0, OUTPUT);
    pinMode(S1, OUTPUT);
    pinMode(S2, OUTPUT);
    pinMode(S3, OUTPUT);
    Serial.println("MUX Setup Completed... (by right)");
}

void selectMuxChannel(int channel) {
    digitalWrite(S0, channel & 0x01);
    digitalWrite(S1, (channel >> 1) & 0x01);
    digitalWrite(S2, (channel >> 2) & 0x01);
    digitalWrite(S3, (channel >> 3) & 0x01);
}

int readMux1Channel(int channel) {
    selectMuxChannel(channel);
    int ldr_value = analogRead(M1);
    Serial.println(ldr_value); 
    return ldr_value; 
}

int readMux2Channel(int channel) {
    selectMuxChannel(channel);
    int ldr_value = analogRead(M2);
    Serial.println(ldr_value); 
    return ldr_value; 
}

void checkLightRing() {
    int ldr = 0;

    //cycle thru mux 1 to check if ldr value above treshhold (treshhold set in light_ring.h)
    //if above treshhold, set ldr value in ldr_threshold_pass: set as true. else set as false
    for (int mux1 = 1; mux1 < 16+1; mux1++) {
        int ldrVal = readMux1Channel(mux1);
        if (ldrVal >= ldr_treshhold) {
            ldr_treshhold_pass[ldr] = true;
        }
        else {
            ldr_treshhold_pass[ldr] = false;
        }
        ldr++;
    }

    //cycle thru mux 2
    for (int mux2 = 1; mux2 < 16+1; mux2++) {
        int ldrVal = readMux2Channel(mux2);
        if (ldrVal >= ldr_treshhold) {
            ldr_treshhold_pass[ldr] = true;
        }
        else {
            ldr_treshhold_pass[ldr] = false;
        }
        ldr++;
    }
}

pair<int, int> getGreatestNonReflexAngle() {
    int ldr1Final = -1, ldr2Final = -1; //begin by intitating to -1. if at the end of function still -1 for both: no line detected
    int greatest_angle = -1; //begin by intitating to -1. variable represents the greatest non reflex angle found so far -> used to compare the led detecting line combinations to find largest non reflex angle

    //use nested loops to cycle thru all possibities of leds detecting line (Big-O: N^2, prob can optimise)
    for (int ldr1 = 0; ldr1 < 32; ldr1++) { 
        if (ldr_treshhold_pass[ldr1]) {
            for (int ldr2 = ldr1+1; ldr2 < 32; ldr2++) {
                if (ldr_treshhold_pass[ldr2]) {
                    int angle = abs(ldr1*11.25 - ldr2*11.25); //calculate angle between the 2 leds found
                    if (angle > 180) angle = 360-angle; // if its a reflex angle make it non reflex
                    
                    if (angle > greatest_angle) {
                        greatest_angle = angle;
                        ldr1Final = ldr1;
                        ldr2Final = ldr2;
                    }
                }
            }
        }
    }

    return {ldr1Final, ldr2Final};
}

void findLine() {

}

//Debugging Functions
//...

//Actual Code
void setup() {
    Serial.begin(115200);
    // setupMux();
    // digitalWrite(S0, LOW);
    // digitalWrite(S1, LOW);
    // digitalWrite(S2, LOW);
    // digitalWrite(S3, LOW);
}

void loop() {
    // Serial.println(analogRead(M1));
    Serial.println("PRINT SOMETHING");
    // Serial.println("should have sometihng here...");
    // for (int channel = 0; channel < 17; channel++) {
    //     readMux2Channels(channel);
    // }
}