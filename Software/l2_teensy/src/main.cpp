#include <utility>
#include "main.h"

using namespace std;
#define ll long long

//Functional Functions
void setupMotors() {
    //MOTOR 1
    pinMode(MOTOR_1_IN_A, OUTPUT);
    pinMode(MOTOR_1_IN_B, OUTPUT);
    pinMode(MOTOR_1_PWM, OUTPUT);
    analogWriteFrequency(MOTOR_1_PWM, 146484);

    //MOTOR 2
    pinMode(MOTOR_2_IN_A, OUTPUT);
    pinMode(MOTOR_2_IN_B, OUTPUT);
    pinMode(MOTOR_2_PWM, OUTPUT);
    analogWriteFrequency(MOTOR_2_PWM, 146484);

    //MOTOR 3
    pinMode(MOTOR_3_IN_A, OUTPUT);
    pinMode(MOTOR_3_IN_B, OUTPUT);
    pinMode(MOTOR_3_PWM, OUTPUT);
    analogWriteFrequency(MOTOR_3_PWM, 146484);

    //MOTOR 4
    pinMode(MOTOR_4_IN_A, OUTPUT);
    pinMode(MOTOR_4_IN_B, OUTPUT);
    pinMode(MOTOR_4_PWM, OUTPUT);
    analogWriteFrequency(MOTOR_4_PWM, 146484);

    Serial.println("Motors pinModes defined (by right...)");
}

pair<double, double> getSerial_L1() {
    if (Serial_L1.available()) {
        String received = Serial_L1.readStringUntil('\n');
        Serial.print("Received from ESP32C3: ");
        Serial.println(received);
        
        // Optional: Trim any trailing \r or whitespace
        received.trim();
        // Parse the line
        int spaceIndex = received.indexOf(' ');
        if (spaceIndex != -1) {
            String angleStr = received.substring(0, spaceIndex);
            String sizeStr = received.substring(spaceIndex + 1);

            double angle = angleStr.toFloat();
            double size = sizeStr.toFloat();

            return {angle, size};
            // Serial.print("Angle: ");
            // Serial.println(angle);
            // Serial.print("Size: ");
            // Serial.println(size);
        }
        else {
            // Serial.println("Malformed line: missing space");
            return {NAN, NAN};
        }
    }
}

// Move the robot in any direction using your 34.4° X-drive setup.
// angleDeg: movement direction in degrees (0° = forward, 90° = right)
// speed:    how fast to move (0.0 to 1.0)
// omega:    how fast to rotate (positive = clockwise)
void moveRobot(double angleDeg, double speed, double omega) {
    // --- Step 1: Convert direction + speed to x/y movement ---
    double angleRad = angleDeg * DEG_TO_RAD; // degrees → radians
    double vx = speed * sin(angleRad);       // x: rightward velocity
    double vy = speed * cos(angleRad);       // y: forward velocity

    // --- Step 2: Project that onto your 34.4°-mounted motors ---
    constexpr double MOUNT_ANGLE_DEG = 34.4;
    double cosA = cos(MOUNT_ANGLE_DEG * DEG_TO_RAD);
    double sinA = sin(MOUNT_ANGLE_DEG * DEG_TO_RAD);

    // Motor mixing — match motor numbers to positions:
    double m1 = vy * sinA + vx * cosA + omega;  // Motor 1 = front left
    double m2 = vy * sinA - vx * cosA + omega;  // Motor 2 = back left
    double m3 = vy * sinA - vx * cosA - omega;  // Motor 3 = back right
    double m4 = vy * sinA + vx * cosA - omega;  // Motor 4 = front right

    // --- Step 3: Normalize motor speeds so none exceed ±1.0 ---
    double maxMag = max({abs(m1), abs(m2), abs(m3), abs(m4), 1.0});
    m1 /= maxMag;
    m2 /= maxMag;
    m3 /= maxMag;
    m4 /= maxMag;

    // --- Step 4: Convert [-1.0, 1.0] to PWM [0, 255] and set motor pins ---
    auto setMotor = [](int inA, int inB, int pwmPin, double power) {
        bool forward = (power >= 0);
        int pwmVal = abs(power * 255);
        digitalWrite(inA, forward ? HIGH : LOW);
        digitalWrite(inB, forward ? LOW : HIGH);
        analogWrite(pwmPin, pwmVal);
    };

    setMotor(MOTOR_1_IN_A, MOTOR_1_IN_B, MOTOR_1_PWM, m1); // Front Left
    setMotor(MOTOR_2_IN_A, MOTOR_2_IN_B, MOTOR_2_PWM, m2); // Back Left
    setMotor(MOTOR_3_IN_A, MOTOR_3_IN_B, MOTOR_3_PWM, m3); // Back Right
    setMotor(MOTOR_4_IN_A, MOTOR_4_IN_B, MOTOR_4_PWM, m4); // Front Right
}

//Debugging Functions
bool debugging = true;

void test_MOTORs() {
    if (debugging) {
        digitalWrite(MOTOR_1_IN_A, LOW);
        digitalWrite(MOTOR_1_IN_B, HIGH);
        analogWrite(MOTOR_1_PWM, 255);
        digitalWrite(MOTOR_2_IN_A, LOW);
        digitalWrite(MOTOR_2_IN_B, HIGH);
        analogWrite(MOTOR_2_PWM, 255);
        digitalWrite(MOTOR_3_IN_A, LOW);
        digitalWrite(MOTOR_3_IN_B, HIGH);
        analogWrite(MOTOR_3_PWM, 255);
        digitalWrite(MOTOR_4_IN_A, LOW);
        digitalWrite(MOTOR_4_IN_B, HIGH);
        analogWrite(MOTOR_4_PWM, 255);
        Serial.println("Motors should spin...");
    }
}

//Actual code
void setup() {
    Serial.begin(115200);
    while (!Serial) { ; } // Wait for serial connection
    Serial_L1.begin(9600);
    Serial_L3.begin(9600);
    Serial_IMU.begin(115200);
    if (debugging) {
        Serial.println("Debugging");
    }
    else {
        Serial.println("Not Debugging");
    }
    setupMotors();
    test_MOTORs();
}

void loop() {
    // test_MOTORs();
    // auto [angle, size] = getSerial_L1();
    // Serial.print(angle);
    // Serial.print(" | ");
    // Serial.println(size);
    // Serial.println("PRINTSTH");
    // Serial_L1.write("Hello from Teensy!\n");
    // Serial.println("test");
}