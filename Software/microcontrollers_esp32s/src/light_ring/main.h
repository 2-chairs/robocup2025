#ifndef LIGHT_RING_H
#define LIGHT_RING_H

#include <Arduino.h>

// Pin Definitions
#define M1 A1// Analog input for MUX1
#define M2 A0  // Analog input for MUX2
#define S0 2
#define S1 3
#define S2 10
#define S3 9

// Function Prototypes
void setupMux();
void selectMuxChannel(int channel);
int readMux1Channel(int channel);
int readMux2Channel(int channel);

int ldr_treshhold = 10;
int ldr_treshhold_pass[32];

#endif  // LIGHT_RING_H