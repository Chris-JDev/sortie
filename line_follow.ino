// line_follower.ino
// Arduino UNO + L298N (HW-95) motor driver
// Listens for 'F','L','R','S' over Serial and drives two motors.
// Prints direction & pin states for debugging.

#include <Arduino.h>

// Motor driver pins (HW-95 definitions)
#define enA    10   // PWM enable Motor A
#define in1     9   // Dir1 Motor A
#define in2     8   // Dir2 Motor A
#define in3     7   // Dir1 Motor B
#define in4     6   // Dir2 Motor B
#define enB     5   // PWM enable Motor B

#define KILL_PIN 2  // Emergency stop (active LOW)

const uint8_t MOTOR_SPEED = 200;  // 0–255 PWM duty

void setup() {
  // Configure motor driver pins
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(enB, OUTPUT);

  // Configure kill switch
  pinMode(KILL_PIN, INPUT_PULLUP);

  // Serial for commands & debug
  Serial.begin(9600);
  while (!Serial) { /* wait for Serial port */ }

  stopMotors();
  Serial.println("Arduino Line-Follower Ready");
}

void loop() {
  // Emergency kill: if button pressed, stop forever
  if (digitalRead(KILL_PIN) == LOW) {
    stopMotors();
    Serial.println("KILL switch engaged — motors stopped.");
    while (true) { }
  }

  // Process incoming command
  if (Serial.available()) {
    char cmd = Serial.read();
    switch (cmd) {
      case 'F': forward();   break;
      case 'L': turnLeft();  break;
      case 'R': turnRight(); break;
      case 'S':
      default:  stopMotors(); break;
    }
  }
}

// ——— Movement routines ———

void forward() {
  analogWrite(enA, MOTOR_SPEED);
  analogWrite(enB, MOTOR_SPEED);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  debugPins("FORWARD");
}

void turnLeft() {
  analogWrite(enA, 0);
  analogWrite(enB, MOTOR_SPEED);
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  debugPins("LEFT");
}

void turnRight() {
  analogWrite(enA, MOTOR_SPEED);
  analogWrite(enB, 0);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  debugPins("RIGHT");
}

void stopMotors() {
  analogWrite(enA, 0);
  analogWrite(enB, 0);
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  debugPins("STOP");
}

// ——— Debug helper ———
// Print direction + pin states & PWM duty
void debugPins(const char* direction) {
  Serial.print(">> Dir: "); Serial.println(direction);

  Serial.print("   PWM enA(pin "); Serial.print(enA);
  Serial.print(")="); Serial.print(getPWM(enA));
  Serial.print(", enB(pin "); Serial.print(enB);
  Serial.print(")="); Serial.println(getPWM(enB));

  Serial.print("   in1(pin "); Serial.print(in1);
  Serial.print(")="); Serial.print(digitalRead(in1)?"HIGH":"LOW");
  Serial.print(", in2(pin "); Serial.print(in2);
  Serial.print(")="); Serial.print(digitalRead(in2)?"HIGH":"LOW");
  Serial.print(", in3(pin "); Serial.print(in3);
  Serial.print(")="); Serial.print(digitalRead(in3)?"HIGH":"LOW");
  Serial.print(", in4(pin "); Serial.print(in4);
  Serial.print(")="); Serial.println(digitalRead(in4)?"HIGH":"LOW");
  Serial.println();
}

// Return current PWM duty for enable pins
int getPWM(int pin) {
  return (pin==enA || pin==enB) ? MOTOR_SPEED : 0;
}
