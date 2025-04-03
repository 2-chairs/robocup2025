#ifndef LIGHT_RING_H
#define LIGHT_RING_H

#include <Arduino.h>
#include <HardwareSerial.h>
#include <utility>
#include <cmath>
#include <stdint.h>

using namespace std;
#define ll long long

// L1 Xiao Esp32C3 Serial Communication to L2 Teensy4.0
HardwareSerial Serial_L1(0);

// Serial_L1
#define RX 6
#define TX 7

// Pin Definitions
#define M1 A1 //Analog input for MUX1
#define M2 A0 //Analog input for MUX2

// Select line Pin Definitions
#define S0 D2
#define S1 D3
#define S2 D10
#define S3 D9

// Solenoid Pin Declaration
#define Solenoid D8

//Functional Function Prototypes
void onTeensyPacket(const byte *buf, size_t size);
void setupMux();
void selectMuxChannel(int channel);
int readMux1Channel(int channel);
int readMux2Channel(int channel);
double smallerAngleDifference(double leftAngle, double rightAngle);
void checkLightRing();
pair<double, double> findLine();
void communicateSerial(double angle, double size);
void setupSolenoid();

//Debugging Function Prototypes
void readLDR1();

int ldr_threshold = 2000;
bool ldr_threshold_pass[32] = {false};
bool filtered_ldr_threshold_pass[16] = {false};
int ldr_values[32] = {0};
int filtered_ldr_values[16] = {0};

#endif  // LIGHT_RING_H