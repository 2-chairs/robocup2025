// #include <Arduino.h>
// #include <Wire.h>

// // VL53L0X ToF sensor library - you might need to install this via PlatformIO
// // If you have a different ToF sensor, replace with the appropriate library
// #include <VL53L0X.h>

// VL53L0X tofSensor;
// bool sensorInitialized = false;

// void setup() {
//   // Initialize serial communication
//   Serial.begin(115200);
//   delay(1000); // Give time for the serial connection to establish
  
//   Serial.println("ToF Sensor Example Starting...");
  
//   // Initialize I2C communication (required for most ToF sensors)
//   Wire.begin();
  
//   // Try to initialize the ToF sensor
//   Serial.println("Initializing ToF sensor...");
  
//   if (!tofSensor.init()) {
//     Serial.println("Failed to initialize VL53L0X sensor!");
//     sensorInitialized = false;
//   } else {
//     Serial.println("VL53L0X sensor initialized successfully!");
    
//     // Configure sensor settings
//     tofSensor.setMeasurementTimingBudget(50000); // 50ms per measurement (faster)
    
//     // Start continuous measurements
//     tofSensor.startContinuous();
    
//     sensorInitialized = true;
//   }
// }

// void loop() {
//   if (sensorInitialized) {
//     // Read the distance from the sensor
//     uint16_t distance = tofSensor.readRangeContinuousMillimeters();
    
//     // Check if there was an error
//     if (tofSensor.timeoutOccurred()) {
//       Serial.println("ToF sensor timeout!");
//     } else {
//       // Print the distance to Serial Monitor
//       Serial.print("Distance: ");
//       Serial.print(distance);
//       Serial.println(" mm");
//     }
//   } else {
//     // If initialization failed, try again
//     Serial.println("Sensor not initialized. Retrying...");
//     if (!tofSensor.init()) {
//       Serial.println("Failed to initialize sensor again.");
//     } else {
//       tofSensor.setMeasurementTimingBudget(50000);
//       tofSensor.startContinuous();
//       sensorInitialized = true;
//       Serial.println("Sensor initialized successfully!");
//     }
//   }
  
//   // Wait before next reading
//   delay(500);
// }
#include <Arduino.h>
#include <Wire.h>
#include <VL53L0X.h>

// Create three sensor objects
VL53L0X sensor1;
VL53L0X sensor2;
VL53L0X sensor3;

// Define the shutdown pins (XSHUT) for each sensor
#define XSHUT_PIN1 1 // GPIO2
#define XSHUT_PIN2 3  // GPIO3
#define XSHUT_PIN3 4  // GPIO4

// Define I2C addresses - each sensor needs a unique address
#define TOF_ADDRESS1 0x30
#define TOF_ADDRESS2 0x33
#define TOF_ADDRESS3 0x32

// Status flags for each sensor
bool sensor1Ready = false;
bool sensor2Ready = false;
bool sensor3Ready = false;

// Function to set up a single sensor
bool setupSensor(VL53L0X &sensor, int xshutPin, uint8_t address, const char* sensorName) {
    // Power on the sensor
    digitalWrite(xshutPin, HIGH);
    delay(50); // Give more time for power-up
    
    // Try to initialize multiple times
    bool initialized = false;
    for (int attempt = 0; attempt < 5; attempt++) {
        if (sensor.init(true)) {
            initialized = true;
            break;
        }
        Serial.print(".");
        delay(100);
    }
    
    if (!initialized) {
        Serial.print(sensorName);
        Serial.println(" initialization failed!");
        return false;
    }
    
    // Set address (No return value, so assume success)
    sensor.setAddress(address);
    
    Serial.print(sensorName);
    Serial.print(" address set to 0x");
    Serial.println(address, HEX);
    
    // Configure for better reliability
    sensor.setMeasurementTimingBudget(100000); // Longer timing budget = more accurate but slower
    
    // Start continuous mode with a high timeout
    sensor.startContinuous(100);
    
    Serial.print(sensorName);
    Serial.println(" initialized successfully.");
    
    return true;
}

void setup() {
    // Initialize serial communication
    Serial.begin(115200);
    delay(1000); // Give time for the serial connection to establish
    
    Serial.println("Three ToF Sensors Example Starting...");
    
    // Initialize I2C with lower clock speed for better reliability
    Wire.begin();
    Wire.setClock(100000); // Use 100 kHz instead of the default 400 kHz
    
    // Set shutdown pins as outputs
    pinMode(XSHUT_PIN1, OUTPUT);
    pinMode(XSHUT_PIN2, OUTPUT);
    pinMode(XSHUT_PIN3, OUTPUT);
    
    // Power off all sensors
    digitalWrite(XSHUT_PIN1, LOW);
    digitalWrite(XSHUT_PIN2, LOW);
    digitalWrite(XSHUT_PIN3, LOW);
    delay(100); // Give more time for reset
    
    // Initialize sensors one by one
    Serial.print("Initializing sensor 1...");
    sensor1Ready = setupSensor(sensor1, XSHUT_PIN1, TOF_ADDRESS1, "Sensor 1");
    
    Serial.print("Initializing sensor 2...");
    sensor2Ready = setupSensor(sensor2, XSHUT_PIN2, TOF_ADDRESS2, "Sensor 2");
    
    Serial.print("Initializing sensor 3...");
    sensor3Ready = setupSensor(sensor3, XSHUT_PIN3, TOF_ADDRESS3, "Sensor 3");
    
    Serial.println();
    Serial.print("Sensors ready: 1=");
    Serial.print(sensor1Ready ? "YES" : "NO");
    Serial.print(", 2=");
    Serial.print(sensor2Ready ? "YES" : "NO");
    Serial.print(", 3=");
    Serial.println(sensor3Ready ? "YES" : "NO");
}

// Function to read a sensor with error handling
uint16_t readSensor(VL53L0X &sensor, bool &sensorReady, const char* sensorName) {
    if (!sensorReady) {
        return 65535; // Invalid reading
    }
    
    uint16_t distance = 0;
    
    // Try to read with error handling
    try {
        distance = sensor.readRangeContinuousMillimeters();
        if (sensor.timeoutOccurred()) {
            Serial.print(sensorName);
            Serial.print(" timeout | ");
            return 65535;
        }
    } catch (...) {
        Serial.print(sensorName);
        Serial.print(" read error | ");
        return 65535;
    }
    
    return distance;
}

void loop() {
    // Read and display distances with better error handling
    uint16_t distance1 = readSensor(sensor1, sensor1Ready, "Sensor 1");
    if (distance1 != 65535) {
        Serial.print("Sensor 1: ");
        Serial.print(distance1);
        Serial.print(" mm | ");
    }
    
    uint16_t distance2 = readSensor(sensor2, sensor2Ready, "Sensor 2");
    if (distance2 != 65535) {
        Serial.print("Sensor 2: ");
        Serial.print(distance2);
        Serial.print(" mm | ");
    }
    
    uint16_t distance3 = readSensor(sensor3, sensor3Ready, "Sensor 3");
    if (distance3 != 65535) {
        Serial.print("Sensor 3: ");
        Serial.print(distance3);
        Serial.print(" mm");
    }
    
    Serial.println();
    delay(200); // Longer delay between readings
}