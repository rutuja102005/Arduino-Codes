#include "DabbleESP32.h"

// Motor Pins (adjust as per your wiring)
#define IN1 14
#define IN2 27
#define ENA 26
#define IN3 25
#define IN4 33
#define ENB 32

void setup() {
  Serial.begin(115200);
  Dabble.begin("ESP32_RC_BOT"); // Dabble-compatible name

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);
}

void loop() {
  Dabble.processInput();  // Must call this regularly

  int x = GamePad.getXaxisData();  // -7 to +7
  int y = GamePad.getYaxisData();  // -7 to +7

  if (y > 2) {
    moveForward();
  } else if (y < -2) {
    moveBackward();
  } else if (x > 2) {
    turnRight();
  } else if (x < -2) {
    turnLeft();
  } else {
    stopMotors();
  }
}

void moveForward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(ENA, HIGH);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  digitalWrite(ENB, HIGH);
}

void moveBackward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(ENA, HIGH);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  digitalWrite(ENB, HIGH);
}

void turnLeft() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(ENA, HIGH);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  digitalWrite(ENB, HIGH);
}

void turnRight() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(ENA, HIGH);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  digitalWrite(ENB, HIGH);
}

void stopMotors() {
  digitalWrite(ENA, LOW);
  digitalWrite(ENB, LOW);
}

