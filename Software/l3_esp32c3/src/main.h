#include <Arduino.h>
#include <Wire.h>
#include <VL53L0X.h>

const int xshutPins[] = {0, 1, 2, 3};

const uint8_t addresses[] = {0x30, 0x31, 0x32, 0x33};

VL53L0X tof1;
VL53L0X tof2;
VL53L0X tof3;
VL53L0X tof4;

void setupTOFs();
void printTOFs();