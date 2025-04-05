
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
// test_MOTORs();
}
void loop() {
getCamData();  // always get latest visual info

if (!isnan(ballAngle) && !isnan(ballDistance)) {
if (ballDistance > 20) {
    // Far from ball — go straight to it
    moveRobot(ballAngle, 1.0, 0);
} else {
    // Near the ball
    double goalAngle;
    bool goalSeen = false;

    if (strcmp(ownGoal, "blue") == 0 && !isnan(lastyellowGoalAngle)) {
        goalAngle = lastyellowGoalAngle;
        goalSeen = true;
    } else if (strcmp(ownGoal, "yellow") == 0 && !isnan(lastblueGoalAngle)) {
        goalAngle = lastblueGoalAngle;
        goalSeen = true;
    }

    if (goalSeen) {
        double angleDiff = goalAngle - ballAngle;
        if (angleDiff > 180) angleDiff -= 360;
        if (angleDiff < -180) angleDiff += 360;

        if (abs(angleDiff) < 10) {
            // Aligned — push through ball
            moveRobot(ballAngle, 1.0, 0);
        } else {
            // Not aligned — orbit to align
            double orbitAngle = clipAngleTo360(goalAngle + 90);  // orbit
            moveRobot(orbitAngle, 1.0, 0);
        }
    } else {
        // Goal not seen — just push the ball forward
        moveRobot(ballAngle, 1.0, 0);
    }
}
} else {
// No ball detected — fallback to own goal
double retreatAngle = (strcmp(ownGoal, "blue") == 0) ? lastblueGoalAngle : lastyellowGoalAngle;
moveRobot(retreatAngle, 1.0, 0);
}
}

// void loop() {
//     getCamData();
//     moveRobot(lastballAngle, 1.6, 0);
// }

// void loop() {
// //     if (firstloop) {}
// //         for (int i = 0; i < 500; i++) {
// //             moveRobot(0, 2, 0);
// //             delay(10);
// //         }
// //         firstloop = false;
// //     }
//     // if (Serial_CAM.available()) {
//     //     String received = Serial_CAM.readStringUntil('\n');
//     //     Serial.print("Received from Teensy: ");
//     //     Serial.println(received);
//     // }
//     // getCamData();
//     // rotateToAngle(lastballAngle);
//     // moveRobot(lastballAngle, 2, (lastballAngle > 180) ? -0.2 : 0.2);
//     // moveRobot(lastballAngle, 0.7, 0);
//     // if (!isnan(ballAngle)){
//     //     moveRobot(lastballAngle, 0.7, 0);
//     // }
//     // else {
//     //     moveRobot(0, 2, 0);
//     // }
//     // moveRobot(0, 2, 0);
//     // Serial.println("test");
//     // test_MOTORs();
//     // moveRobot(0, 2, 0);
//     // test_MOTORs();
//     // moveRobot(45, 2, 0);
//     // auto [angle, size] = getSerial_L1();
//     // if (!isnan(angle) || !isnan(size)) {
//     //     //line detected
//     //     moveRobot(angle, 2, 0);
//     // }
//     // else {
//         //line not detected
//         // getCamData();
//         // if (isnan(ballX) || isnan(ballY) || isnan(ballAngle) || isnan(ballDistance)) {
//         //     //ball not detected, run back towards own goal
//         //     if (ownGoal == "blue") {
//         //         moveRobot(lastblueGoalAngle, 2, 0);
//         //     }
//         //     else {
//         //         moveRobot(lastyellowGoalAngle, 2, 0);
//         //     }
//         // }
//         // else {
//         //     //ball detected, move towards ball
//         //     if (ballDistance > 20) {
//         //         //still not near ball -> move towards ball
//         //         moveRobot(ballAngle, 2, 0);
//         //     }
//         //     else {
//         //         //near ball, circle around ball
//         //         if (!(abs(((ownGoal == "blue") ? lastyellowGoalAngle : lastblueGoalAngle) - ballAngle) < 10)) {
//         //             //not aligned, continue circling
//         //             moveRobot(clipAngleTo360(((ownGoal == "blue") ? lastyellowGoalAngle : lastblueGoalAngle) + 90), 2, 0);
//         //         }
//         //         else {
//         //             //aligned, move towards goal
//         //             moveRobot(clipAngleTo360(((ownGoal == "blue") ? lastyellowGoalAngle : lastblueGoalAngle) + 90), 2, 0);
//         //         }
//         //     }
//         // }
//     // }
//     // Serial.print(angle);
//     // Serial.print(" | ");
//     // Serial.println(size);
//     // Serial.println("PRINTSTH");
//     // Serial_L1.write("Hello from Teensy!\n");
//     // getCamData();
//     // Serial.println("test");
//     getCamData();  // update camera readings

//     bool ballVisible = !isnan(ballAngle) && !isnan(ballDistance);
//     bool yellowGoalVisible = !isnan(yellowGoalAngle);
//     bool blueGoalVisible = !isnan(blueGoalAngle);

//     if (ballVisible) {
//         if (ballDistance > 20) {
//             // Approach the ball directly
//             moveRobot(ballAngle, 1.0, 0);
//         } else {
//             // Close to ball, try aligning to opponent goal
//             bool useYellow = strcmp(ownGoal, "blue") == 0;
//             double goalAngle = useYellow ? lastyellowGoalAngle : lastblueGoalAngle;
//             bool goalVisible = useYellow ? yellowGoalVisible : blueGoalVisible;

//             if (goalVisible) {
//                 // Calculate angular error between goal and ball
//                 double angleDiff = goalAngle - ballAngle;
//                 if (angleDiff > 180) angleDiff -= 360;
//                 if (angleDiff < -180) angleDiff += 360;

//                 if (abs(angleDiff) < 10) {
//                     // Aligned with goal — drive through ball
//                     moveRobot(ballAngle, 1.0, 0);
//                 } else {
//                     // Orbit around ball to align
//                     double orbitAngle = clipAngleTo360(goalAngle + 90);
//                     moveRobot(orbitAngle, 1.0, 0);
//                 }
//             } else {
//                 // Goal not visible — just push ball straight ahead
//                 moveRobot(ballAngle, 1.0, 0);
//             }
//         }
//     } else {
//         // Ball not visible — fallback behavior: go home
//         double fallbackAngle = strcmp(ownGoal, "blue") == 0 ? lastblueGoalAngle : lastyellowGoalAngle;
//         bool fallbackGoalVisible = strcmp(ownGoal, "blue") == 0 ? blueGoalVisible : yellowGoalVisible;

//         if (fallbackGoalVisible) {
//             moveRobot(fallbackAngle, 1.0, 0);
//         } else {
//             // Nothing visible — spin to search
//             moveRobot(0, 0, 0.8);
//         }
//     }
// }
// void setup() {
//     pinMode(13, OUTPUT);
//   }
