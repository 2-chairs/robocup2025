#include "main.h"

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

    for (int i = 0; i < 16; i++) {
        filtered_ldr_threshold_pass[i] = ldr_threshold_pass[i*2];
        filtered_ldr_values[i] = ldr_values[i*2];
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
    for (int i = 0; i < 32; i++) {

        // if (ldr_threshold_pass[i]) {
        //     Serial.print(1);
        // }
        // else {
        //     Serial.print(0);
        // }
        // Serial.print(filtered_ldr_values[i]);
        Serial.print(ldr_values[i]);
        Serial.print(" | ");
    }
}

void setupSolenoid() {
    pinMode(Solenoid, OUTPUT);
    digitalWrite(Solenoid, LOW);
}

void kickSolenoid() {
    digitalWrite(Solenoid, HIGH);
    delay(500);
    digitalWrite(Solenoid, LOW);
}

// Run the calibration routine
void printLightRingCalibration() {
    // min = green (field), max = white (line)
    uint16_t min[16], max[16];
    for (int i = 0; i < 16; ++i) {
        min[i] = 0xFFFF;
        max[i] = 0x0000;
    }

    const auto endTime = millis() + 1000;
    while (millis() < endTime) {
        for (uint8_t i = 0; i < 16; ++i) {
            const auto value = filtered_ldr_values[i];
            if (value < min[i]) min[i] = value;
            if (value > max[i]) max[i] = value;
        }
    }

    // Print the thresholds (averages of min and max)
    Serial.printf("Thresholds: {");
    for (uint8_t i = 0; i < 16; ++i) {
        // TODO: Find a more accurate way to calculate the threshold
        const auto threshold = (max[i] + min[i]) / 2;
        Serial.printf("%d, ", (uint16_t)threshold);
    }
    Serial.printf("}\n");
}

//debug threshold
void printLDRThreshold() {
    checkLightRing();
    for (int i = 0; i < 16; i++) {
        Serial.print(filtered_ldr_threshold_pass[i]);
        Serial.print(" | ");
    }
    Serial.println();
}

//Actual Code
void setup() {
    Serial.begin(115200);
    Serial_L1.begin(9600, SERIAL_8N1, -1, -1);
    analogReadResolution(12);
    setupMux();
    setupSolenoid();
}

void loop() {
    // digitalWrite(Solenoid, LOW);
    // if (Serial_L1.available()) {
    //     String received = Serial_L1.readStringUntil('\n');
    //     Serial.print("Received from Teensy: ");
    //     Serial.println(received);
    // }
    // Serial_L1.print("Hello from ESP32C3!\n");
    // Serial.print("test");
    // delay(10);
    // checkLightRing();
    auto [angle, size] = findLine();
    // for testing communication, fabricated numbers
    // double angle = 22.5;
    // double size = 1.0;
    if (isnan(angle) || isnan(size)) {
        //not on line, continue doing whatever, no complains
        Serial.println("NOT ON LINE");
    }
    else {
        //code on what to do if on line

        //flip line angle
        angle = clipAngleTo360(angle + 180);

        //complain to teensy
        communicateSerial(angle, size);
        Serial.println("ON LINE");
    }
    // printLDRThreshold();
    // checkLightRing();
    // printLightRingCalibration();
    // readLDRs();
    // Serial.println(readMux1Channel(1));

}