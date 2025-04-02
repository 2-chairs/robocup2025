#include <utility>
#include "main.h"

using namespace std;
#define ll long long

//Functional Functions
void setupMotors() {
    //MOTOR 1
    pinMode(MOTOR_1_IN_A, OUTPUT);
    pinMode(MOTOR_1_IN_B, OUTPUT);
    pinMode(MOTOR_1_PWM, OUTPUT);

    //MOTOR 2
    pinMode(MOTOR_2_IN_A, OUTPUT);
    pinMode(MOTOR_2_IN_B, OUTPUT);
    pinMode(MOTOR_2_PWM, OUTPUT);

    //MOTOR 3
    pinMode(MOTOR_3_IN_A, OUTPUT);
    pinMode(MOTOR_3_IN_B, OUTPUT);
    pinMode(MOTOR_3_PWM, OUTPUT);

    //MOTOR 4
    pinMode(MOTOR_4_IN_A, OUTPUT);
    pinMode(MOTOR_4_IN_B, OUTPUT);
    pinMode(MOTOR_4_PWM, OUTPUT);

    Serial.println("Motors pinModes defined (by right...)");
}

pair<double, double> getSerial_L1() {
    String serial_L1_buffer = "";
    while (Serial_L1.available()) {
        char c = Serial_L1.read();
        
        if (c == '\r') continue; //to skip carriage return (chatgpt says must add, prob becuz Serial.println() end with "\r\n")
        
        if (c == 'n') {

        }

        serial_L1_buffer += c;

        //should not happen, no clue how this would even happen
        //cap buffer size to prevent overflow
        if (serial_L1_buffer.length() > 32) {
            Serial.println("HELP L1 SERIAL BUFFER EXCEEDED AHHH");
            serial_L1_buffer = "";
            return {NAN, NAN};
        }
    }
}

//Debugging Functions
bool debugging = true;

void test_MOTOR_1() {
    if (debugging) {
        digitalWrite(MOTOR_1_IN_A, LOW);
        digitalWrite(MOTOR_1_IN_B, HIGH);
        analogWrite(MOTOR_1_PWM, 255);
        Serial.println("Motor 1 should spin...");
    }
}

//Actual code
void setup() {
    Serial.begin(115200);
    Serial_L1.begin(115200);
    Serial_L3.begin(115200);
    Serial_IMU.begin(115200);
    if (debugging) {
        Serial.println("Debugging");
    }
    else {
        Serial.println("Not Debugging");
    }
    setupMotors();
    test_MOTOR_1();
}

void loop() {
    test_MOTOR_1();
}