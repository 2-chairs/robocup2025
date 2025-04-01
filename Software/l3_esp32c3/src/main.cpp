// // #include <utility>
// // #include "main.h"

// // using namespace std;
// // #define ll long long

// // //Functional Functions
// // void setupTOFs() {

// // }

// // //Debugging Functions
// // void printTOFs() {
// //     for (int i = 0; i < 4; i++) {
// //         Serial.println("TOF ");
// //         Serial.print(i);
// //         Serial.print(": ");
// //         Serial.print(tof1.readRangeContinuousMillimeters()); Serial.print(" mm\t");
// //     }
// // }

// // //Actual code
// // void setup() {
// //     Serial.begin(115200);
// //     Wire.begin();
// //     setupTOFs();
// // }

// // void loop() {
// //     // Serial.println("print sth");
// //     printTOFs();
// // }

// #include <Arduino.h>
// #include <Wire.h>
// #include <VL53L0X.h>

// #define XSHUT1 D0

// VL53L0X sensor;

// void setup() {
//     pinMode(XSHUT1, OUTPUT);
//     digitalWrite(XSHUT1, HIGH);
//     Serial.begin(115200);
//     Wire.begin(6, 7); // SDA, SCL for XIAO ESP32-C3
//     Wire.setClock(400000); // 400 kHz I2C

//     sensor.setTimeout(500);
  
//     if (!sensor.init()) {
//         Serial.println("Failed to detect and initialize sensor!");
//         while (!sensor.init()) {
//             Serial.println("Failed to detect and initialize sensor!");
//         }
//     }

//     sensor.startContinuous();
// }

// void loop() {
//   Serial.print("Distance: ");
//   Serial.print(sensor.readRangeContinuousMillimeters());
//   if (sensor.timeoutOccurred()) {
//     Serial.print(" TIMEOUT");
//   }
//   Serial.println();
//   delay(100);
// }

#include <Arduino.h>
#include <Wire.h>
#include <VL53L0X.h>

VL53L0X sensor;

void setup() {
    pinMode(D0, OUTPUT);
    digitalWrite(D0, LOW);
    delay(10);  // Ensure shutdown
    digitalWrite(D0, HIGH);
    delay(10);  // Allow it to wake up
    Serial.begin(115200);
    Wire.begin(6,7); // ESP32 default: SDA = 21, SCL = 22

    if (!sensor.init()) {
        Serial.println("Failed to initialize VL53L0X! Check connections.");
        while (1) {
          Serial.println("cannot initalise");
        }; // Stop execution
    }

    sensor.setTimeout(500);
    sensor.startContinuous();
    Serial.println("VL53L0X Initialized!");
}

void loop() {
    Serial.print("Distance: ");
    Serial.print(sensor.readRangeContinuousMillimeters());
    if (sensor.timeoutOccurred()) {
        Serial.print(" TIMEOUT");
    }
    Serial.println(" mm");

    delay(500);
}