#include <ESP32Servo.h>
#include <BluetoothSerial.h>

BluetoothSerial SerialBT; // Bluetooth object
Servo myservo;            // Servo object

#define ENA 25  // Enable A (left motor)
#define IN1 26
#define IN2 27
#define ENB 14  // Enable B (right motor)
#define IN3 12
#define IN4 17

void setup() {
  myservo.attach(13);
  Serial.begin(115200);
  SerialBT.begin("ESP32_Car");
  Serial.println("Bluetooth Car Ready");

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
}

void loop() {
  if (SerialBT.available()) {
    char command = SerialBT.read();

    switch (command) {
      case 'F': forward(); break;
      case 'B': backward(); break;
      case 'L': left(); break;
      case 'R': right(); break;
      case 'S': stopCar(); break;
      case 'U': myservo.write(0); break;   // Servo to 0°
      case 'D': myservo.write(90); break;  // Servo to 90°
    }
  }
}

void forward() {
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void backward() {
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void left() {
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void right() {
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void stopCar() {
  digitalWrite(ENA, LOW);
  digitalWrite(ENB, LOW);
}
