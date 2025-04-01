#ifndef LIGHT_RING_H
#define LIGHT_RING_H

#include <Arduino.h>
#include <utility>
#include <cmath>
#include <stdint.h>

using namespace std;
#define ll long long

// Pin Definitions
#define M1 A1 //Analog input for MUX1
#define M2 A0 //Analog input for MUX2

//Select line Pin Definitions
#define S0 D2
#define S1 D3
#define S2 D10
#define S3 D9

//Teensy Serial
#define TeensySerial Serial1

//Functional Function Prototypes
void onTeensyPacket(const byte *buf, size_t size);
void setupMux();
void selectMuxChannel(int channel);
int readMux1Channel(int channel);
int readMux2Channel(int channel);
double smallerAngleDifference(double leftAngle, double rightAngle);
void checkLightRing();
pair<double, double> findLine();

//Debugging Function Prototypes
void readLDR1();

int ldr_threshold = 10;
bool ldr_threshold_pass[32] = {false};
int ldr_values[32] = {0};

#endif  // LIGHT_RING_H