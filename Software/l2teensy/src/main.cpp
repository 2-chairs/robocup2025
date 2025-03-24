#include <utility>
#include "main.h"

using namespace std;
#define ll long long

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
}

void setup() {
    setupMotors();
    digitalWrite(MOTOR_1_IN_A, LOW);
    digitalWrite(MOTOR_1_IN_B, HIGH);
    analogWrite(MOTOR_1_PWM, 255);
}

void loop() {

}