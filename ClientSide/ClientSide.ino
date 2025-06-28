#include <WiFi.h>
#include <HTTPClient.h>


#define MS1 A0
#define MS2 A1
#define MS3 A2

const char* ssid = "रामबाण";
const char* password = "12345678";
const char* serverIP = "192.168.4.1";  // ESP-Server IP

int readSensor(int s) {
  int sensorValue = analogRead(s);  / // map the 10-bit data to 8-bit data
  return sensorValue;             // Return analog moisture value
}

void checkServerInstructions() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("http://" + String(serverIP) + "/data.json");
    int httpCode = http.GET();

    if (httpCode == 200) {
      String response = http.getString();
      Serial.println("Received from server: " + response);

      if (response.indexOf("\"command\":\"start\"") >= 0) {
        Serial.println("Start command received");
        // do something
      } else if (response.indexOf("\"command\":\"stop\"") >= 0) {
        Serial.println("Stop command received");
        // stop something
      }
    }

    http.end();
  }
}


void setup() {
  Serial.begin(115200);

  // Wifi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to AP");


}

void loop() {

  int moisture1 = readSensor(MS1);
  int moisture2 = readSensor(MS2);
  int moisture3 = readSensor(MS3);
  float moisture = (moisture1+moisture2+moisture3)/3;

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("http://" + String(serverIP) + "/data.json");
    http.addHeader("Content-Type", "application/json");

    String payload = "{";
    payload += "\"moisture1\":\"" + String(moisture1) + "%\",";
    payload += "\"moisture2\":\"" + String(moisture2) + "%\",";
    payload += "\"moisture3\":\"" + String(moisture3) + "%\",";
    payload += "\"moistureAvg\":\"" + String(moisture) + "%\",";
    payload += "\"water\":\"" + String(random(60, 100)) + "%\",";
    payload += "\"temp\":\"" + String(random(60, 100)) + "°C\",";
    payload += "\"ph\":\"6.5\",";
    payload += "\"soilTemp\":\"22°C\"";
    payload += "\"command\":\"None\"";
    payload += "}";

    int response = http.POST(payload);
    Serial.printf("POST [%d] Sent: %s\n", response, payload.c_str());
    http.end();
  }

  delay(10000);
}
