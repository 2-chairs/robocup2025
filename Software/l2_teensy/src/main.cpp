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
    if (Serial_L1.available()) {
        String received = Serial_L1.readStringUntil('\n');
        Serial.print("Received from ESP32C3: ");
        Serial.println(received);
        
        // Optional: Trim any trailing \r or whitespace
        received.trim();
        // Parse the line
        int spaceIndex = received.indexOf(' ');
        if (spaceIndex != -1) {
            String angleStr = received.substring(0, spaceIndex);
            String sizeStr = received.substring(spaceIndex + 1);

            double angle = angleStr.toFloat();
            double size = sizeStr.toFloat();

            return {angle, size};
            // Serial.print("Angle: ");
            // Serial.println(angle);
            // Serial.print("Size: ");
            // Serial.println(size);
        }
        else {
            // Serial.println("Malformed line: missing space");
            return {NAN, NAN};
        }
    }
}

//Debugging Functions
bool debugging = true;

void test_MOTORs() {
    if (debugging) {
        digitalWrite(MOTOR_1_IN_A, LOW);
        digitalWrite(MOTOR_1_IN_B, HIGH);
        analogWrite(MOTOR_1_PWM, 255);
        digitalWrite(MOTOR_2_IN_A, LOW);
        digitalWrite(MOTOR_2_IN_B, HIGH);
        analogWrite(MOTOR_2_PWM, 255);
        digitalWrite(MOTOR_3_IN_A, LOW);
        digitalWrite(MOTOR_3_IN_B, HIGH);
        analogWrite(MOTOR_3_PWM, 255);
        digitalWrite(MOTOR_4_IN_A, LOW);
        digitalWrite(MOTOR_4_IN_B, HIGH);
        analogWrite(MOTOR_4_PWM, 255);
        Serial.println("Motors should spin...");
    }
}

//Actual code
void setup() {
    Serial.begin(115200);
    Serial_L1.begin(9600);
    Serial_L3.begin(9600);
    Serial_IMU.begin(115200);
    // if (debugging) {
    //     Serial.println("Debugging");
    // }
    // else {
    //     Serial.println("Not Debugging");
    // }
    setupMotors();
    test_MOTORs();
}

void loop() {
    test_MOTORs();
    // auto [angle, size] = getSerial_L1();
    // Serial.print(angle);
    // Serial.print(" | ");
    // Serial.println(size);
    // Serial.println("PRINTSTH");
    // Serial_L1.write("Hello from Teensy!\n");
    // Serial.println("test");
}