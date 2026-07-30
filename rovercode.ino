#include <AFMotor.h>
#include <Servo.h>

// ==================================================
// MOTOR CONNECTIONS
// ==================================================

AF_DCMotor motor1(1);  // M1 - Back right
AF_DCMotor motor2(2);  // M2 - Front right
AF_DCMotor motor3(3);  // M3 - Back left
AF_DCMotor motor4(4);  // M4 - Front left

// ==================================================
// ULTRASONIC SENSOR
// ==================================================

const int TRIG_PIN = A1;
const int ECHO_PIN = A0;

// ==================================================
// SERVO
// ==================================================

Servo sensorServo;

const int SERVO_PIN = 10;

const int CENTER_ANGLE = 90;
const int RIGHT_LOOK_ANGLE = 20;
const int LEFT_LOOK_ANGLE = 160;

// ==================================================
// SPEED AND DISTANCE SETTINGS
// ==================================================

const int DRIVE_SPEED = 200;
const int REVERSE_SPEED = 180;
const int TURN_SPEED = 200;

const int STOP_DISTANCE_CM = 20;

// ==================================================
// TIMING SETTINGS
// ==================================================

// Adjust these until the rover turns approximately 90 degrees
const int RIGHT_90_TIME_MS = 550;
const int LEFT_90_TIME_MS = 550;

// Approximately double the 90-degree turning time
const int RIGHT_180_TIME_MS = 1100;
const int LEFT_180_TIME_MS = 1100;

// How long to reverse when every direction is blocked
const int BACKUP_TIME_MS = 500;

// How long to move forward after completing a turn
const int CLEAR_OBSTACLE_TIME_MS = 600;

// Servo movement settling time
const int SERVO_SETTLE_TIME_MS = 600;

// Remembers the previous avoidance direction
bool lastTurnWasRight = true;

void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  setAllMotorSpeeds(DRIVE_SPEED);

  sensorServo.attach(SERVO_PIN);
  sensorServo.write(CENTER_ANGLE);

  stopRover();

  // Give the servo time to point straight
  delay(800);

  // Wait after switching the rover on
  delay(2000);
}

void loop() {
  // Keep the sensor facing forward while driving
  sensorServo.write(CENTER_ANGLE);

  long frontDistance = getStableDistanceCM();

  // Move when there is no nearby obstacle
  if (frontDistance == 0 ||
      frontDistance > STOP_DISTANCE_CM) {

    moveForward();
  }
  else {
    avoidObstacle();
  }

  delay(50);
}

// ==================================================
// OBSTACLE AVOIDANCE
// ==================================================

void avoidObstacle() {
  stopRover();
  delay(300);

  // Look right
  long rightDistance = lookRight();

  // Look left
  long leftDistance = lookLeft();

  // Return sensor to the center
  sensorServo.write(CENTER_ANGLE);
  delay(SERVO_SETTLE_TIME_MS);

  bool rightIsClear = isDirectionClear(rightDistance);
  bool leftIsClear = isDirectionClear(leftDistance);

  // ----------------------------------------------
  // RIGHT CLEAR, LEFT BLOCKED
  // ----------------------------------------------

  if (rightIsClear && !leftIsClear) {
    turnRight90();
  }

  // ----------------------------------------------
  // LEFT CLEAR, RIGHT BLOCKED
  // ----------------------------------------------

  else if (leftIsClear && !rightIsClear) {
    turnLeft90();
  }

  // ----------------------------------------------
  // BOTH SIDES CLEAR
  // Choose the side with more open space
  // ----------------------------------------------

  else if (rightIsClear && leftIsClear) {
    long rightSpace = distanceScore(rightDistance);
    long leftSpace = distanceScore(leftDistance);

    if (rightSpace >= leftSpace) {
      turnRight90();
    }
    else {
      turnLeft90();
    }
  }

  // ----------------------------------------------
  // BOTH SIDES BLOCKED
  // Reverse and perform a 180-degree turn
  // ----------------------------------------------

  else {
    moveBackward();
    delay(BACKUP_TIME_MS);

    stopRover();
    delay(300);

    // Turn opposite to the previous avoidance turn
    if (lastTurnWasRight) {
      turnLeft180();
    }
    else {
      turnRight180();
    }
  }

  stopRover();
  delay(300);

  // Move away from the obstacle after turning
  moveForward();
  delay(CLEAR_OBSTACLE_TIME_MS);
}

// ==================================================
// LOOK RIGHT AND LEFT
// ==================================================

long lookRight() {
  stopRover();

  sensorServo.write(RIGHT_LOOK_ANGLE);
  delay(SERVO_SETTLE_TIME_MS);

  return getStableDistanceCM();
}

long lookLeft() {
  stopRover();

  sensorServo.write(LEFT_LOOK_ANGLE);
  delay(SERVO_SETTLE_TIME_MS);

  return getStableDistanceCM();
}

// ==================================================
// 90-DEGREE TURNS
// ==================================================

void turnRight90() {
  turnRight();
  delay(RIGHT_90_TIME_MS);

  lastTurnWasRight = true;
}

void turnLeft90() {
  turnLeft();
  delay(LEFT_90_TIME_MS);

  lastTurnWasRight = false;
}

// ==================================================
// 180-DEGREE TURNS
// ==================================================

void turnRight180() {
  turnRight();
  delay(RIGHT_180_TIME_MS);

  lastTurnWasRight = true;
}

void turnLeft180() {
  turnLeft();
  delay(LEFT_180_TIME_MS);

  lastTurnWasRight = false;
}

// ==================================================
// MOVE FORWARD
// ==================================================

void moveForward() {
  setAllMotorSpeeds(DRIVE_SPEED);

  // Confirmed physical-forward direction
  motor1.run(BACKWARD);
  motor2.run(BACKWARD);
  motor3.run(BACKWARD);
  motor4.run(BACKWARD);
}

// ==================================================
// MOVE BACKWARD
// ==================================================

void moveBackward() {
  setAllMotorSpeeds(REVERSE_SPEED);

  // Opposite of your confirmed forward direction
  motor1.run(FORWARD);
  motor2.run(FORWARD);
  motor3.run(FORWARD);
  motor4.run(FORWARD);
}

// ==================================================
// TURN RIGHT
// ==================================================

void turnRight() {
  setAllMotorSpeeds(TURN_SPEED);

  // Right wheels physically backward
  motor1.run(FORWARD);
  motor2.run(FORWARD);

  // Left wheels physically forward
  motor3.run(BACKWARD);
  motor4.run(BACKWARD);
}

// ==================================================
// TURN LEFT
// ==================================================

void turnLeft() {
  setAllMotorSpeeds(TURN_SPEED);

  // Right wheels physically forward
  motor1.run(BACKWARD);
  motor2.run(BACKWARD);

  // Left wheels physically backward
  motor3.run(FORWARD);
  motor4.run(FORWARD);
}

// ==================================================
// STOP
// ==================================================

void stopRover() {
  motor1.run(RELEASE);
  motor2.run(RELEASE);
  motor3.run(RELEASE);
  motor4.run(RELEASE);
}

// ==================================================
// SET MOTOR SPEEDS
// ==================================================

void setAllMotorSpeeds(int speedValue) {
  motor1.setSpeed(speedValue);
  motor2.setSpeed(speedValue);
  motor3.setSpeed(speedValue);
  motor4.setSpeed(speedValue);
}

// ==================================================
// DETERMINE WHETHER A DIRECTION IS CLEAR
// ==================================================

bool isDirectionClear(long distance) {
  // A zero reading means no echo was returned.
  // This is treated as open/out of range.
  return distance == 0 ||
         distance > STOP_DISTANCE_CM;
}

// Used when both directions are clear
long distanceScore(long distance) {
  if (distance == 0) {
    return 400;
  }

  return distance;
}

// ==================================================
// STABLE DISTANCE READING
// ==================================================

long getStableDistanceCM() {
  long totalDistance = 0;
  int validReadings = 0;

  // Take three readings to reduce false measurements
  for (int i = 0; i < 3; i++) {
    long distance = getDistanceCM();

    if (distance > 0) {
      totalDistance += distance;
      validReadings++;
    }

    delay(40);
  }

  if (validReadings == 0) {
    return 0;
  }

  return totalDistance / validReadings;
}

// ==================================================
// ULTRASONIC DISTANCE
// ==================================================

long getDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  unsigned long duration =
      pulseIn(ECHO_PIN, HIGH, 30000UL);

  if (duration == 0) {
    return 0;
  }

  return duration * 0.0343 / 2.0;
}