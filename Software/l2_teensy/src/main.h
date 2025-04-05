#ifndef TEENSY_H
#define TEENSY_H

#include <Arduino.h>

//Goal
#define oppsGoal "blue"
// #define oppsGoal "yellow"

//MOTOR 1
#define MOTOR_1_IN_A 12
#define MOTOR_1_IN_B 11
#define MOTOR_1_PWM 3

//MOTOR 2
#define MOTOR_2_IN_A 10
#define MOTOR_2_IN_B 9
#define MOTOR_2_PWM 2

//MOTOR 3
#define MOTOR_3_IN_A 22
#define MOTOR_3_IN_B 23
#define MOTOR_3_PWM 1

//MOTOR 4
#define MOTOR_4_IN_A 6
#define MOTOR_4_IN_B 5
#define MOTOR_4_PWM 0

//Serials
#define Serial_L1 Serial2 // Serial to L1 Esp32c3
#define Serial_L3 Serial5 // Serial to L3 Esp32c3
#define Serial_CAM Serial3 // Serial to CAM
#define Serial_IMU Serial4 // Serial to IMU

//Cam

//ball
double ballX = NAN;
double ballY = NAN;
double ballAngle = NAN;
double ballDistance = NAN;
//last known positions
double lastballX = NAN;
double lastballY = NAN;
double lastballAngle = 340;
double lastballDistance = NAN;

//blue goal
double blueGoalX = NAN;
double blueGoalY = NAN;
double blueGoalAngle = NAN;
double blueGoalDistance = NAN;
//last known positions
double lastblueGoalX = NAN;
double lastblueGoalY = NAN;
double lastblueGoalAngle = NAN;
double lastblueGoalDistance = NAN;

//yellow goal
double yellowGoalX = NAN;
double yellowGoalY = NAN;
double yellowGoalAngle = 0;
double yellowGoalDistance = NAN;
//last known positions
double lastyellowGoalX = NAN;
double lastyellowGoalY = NAN;
double lastyellowGoalAngle = 180;
double lastyellowGoalDistance = NAN;

//Functional Function Prototypes
void setupMotors();
void test_MOTOR_1();

bool firstloop = true;

#endif  // TEENSY_H