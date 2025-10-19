#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// WiFi details
const char* ssid = "Galaxy M028e8e";
const char* password = "13042023";

// ThingSpeak channel info
const char* channelID = "3031319";
const char* readAPIKey = "LP9C7AKN7AQENVA1";  // If private channel, fill your read API key

// Relay pins
const int waterPumpRelayPin = 25;
const int fertilizerPumpRelayPin = 26;

// Threshold values
const float moistureThreshold = 40.0;  // Moisture below this triggers water pump ON
const float phMin = 5.5;
const float phMax = 7.5;

void setup() {
  Serial.begin(115200);

  pinMode(waterPumpRelayPin, OUTPUT);
  pinMode(fertilizerPumpRelayPin, OUTPUT);

  // Relays are usually active LOW (adjust if needed)
  digitalWrite(waterPumpRelayPin, HIGH);
  digitalWrite(fertilizerPumpRelayPin, HIGH);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
}

void loop() {
  float phValue = getThingSpeakField(1);
  float moistureValue = getThingSpeakField(2);

  Serial.print("pH: "); Serial.println(phValue);
  Serial.print("Moisture: "); Serial.println(moistureValue);

  // Control water pump
  if (moistureValue >= 0 && moistureValue < moistureThreshold) {
    Serial.println("Moisture low, water pump ON");
    digitalWrite(waterPumpRelayPin, LOW);
  } else {
    Serial.println("Moisture OK, water pump OFF");
    digitalWrite(waterPumpRelayPin, HIGH);
  }

  // Control fertilizer pump
  if (phValue >= 0 && (phValue < phMin || phValue > phMax)) {
    Serial.println("pH out of range, fertilizer pump ON");
    digitalWrite(fertilizerPumpRelayPin, LOW);
  } else {
    Serial.println("pH in range, fertilizer pump OFF");
    digitalWrite(fertilizerPumpRelayPin, HIGH);
  }

  delay(30000); // Check every 30 seconds
}

float getThingSpeakField(int fieldNumber) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected");
    return -1;
  }

  HTTPClient http;
  String url = String("https://api.thingspeak.com/channels/") + channelID + "/fields/" + String(fieldNumber) + "/last.json";
  if (readAPIKey != "") {
    url += "?api_key=" + String(readAPIKey);
  }

  http.begin(url);
  int httpCode = http.GET();

  float value = -1;
  if (httpCode > 0) {
    String payload = http.getString();

    StaticJsonDocument<200> doc;
    DeserializationError err = deserializeJson(doc, payload);
    if (!err) {
      if (fieldNumber == 1 && doc.containsKey("field1")) value = doc["field1"].as<float>();
      else if (fieldNumber == 2 && doc.containsKey("field2")) value = doc["field2"].as<float>();
    } else {
      Serial.println("JSON parse error");
    }
  } else {
    Serial.print("HTTP error: ");
    Serial.println(httpCode);
  }

  http.end();
  return value;
}
