#include "main.h"

#include <PacketSerial.h>
#include <algorithm>
#include <array>
#include <vector>
#include <array>
#include <deque>

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
    // Serial.println(ldr_value); 
    return ldr_value; 
}

int readMux2Channel(int channel) {
    selectMuxChannel(channel);
    int ldr_value = analogRead(M2);
    // Serial.println(ldr_value); 
    return ldr_value; 
}

// Clips an angle to the range (-180, 180]
double clipAngleTo180(double angle) {
    angle = fmod(angle, 360);
    return angle > 180 ? angle - 360 : (angle < -180 ? angle + 360 : angle);
}

// Clips an angle to the range [0, 360)
double clipAngleTo360(double angle) {
    angle = fmod(angle, 360);
    return angle < 0 ? angle + 360 : angle;
}

// Finds the difference between two angles, in the range [0, 360)
double angleDifference(double leftAngle, double rightAngle) {
    return clipAngleTo360(rightAngle - leftAngle);
}

// Finds the smaller difference between two angles, in the range [0, 180]
double smallerAngleDifference(double leftAngle, double rightAngle) {
    const auto angle = angleDifference(leftAngle, rightAngle);
    return fmin(angle, 360 - angle);
}

// Finds the angle bisector between two angles, in the range [0, 360)
double angleBisector(double leftAngle, double rightAngle) {
    return clipAngleTo360(leftAngle + smallerAngleDifference(leftAngle, rightAngle) / 2);
}

void checkLightRing() {
    int ldr = 0;

    //cycle thru mux 1 to check if ldr value above treshhold (treshhold set in light_ring.h)
    //if above treshhold, set ldr value in ldr_threshold_pass: set as true. else set as false
    for (int mux1 = 0; mux1 < 16; mux1++) {
        int ldrVal = readMux1Channel(mux1);
        ldr_values[ldr] = ldrVal;
        if (ldrVal >= ldr_threshold) {
            ldr_threshold_pass[ldr] = true;
        }
        else {
            ldr_threshold_pass[ldr] = false;
        }
        ldr++;
    }

    //cycle thru mux 2
    for (int mux2 = 0; mux2 < 16; mux2++) {
        int ldrVal = readMux2Channel(mux2);
        ldr_values[ldr] = ldrVal;
        if (ldrVal >= ldr_threshold) {
            ldr_threshold_pass[ldr] = true;
        }
        else {
            ldr_threshold_pass[ldr] = false;
        }
        ldr++;
    }

    int y = 0;
    for (int x = 0; x < 16; x+=2) {
        filtered_ldr_threshold_pass[x] = ldr_threshold_pass[y];
        y+=2;
    }


}

pair<double, double> findLine() {
    //from senrobo github code-int23
    //adding ldrs detecting the line to vector matches
    vector<uint8_t> matches;
    matches.reserve(16);
    for (int i = 0; i < 16; i++) {
        if (filtered_ldr_threshold_pass[i]) matches.push_back(i);
    }

    //less than 2 matches detected, not on the line
    if (matches.size() <= 1) return {NAN, NAN};

    //find scope of the line

    double lineStartAngle = NAN, lineEndAngle = NAN;
    
    //iterate thru all combinations of matching indices (cool work here) and find the pair furthest apart
    
    double maxAngleDifference = 0;
    for (uint8_t i = 0; i < matches.size()-1; i++) {
        for (uint8_t j = i+1; j < matches.size(); j++) {
            //getting bearings of ldrs
            const auto angleI = matches[i]*22.5;
            const auto angleJ = matches[j]*22.5;
            //current angle difference between ldr i and j
            const auto angleDifference = smallerAngleDifference(angleI, angleJ);
            if (angleDifference > maxAngleDifference) {
                maxAngleDifference = angleDifference;
                lineStartAngle = angleI;
                lineEndAngle = angleJ;
            }
        }
    }

    //This should not happen, but just in case (quoted)
    if (isnan(lineStartAngle) || isnan(lineEndAngle)) return {NAN, NAN};

    //Calculate the line angle bisector and size
    if (lineEndAngle - lineStartAngle > 180) swap(lineStartAngle, lineEndAngle);
    
    const auto lineAngleBisector = angleBisector(lineStartAngle, lineEndAngle);
    //Let line size be the ratio of the cluster size to 180°, so it's in [0, 1]
    const auto lineSize = maxAngleDifference / 180.0;

    return {lineAngleBisector, lineSize};
}

void communicateSerial(double angle, double size) {
    Serial_L1.print(angle);
    Serial_L1.print(" ");
    Serial_L1.println(size);
}

//Debugging Functions
void readLDR1() {
    digitalWrite(S0, HIGH);
    digitalWrite(S1, LOW);
    digitalWrite(S2, LOW);
    digitalWrite(S3, LOW);
    Serial.println(analogRead(M1));
}

void readLDRs() {
    checkLightRing();
    Serial.println();
    Serial.print("LDRs: ");
    for (int i = 0; i < 16; i++) {

        // if (ldr_threshold_pass[i]) {
        //     Serial.print(1);
        // }
        // else {
        //     Serial.print(0);
        // }
        Serial.print(ldr_values[i*2]);
        Serial.print(" | ");
    }
}

//Actual Code
void setup() {
    Serial.begin(115200);
    Serial_L1.begin(115200);
    analogReadResolution(12);
    setupMux();
}

void loop() {
    checkLightRing();
    auto [angle, size] = findLine();
    if (isnan(angle) || isnan(size)) {
        //not on line, continue doing whatever, no complains
        ;
    }
    else {
        //code on what to do if on line
        //just complain to teensy
        communicateSerial(angle, size);
    }
    // readLDRs();
    // Serial.println(readMux1Channel(1));

}