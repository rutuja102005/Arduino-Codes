#include <WiFi.h>
#include <ESP32Servo.h>

// Wi-Fi credentials
const char* ssid = "YOUR_WIFI_SSID";             // Replace with your Wi-Fi SSID
const char* password = "YOUR_WIFI_PASSWORD";     // Replace with your Wi-Fi password

// ThingSpeak API
const char* host = "api.thingspeak.com";
String apiKey = "11ES5R8WSII6FTKT";              // Your ThingSpeak Write API Key
const long channelId = 3025032;                  // Your ThingSpeak Channel ID

// Pin Definitions
#define PH_SENSOR_PIN 36         // GPIO36 = A0
#define MOISTURE_SENSOR_PIN 39   // GPIO39 = A3
#define SERVO_PH_PIN 18
#define SERVO_MOISTURE_PIN 19

Servo servoPH;
Servo servoMoisture;

void setup() {
  Serial.begin(115200);

  // Attach servos
  servoPH.attach(SERVO_PH_PIN);
  servoMoisture.attach(SERVO_MOISTURE_PIN);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("Channel ID: "); Serial.println(channelId);
}

void loop() {
  float pHValue = readPHSensor();
  int moisture = readMoistureSensor();

  Serial.print("pH: "); Serial.println(pHValue);
  Serial.print("Moisture: "); Serial.print(moisture); Serial.println("%");

  sendToThingSpeak(pHValue, moisture);

  delay(20000); // 20 seconds delay (ThingSpeak minimum interval is 15s)
}

float readPHSensor() {
  // Lower pH probe
  servoPH.write(90);
  delay(3000);  // Wait for probe to settle

  int raw = analogRead(PH_SENSOR_PIN);
  float voltage = raw * (3.3 / 4095.0);  // 12-bit ADC
  float pH = 3.5 * voltage;  // Approximate, calibrate as needed

  // Retract
  servoPH.write(0);
  delay(1000);

  return pH;
}

int readMoistureSensor() {
  // Lower moisture probe
  servoMoisture.write(90);
  delay(3000);  // Wait to stabilize

  int raw = analogRead(MOISTURE_SENSOR_PIN);
  int moisturePercent = map(raw, 4095, 0, 0, 100); // Adjust based on calibration

  // Retract
  servoMoisture.write(0);
  delay(1000);

  return moisturePercent;
}

void sendToThingSpeak(float pH, int moisture) {
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("Connection to ThingSpeak failed");
    return;
  }

  String url = "/update?api_key=" + apiKey +
               "&field1=" + String(pH, 2) +
               "&field2=" + String(moisture);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("Data sent to ThingSpeak");
}
