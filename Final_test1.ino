#include <WiFi.h>
#include "ThingSpeak.h"
#include <ESP32Servo.h>
#include <BluetoothSerial.h>

// ==================== USER CONFIGURATION ====================
const char* ssid = "Galaxy M028e8e";         
const char* password = "13042023";
unsigned long myChannelNumber = 1;     
const char* myWriteAPIKey = "3DJEVA456A65TS0W";
// =============================================================

WiFiClient client;
BluetoothSerial SerialBT;
Servo myservo;

// Motor pins
#define ENA 25
#define IN1 26
#define IN2 27
#define ENB 14
#define IN3 12
#define IN4 17

// Sensor pins
#define PH_PIN 34
#define MOISTURE_PIN 35

// Timing for ThingSpeak
unsigned long lastThingSpeakTime = 0;
const unsigned long thingSpeakInterval = 20000; // 20 seconds

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_Car");
  Serial.println("Bluetooth Car Ready");

  // WiFi connection
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi connected!");
  ThingSpeak.begin(client);

  // Servo setup
  myservo.attach(13);

  // Motor pin setup
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
}

void loop() {
  // Handle Bluetooth commands
  if (SerialBT.available()) {
    char command = SerialBT.read();
    switch (command) {
      case 'F': forward(); break;
      case 'B': backward(); break;
      case 'L': left(); break;
      case 'R': right(); break;
      case 'S': stopCar(); break;
      case 'U': myservo.write(0); break;
      case 'D': myservo.write(90); break;
    }
  }

  // Handle ThingSpeak updates without blocking
  if (millis() - lastThingSpeakTime >= thingSpeakInterval) {
    sendSensorData();
    lastThingSpeakTime = millis();
  }
}

// ====== Motor Control Functions ======
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

// ====== Sensor Reading and ThingSpeak Upload ======
void sendSensorData() {
  // Read pH
  int phRaw = analogRead(PH_PIN);
  float voltage = phRaw * (3.3 / 4095.0);
  float pH = 7 + ((2.5 - voltage) / 0.18);

  // Read soil moisture
  int moistureValue = analogRead(MOISTURE_PIN);
  int moisturePercent = map(moistureValue, 4095, 0, 0, 100);

  // Print readings
  Serial.print("🌱 pH: "); Serial.println(pH, 2);
  Serial.print("💧 Moisture: "); Serial.print(moisturePercent); Serial.println("%");

  // Send to ThingSpeak
  ThingSpeak.setField(1, pH);
  ThingSpeak.setField(2, moisturePercent);
  int statusCode = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  if (statusCode == 200) {
    Serial.println("📤 Data sent to ThingSpeak successfully!");
  } else {
    Serial.println("❌ Error sending data: " + String(statusCode));
  }
}

