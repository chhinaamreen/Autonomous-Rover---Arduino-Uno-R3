// Left motor
const int ENA = 5;
const int IN1 = 2;
const int IN2 = 3;

// Right motor
const int ENB = 6;
const int IN3 = 4;
const int IN4 = 7;

void setup() {
  // Set all motor pins as outputs
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Move both motors forward
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  // Motor speed: 0 to 255
  analogWrite(ENA, 120);
  analogWrite(ENB, 120);

  // Continue moving for 5 seconds
  delay(5000);

  // Stop both motors
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

void loop() {
  // Nothing here, so the rover stays stopped
}
