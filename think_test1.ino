#include <WiFi.h>
#include "ThingSpeak.h"

// ==================== USER CONFIGURATION ====================
const char* ssid = "Galaxy M028e8e";          // 🔹 Your WiFi name
const char* password = "13042023";  // 🔹 Your WiFi password
unsigned long myChannelNumber =  3031319;      // 🔹 Your ThingSpeak channel number
const char* myWriteAPIKey = "3DJEVA456A65TS0W"; // 🔹 Your ThingSpeak write API key
// =============================================================

WiFiClient client;

// Sensor pins
#define PH_PIN 34          // pH sensor analog pin
#define MOISTURE_PIN 35    // Soil moisture sensor analog pin

void setup() {
  Serial.begin(115200);

  // Connect to WiFi
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi connected!");

  // Initialize ThingSpeak
  ThingSpeak.begin(client);
}

void loop() {
  // === Read pH value ===
  int phRaw = analogRead(PH_PIN);
  float voltage = phRaw * (3.3 / 4095.0);    // ADC conversion for ESP32
  float pH = 7 + ((2.5 - voltage) / 0.18);   // Approx formula (calibrate later)

  // === Read soil moisture value ===
  int moistureValue = analogRead(MOISTURE_PIN);
  int moisturePercent = map(moistureValue, 4095, 0, 0, 100); // 0-100% scale

  // === Print readings ===
  Serial.print("🌱 pH: "); Serial.println(pH, 2);
  Serial.print("💧 Moisture: "); Serial.print(moisturePercent); Serial.println("%");

  // === Send to ThingSpeak ===
  ThingSpeak.setField(1, pH);               // Field 1 = pH
  ThingSpeak.setField(2, moisturePercent);  // Field 2 = moisture

  int statusCode = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  if (statusCode == 200) {
    Serial.println("📤 Data sent to ThingSpeak successfully!");
  } else {
    Serial.println("❌ Error sending data: " + String(statusCode));
  }

  delay(20000); // ThingSpeak limit = every 15 seconds minimum
}
