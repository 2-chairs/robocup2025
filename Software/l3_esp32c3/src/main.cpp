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
#include "PacketSerial.h"

// Serial for Packet Communication
HardwareSerial MySerial0(0);
PacketSerial lidarSerial;

// Define XSHUT pins for sensors (front, right, back, left)
const int xshutPins[4] = {16, 17, 18, 19};
const byte sensorAddresses[4] = {0x30, 0x31, 0x32, 0x33};

VL53L0X sensors[4];
uint16_t distances[4];

struct LidarData {
    uint16_t distance[4];
};

LidarData esp32LidarData;

typedef struct lidarTxPayload {
    LidarData esp32LidarData;
} lidarTxPayload;

void setup() {
    Serial.begin(115200);
    MySerial0.begin(115200, SERIAL_8N1, -1, -1);
    lidarSerial.begin(&MySerial0);

    Wire.begin(21, 22);

    // Initialize XSHUT pins (disable all sensors initially)
    for (int i = 0; i < 4; i++) {
        pinMode(xshutPins[i], OUTPUT);
        digitalWrite(xshutPins[i], LOW);
    }

    delay(10);

    // Wake and initialize each sensor one by one
    for (int i = 0; i < 4; i++) {
        digitalWrite(xshutPins[i], HIGH);
        delay(10);

        if (!sensors[i].init()) {
            Serial.print("Sensor init failed at position: ");
            Serial.println(i);
            while (1) delay(1000);
        }

        sensors[i].setTimeout(500);
        sensors[i].setAddress(sensorAddresses[i]);
        sensors[i].startContinuous();
        Serial.print("Sensor initialized at address: 0x");
        Serial.println(sensorAddresses[i], HEX);
    }
}

void loop() {
    for (int i = 0; i < 4; i++) {
        uint16_t distance = sensors[i].readRangeContinuousMillimeters();

        if (!sensors[i].timeoutOccurred()) {
            esp32LidarData.distance[i] = distance;
        } else {
            esp32LidarData.distance[i] = 0;  // Error or timeout
        }
    }

    // Prepare and send data using PacketSerial
    byte buf[sizeof(lidarTxPayload)];
    memcpy(buf, &esp32LidarData, sizeof(esp32LidarData));
    lidarSerial.send(buf, sizeof(buf));

    delay(20);
}