<<<<<<< Updated upstream
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
=======
#include <Wire.h>
#include <VL53L0X.h>

VL53L0X sensor;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  
  // Initialize the sensor
  if (!sensor.init()) {
    Serial.println("Failed to detect and initialize sensor!");
    while (1);
  }
  
  // Configure the sensor for better accuracy
  sensor.setMeasurementTimingBudget(200000); // 200 ms timing budget
  
  Serial.println("VL53L0X ToF sensor initialized");
}

void loop() {
  // Get distance measurement
  int distance = sensor.readRangeSingleMillimeters();
  
  // Check for timeout
  if (sensor.timeoutOccurred()) {
    Serial.println("Sensor timeout!");
  } else {
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" mm");
  }
  
  delay(500); // Wait half a second between measurements
}



// #include <Arduino.h>
// #include <Wire.h>
// #include "VL53L0X.h"

// // Define GPIO pins for XSHUT control
// #define XSHUT1 0
// #define XSHUT2 1
// #define XSHUT3 2
// #define XSHUT4 3

// // Create VL53L0X objects
// VL53L0X sensor1;
// VL53L0X sensor2;
// VL53L0X sensor3;

// // Define I2C addresses for each sensor
// #define SENSOR1_ADDRESS 0x30
// #define SENSOR2_ADDRESS 0x31
// #define SENSOR3_ADDRESS 0x32
// void setup() {
//   Serial.begin(115200);
//   Wire.begin();
//   Wire.setClock(400000);  // Set I2C clock to 400kHz for faster communication

//   // Set XSHUT pins as outputs and turn all sensors off
//   pinMode(XSHUT1, OUTPUT);
//   pinMode(XSHUT2, OUTPUT);
//   pinMode(XSHUT3, OUTPUT);
//   digitalWrite(XSHUT1, LOW);
//   digitalWrite(XSHUT2, LOW);
//   digitalWrite(XSHUT3, LOW);
//   delay(10);

//   // Initialize each sensor one at a time
//   // Sensor 1
//   digitalWrite(XSHUT1, HIGH);
//   delay(10);
//   if (!sensor1.init()) {
//     Serial.println("Failed to initialize Sensor 1");
//     while (true) {
//         Serial.println("poopy");
//     }
//     return;
//   }
//   sensor1.setAddress(SENSOR1_ADDRESS);
//   sensor1.setTimeout(500);
//   sensor1.startContinuous();

//   // Sensor 2
//   digitalWrite(XSHUT2, HIGH);
//   delay(10);
//   if (!sensor2.init()) {
//     Serial.println("Failed to initialize Sensor 2");
//     return;
//   }
//   sensor2.setAddress(SENSOR2_ADDRESS);
//   sensor2.setTimeout(500);
//   sensor2.startContinuous();

//   // Sensor 3
//   digitalWrite(XSHUT3, HIGH);
//   delay(10);
//   if (!sensor3.init()) {
//     Serial.println("Failed to initialize Sensor 3");
//     return;
//   }
//   sensor3.setAddress(SENSOR3_ADDRESS);
//   sensor3.setTimeout(500);
//   sensor3.startContinuous();

//   Serial.println("Sensors initialized successfully.");
// }

// void loop() {
//   // Read distances from each sensor
//   uint16_t distance1 = sensor1.readRangeSingleMillimeters();
//   uint16_t distance2 = sensor2.readRangeSingleMillimeters();
//   uint16_t distance3 = sensor3.readRangeSingleMillimeters();

//   // Check for any measurement errors
//   if (sensor1.timeoutOccurred()) Serial.println("Sensor 1 timeout");
//   if (sensor2.timeoutOccurred()) Serial.println("Sensor 2 timeout");
//   if (sensor3.timeoutOccurred()) Serial.println("Sensor 3 timeout");

//   // Print results
//   Serial.print("S1: ");
//   Serial.print(distance1);
//   Serial.print("mm  S2: ");
//   Serial.print(distance2);
//   Serial.print("mm  S3: ");
//   Serial.print(distance3);
//   Serial.println("mm");

//   delay(100);
// }
>>>>>>> Stashed changes
