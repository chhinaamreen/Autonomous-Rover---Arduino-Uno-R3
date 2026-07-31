#include <AFMotor.h>

AF_DCMotor motor1(1);  // M1 - Back right
AF_DCMotor motor2(2);  // M2 - Front right
AF_DCMotor motor3(3);  // M3 - Back left
AF_DCMotor motor4(4);  // M4 - Front left

const int MOTOR_SPEED = 200;

void setup() {
  motor1.setSpeed(MOTOR_SPEED);
  motor2.setSpeed(MOTOR_SPEED);
  motor3.setSpeed(MOTOR_SPEED);
  motor4.setSpeed(MOTOR_SPEED);

  stopRover();
  delay(2000);

  // Confirmed physical-forward direction
  motor1.run(BACKWARD);
  motor2.run(BACKWARD);
  motor3.run(BACKWARD);
  motor4.run(BACKWARD);

  delay(3000);

  stopRover();
}

void loop() {
}

void stopRover() {
  motor1.run(RELEASE);
  motor2.run(RELEASE);
  motor3.run(RELEASE);
  motor4.run(RELEASE);
}
