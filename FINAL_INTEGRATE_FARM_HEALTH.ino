// ================= Libraries =================
#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE
#include <DabbleESP32.h>
#include <WiFi.h>
#include "ThingSpeak.h"
#include <ESP32Servo.h>

// ================= Wi-Fi & ThingSpeak =================
const char* ssid = "Galaxy M028e8e";
const char* password = "13042023";
unsigned long myChannelNumber = 1;  // Your channel number
const char* myWriteAPIKey = "3DJEVA456A65TS0W";

WiFiClient client;

// ================= Motor Pins =================
// Right motor
int enableRightMotor = 22;
int rightMotorPin1 = 16;
int rightMotorPin2 = 17;
// Left motor
int enableLeftMotor = 23;
int leftMotorPin1 = 5;
int leftMotorPin2 = 19;

#define MAX_MOTOR_SPEED 255
const int PWMFreq = 1000;
const int PWMResolution = 8;
// ⚠️ Changed PWM channels to avoid conflict with servo
const int rightMotorPWMSpeedChannel = 0;
const int leftMotorPWMSpeedChannel = 1;

// ================= Servo Pin =================
#define SERVO_PIN 13  // ✅ Use reliable PWM pin
Servo sensorServo;
int servoDownPos = 90; // Adjust for your hardware
int servoUpPos = 0;    // Adjust for your hardware

// ================= Sensor Pins =================
#define PH_PIN 34
#define MOISTURE_PIN 35

// ================= Functions =================
void rotateMotor(int rightMotorSpeed, int leftMotorSpeed) {
  if (rightMotorSpeed < 0) {
    digitalWrite(rightMotorPin1, LOW);
    digitalWrite(rightMotorPin2, HIGH);
  } else if (rightMotorSpeed > 0) {
    digitalWrite(rightMotorPin1, HIGH);
    digitalWrite(rightMotorPin2, LOW);
  } else {
    digitalWrite(rightMotorPin1, LOW);
    digitalWrite(rightMotorPin2, LOW);
  }

  if (leftMotorSpeed < 0) {
    digitalWrite(leftMotorPin1, LOW);
    digitalWrite(leftMotorPin2, HIGH);
  } else if (leftMotorSpeed > 0) {
    digitalWrite(leftMotorPin1, HIGH);
    digitalWrite(leftMotorPin2, LOW);
  } else {
    digitalWrite(leftMotorPin1, LOW);
    digitalWrite(leftMotorPin2, LOW);
  }

  ledcWrite(rightMotorPWMSpeedChannel, abs(rightMotorSpeed));
  ledcWrite(leftMotorPWMSpeedChannel, abs(leftMotorSpeed));
}

void setUpPinModes() {
  pinMode(enableRightMotor, OUTPUT);
  pinMode(rightMotorPin1, OUTPUT);
  pinMode(rightMotorPin2, OUTPUT);

  pinMode(enableLeftMotor, OUTPUT);
  pinMode(leftMotorPin1, OUTPUT);
  pinMode(leftMotorPin2, OUTPUT);

  // PWM setup
  ledcSetup(rightMotorPWMSpeedChannel, PWMFreq, PWMResolution);
  ledcSetup(leftMotorPWMSpeedChannel, PWMFreq, PWMResolution);
  ledcAttachPin(enableRightMotor, rightMotorPWMSpeedChannel);
  ledcAttachPin(enableLeftMotor, leftMotorPWMSpeedChannel);

  rotateMotor(0, 0); // Stop motors initially
}

// ================= Setup =================
void setup() {
  Serial.begin(115200);

  // Motors & Servo
  setUpPinModes();

  // ✅ Attach servo with confirmation
  int servoAttached = sensorServo.attach(SERVO_PIN);
  if (servoAttached == 0) {
    Serial.println("❌ Failed to attach servo! Check pin or conflict.");
  } else {
    Serial.println("✅ Servo attached successfully.");
    sensorServo.write(servoUpPos); // Move to default UP position
  }

  // Dabble Bluetooth
  Dabble.begin("MyFarmBot");

  // Wi-Fi
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 30) {
    delay(500);
    Serial.print(".");
    retries++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ Wi-Fi Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n❌ Wi-Fi Connection Failed!");
  }

  ThingSpeak.begin(client);
}

// ================= Loop =================
void loop() {
  Dabble.processInput();

  // ==== RC Car Movement ====
  int rightMotorSpeed = 0;
  int leftMotorSpeed = 0;

  if (GamePad.isUpPressed()) {
    rightMotorSpeed = MAX_MOTOR_SPEED;
    leftMotorSpeed = MAX_MOTOR_SPEED;
    Serial.print("⬆ UpPressed ");
  }
  if (GamePad.isDownPressed()) {
    rightMotorSpeed = -MAX_MOTOR_SPEED;
    leftMotorSpeed = -MAX_MOTOR_SPEED;
    Serial.print("⬇ DownPressed ");
  }
  if (GamePad.isLeftPressed()) {
    rightMotorSpeed = MAX_MOTOR_SPEED;
    leftMotorSpeed = -MAX_MOTOR_SPEED;
    Serial.print("⬅ LeftPressed ");
  }
  if (GamePad.isRightPressed()) {
    rightMotorSpeed = -MAX_MOTOR_SPEED;
    leftMotorSpeed = MAX_MOTOR_SPEED;
    Serial.print("➡ RightPressed ");
  }

  rotateMotor(rightMotorSpeed, leftMotorSpeed);

  // ==== Servo Control ====
  if (GamePad.isSquarePressed()) {
    Serial.println("⬇ Moving sensors DOWN");
    sensorServo.write(servoDownPos);
    delay(2000); // Wait for movement
  }
  if (GamePad.isCirclePressed()) {
    Serial.println("⬆ Moving sensors UP");
    sensorServo.write(servoUpPos);
    delay(2000); // Wait for movement
  }

  // ==== Sensor Reading & Upload ====
  if (GamePad.isCrossPressed()) { // Press X to take reading and upload
    Serial.println("📡 Taking sensor reading...");

    int phValueRaw = analogRead(PH_PIN);
    int moistureValueRaw = analogRead(MOISTURE_PIN);

    float phLevel = map(phValueRaw, 0, 4095, 0, 14);
    float moisturePercent = map(moistureValueRaw, 0, 4095, 0, 100);

    Serial.print("pH: ");
    Serial.println(phLevel);
    Serial.print("Moisture: ");
    Serial.print(moisturePercent);
    Serial.println("%");

    ThingSpeak.setField(1, phLevel);
    ThingSpeak.setField(2, moisturePercent);
    int status = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

    if (status == 200) {
      Serial.println("✅ Data sent to ThingSpeak!");
    } else {
      Serial.print("❌ Problem sending data. HTTP error code: ");
      Serial.println(status);
    }
  }
}
