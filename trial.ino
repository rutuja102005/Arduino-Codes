#include <ESP8266WiFi.h>

// ==== Wi-Fi Credentials ====
const char* ssid = "YOUR_WIFI_SSID";         // Replace with your Wi-Fi name
const char* password = "YOUR_WIFI_PASSWORD"; // Replace with your Wi-Fi password

// ==== ThingSpeak Settings ====
const char* host = "api.thingspeak.com";
String apiKey = "11ES5R8WSII6FTKT";          // Replace with your ThingSpeak API key

// ==== Pin Definitions ====
#define PH_VCC_PIN D1           // GPIO5 powers pH sensor
#define MOISTURE_VCC_PIN D2     // GPIO4 powers moisture sensor
#define ANALOG_PIN A0           // Shared analog pin

void setup() {
  Serial.begin(115200);

  // Set control pins as output
  pinMode(PH_VCC_PIN, OUTPUT);
  pinMode(MOISTURE_VCC_PIN, OUTPUT);

  // Ensure both sensors are off
  digitalWrite(PH_VCC_PIN, LOW);
  digitalWrite(MOISTURE_VCC_PIN, LOW);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
}

void loop() {
  float pH = readPHSensor();
  delay(1000);
  int moisture = readMoistureSensor();

  Serial.print("pH: ");
  Serial.println(pH, 2);
  Serial.print("Moisture: ");
  Serial.print(moisture);
  Serial.println("%");

  sendToThingSpeak(pH, moisture);
  delay(20000); // Wait at least 15 sec between ThingSpeak updates
}

float readPHSensor() {
  digitalWrite(MOISTURE_VCC_PIN, LOW); // Turn off moisture sensor
  digitalWrite(PH_VCC_PIN, HIGH);      // Turn on pH sensor
  delay(1000);                          // Wait to stabilize

  int raw = analogRead(ANALOG_PIN);
  float voltage = raw * (3.3 / 1023.0); // 10-bit ADC
  float pH = 3.5 * voltage;             // Simple estimation (calibrate for accuracy)

  digitalWrite(PH_VCC_PIN, LOW);       // Turn off pH sensor
  return pH;
}

int readMoistureSensor() {
  digitalWrite(PH_VCC_PIN, LOW);           // Turn off pH sensor
  digitalWrite(MOISTURE_VCC_PIN, HIGH);    // Turn on moisture sensor
  delay(1000);                              // Wait to stabilize

  int raw = analogRead(ANALOG_PIN);
  int moisturePercent = map(raw, 1023, 0, 0, 100); // Calibrate as needed

  digitalWrite(MOISTURE_VCC_PIN, LOW);     // Turn off moisture sensor
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
