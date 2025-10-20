#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "PGLAB 0926";
const char* password = "123456789";
const char* server = "http://api.thingspeak.com/update";
String apiKey = "11ES5R8WSII6FTKT";

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, 16, 17); // RX=16, TX=17 (only RX used here)

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
}

void loop() {
  if (Serial2.available()) {
    String data = Serial2.readStringUntil('\n');
    Serial.println("Received from Nano: " + data);

    int moistureIndex = data.indexOf("Moisture:");
    int phIndex = data.indexOf(",PH:");

    if (moistureIndex != -1 && phIndex != -1) {
      String moistureVal = data.substring(moistureIndex + 9, phIndex);
      String phVal = data.substring(phIndex + 4);

      if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = server;
        url += "?api_key=" + apiKey;
        url += "&field1=" + moistureVal;
        url += "&field2=" + phVal;

        http.begin(url);
        int httpCode = http.GET();
        if (httpCode > 0) {
          Serial.println("Data sent to ThingSpeak");
        } else {
          Serial.println("Error sending data");
        }
        http.end();
      }
    }
  }

  delay(15000); // ThingSpeak free account limit: 1 update per 15 seconds
}
