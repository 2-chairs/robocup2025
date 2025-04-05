#include <utility>
// #include <Adafruit_BNO08x.h>
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

double clipAngleTo360(double angle) {
    angle = fmod(angle, 360);
    return angle < 0 ? angle + 360 : angle;
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

// void setReports(void) {
//   Serial.println("Setting desired reports");
//   if (! bno08x.enableReport(SH2_GAME_ROTATION_VECTOR)) {
//     Serial.println("Could not enable game vector");
//   }
// }

// void setupIMU() {
//       if (!bno08x.begin_UART(&Serial_IMU)) {  // Requires a device with > 300 byte UART buffer!
//           while (1) {
//           Serial.println("Failed to find BNO08x chip");
//           }
//       }
//       Serial.println("BNO08x Found!");
//       setReports();
// }

// Move the robot in any direction using your 34.4° X-drive setup.
// angleDeg: movement direction in degrees (0° = forward, 90° = right)
// speed:    how fast to move (0.0 to 1.0)
// omega:    how fast to rotate (positive = clockwise)
void moveRobot(double angleDeg, double speed, double omega) {
    // Convert movement direction to radians
    double angleRad = angleDeg * DEG_TO_RAD;

    // X/Y velocity components
    double vx = speed * cos(angleRad);  // Rightward
    double vy = speed * sin(angleRad);  // Forward

    // Standard X-drive motor vector projection:
    // M1 (Front Left)  = vy + vx + omega
    // M2 (Back Left)   = vy - vx + omega
    // M3 (Back Right)  = vy + vx - omega
    // M4 (Front Right) = vy - vx - omega

    double m1 = vy + vx + omega; // Front Left
    double m2 = vy - vx + omega; // Back Left
    double m3 = vy + vx - omega; // Back Right
    double m4 = vy - vx - omega; // Front Right

    // Normalize if needed
    double maxMag = max({abs(m1), abs(m2), abs(m3), abs(m4), 1.0});
    m1 /= maxMag;
    m2 /= maxMag;
    m3 /= maxMag;
    m4 /= maxMag;

    // Apply motor outputs
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

void getCamData() {
    if (Serial_CAM.available()) {
        String received = Serial_CAM.readStringUntil('\n');
        Serial.print("Received from Teensy: ");
        Serial.println(received);
        received.trim();
        // Prepare 12 double variables
        double values[12];

        int index = 0;
        int from = 0;

        while (index < 12) {
            int space = received.indexOf(' ', from);
            if (space == -1) space = received.length();
            String token = received.substring(from, space);
            from = space + 1;

            values[index] = (token == "none") ? NAN : token.toFloat(); // use nan for missing blobs
            index++;
        }

        ballX = values[0];
        if (!isnan(ballX)) lastballX = ballX;
        ballY = values[1];
        if (!isnan(ballY)) lastballY = ballY;
        ballAngle = values[2];
        if (!isnan(ballAngle)) lastballAngle = ballAngle;
        ballDistance = values[3];
        if (!isnan(ballDistance)) lastballDistance = ballDistance;

        blueGoalX = values[4];
        if (!isnan(blueGoalX)) lastblueGoalX = blueGoalX;
        blueGoalY = values[5];
        if (!isnan(blueGoalY)) lastblueGoalY = blueGoalY;
        blueGoalAngle = values[6];
        if (!isnan(blueGoalAngle)) lastblueGoalAngle = blueGoalAngle;
        blueGoalDistance = values[7];
        if (!isnan(blueGoalDistance)) lastblueGoalDistance = blueGoalDistance;

        yellowGoalX = values[8];
        if (!isnan(yellowGoalX)) lastyellowGoalX = yellowGoalX;
        yellowGoalY = values[9];
        if (!isnan(yellowGoalY)) lastyellowGoalY = yellowGoalY;
        yellowGoalAngle = values[10];
        if (!isnan(yellowGoalAngle)) lastyellowGoalAngle = yellowGoalAngle;
        yellowGoalDistance = values[11];
        if (!isnan(yellowGoalDistance)) lastyellowGoalDistance = yellowGoalDistance;

        Serial.println("=== BALL DATA ===");
        Serial.print("ballX: "); Serial.println(ballX);
        Serial.print("ballY: "); Serial.println(ballY);
        Serial.print("ballAngle: "); Serial.println(ballAngle);
        Serial.print("ballDistance: "); Serial.println(ballDistance);

        Serial.println("=== BLUE GOAL ===");
        Serial.print("blueGoalX: "); Serial.println(blueGoalX);
        Serial.print("blueGoalY: "); Serial.println(blueGoalY);
        Serial.print("blueGoalAngle: "); Serial.println(blueGoalAngle);
        Serial.print("blueGoalDistance: "); Serial.println(blueGoalDistance);

        Serial.println("=== YELLOW GOAL ===");
        Serial.print("yellowGoalX: "); Serial.println(yellowGoalX);
        Serial.print("yellowGoalY: "); Serial.println(yellowGoalY);
        Serial.print("yellowGoalAngle: "); Serial.println(yellowGoalAngle);
        Serial.print("yellowGoalDistance: "); Serial.println(yellowGoalDistance);
    }
}

//Debugging Functions
bool debugging = true;

void test_MOTORs() {
    if (debugging) {
        digitalWrite(MOTOR_1_IN_A, HIGH);
        digitalWrite(MOTOR_1_IN_B, LOW);
        analogWrite(MOTOR_1_PWM, 255);
        digitalWrite(MOTOR_2_IN_A, HIGH);
        digitalWrite(MOTOR_2_IN_B, LOW);
        analogWrite(MOTOR_2_PWM, 255);
        digitalWrite(MOTOR_3_IN_A, HIGH);
        digitalWrite(MOTOR_3_IN_B, LOW);
        analogWrite(MOTOR_3_PWM, 255);
        digitalWrite(MOTOR_4_IN_A, HIGH);
        digitalWrite(MOTOR_4_IN_B, LOW);
        analogWrite(MOTOR_4_PWM, 255);
        Serial.println("Motors should spin...");
    }
}

void test_MOTOR_1() {
    for (int x = 0; x < 255; x++) { 
        digitalWrite(MOTOR_1_IN_A, LOW);
        digitalWrite(MOTOR_1_IN_B, HIGH);
        analogWrite(MOTOR_1_PWM, x);
        delay(10);
    }
    for (int x = 255; x > 0; x--) { 
        digitalWrite(MOTOR_1_IN_A, LOW);
        digitalWrite(MOTOR_1_IN_B, HIGH);
        analogWrite(MOTOR_1_PWM, x);
        delay(1);
    }
    for (int x = 0; x < 255; x++) {
        digitalWrite(MOTOR_1_IN_A, HIGH);
        digitalWrite(MOTOR_1_IN_B, LOW);
        analogWrite(MOTOR_1_PWM, x);
        delay(1);
    }
    for (int x = 255; x > 0; x--) { 
        digitalWrite(MOTOR_1_IN_A, HIGH);
        digitalWrite(MOTOR_1_IN_B, LOW);
        analogWrite(MOTOR_1_PWM, x);
        delay(10);
    }
}

//Actual code
void setup() {
    Serial.begin(115200);
    // while (!Serial) { ; } // Wait for serial connection
    Serial_L1.begin(9600);
    Serial_L3.begin(9600);
    Serial_IMU.begin(115200);
    Serial_CAM.begin(115200);
    Serial.println("UART Intialised... (by right)");
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
    moveRobot(0, 2, 0);
    // test_MOTORs();
    // moveRobot(45, 2, 0);
    // auto [angle, size] = getSerial_L1();
    // if (!isnan(angle) || !isnan(size)) {
    //     //line detected
    //     moveRobot(angle, 2, 0);
    // }
    // else {
    //     //line not detected
    //     getCamData();
    //     if (isnan(ballX) || isnan(ballY) || isnan(ballAngle) || isnan(ballDistance)) {
    //         //ball not detected, run back towards own goal
    //         if (ownGoal == "blue") {
    //             moveRobot(lastblueGoalAngle, 2, 0);
    //         }
    //         else {
    //             moveRobot(lastyellowGoalAngle, 2, 0);
    //         }
    //     }
    //     else {
    //         //ball detected, move towards ball
    //         if (ballDistance > 20) {
    //             //still not near ball -> move towards ball
    //             moveRobot(ballAngle, 2, 0);
    //         }
    //         else {
    //             //near ball, circle around ball
    //             if (!(abs(((ownGoal == "blue") ? lastyellowGoalAngle : lastblueGoalAngle) - ballAngle) < 10)) {
    //                 //not aligned, continue circling
    //                 moveRobot(clipAngleTo360(((ownGoal == "blue") ? lastyellowGoalAngle : lastblueGoalAngle) + 90), 2, 0);
    //             }
    //             else {
    //                 //aligned, move towards goal
    //                 moveRobot(clipAngleTo360(((ownGoal == "blue") ? lastyellowGoalAngle : lastblueGoalAngle) + 90), 2, 0);
    //             }
    //         }
    //     }
    // }
    // Serial.print(angle);
    // Serial.print(" | ");
    // Serial.println(size);
    // Serial.println("PRINTSTH");
    // Serial_L1.write("Hello from Teensy!\n");
    // getCamData();
    // Serial.println("test");
}
// void setup() {
//     pinMode(13, OUTPUT);
//   }
  
//   void loop() {
//     digitalWrite(13, HIGH);
//     delay(1000);
//     digitalWrite(13, LOW);
//     delay(1000);
//   }