#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

const char* ssid = "रामबाण";
const char* password = "12345678";
WebServer server(80);

float airTemp;
float airPressure;
float Altitude;
float airHumidity;
String forecast;

#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme;
bool bmeStatus = false;

void handleFileRead(String path) {
  if (path.endsWith("/")) path += "index.html";
  String contentType = "text/html";
  if (path.endsWith(".css")) contentType = "text/css";
  else if (path.endsWith(".js")) contentType = "application/javascript";
  else if (path.endsWith(".json")) contentType = "application/json";
  else if (path.endsWith(".svg")) contentType = "image/svg+xml";
  else if (path.endsWith(".woff2")) contentType = "font/woff2";

  if (LittleFS.exists(path)) {
    File file = LittleFS.open(path, "r");
    server.streamFile(file, contentType);
    file.close();
  } else {
    server.send(404, "text/plain", "File Not Found");
  }
}

void handleRoot() {
  handleFileRead("/index.html");
}

void handleDataUpdate() {
  if (server.hasArg("plain")) {
    File file = LittleFS.open("/data.json", "w");
    if (!file) {
      server.send(500, "text/plain", "Failed to write");
      return;
    }
    file.print(server.arg("plain"));
    file.close();
    server.send(200, "text/plain", "Data saved");
  } else {
    server.send(400, "text/plain", "No data received");
  }
}

String estimateWeather(float temperature, float humidity, float pressure, float altitude) {
  if (isnan(temperature) || isnan(humidity) || isnan(pressure) || isnan(altitude)) {
    return "Sensor Error";
  }

  float adjustedPressure = pressure + (altitude / 8.5);

  if (adjustedPressure > 1025 && humidity < 50 && temperature >= 20) {
    return "Clear or Sunny";
  } else if (adjustedPressure > 1010 && humidity >= 50 && humidity < 80) {
    return "Partly Cloudy";
  } else if (adjustedPressure < 1000 && humidity > 85 && temperature > 10) {
    return "Rain Likely";
  } else if (adjustedPressure < 995 && humidity > 90 && temperature > 20) {
    return "Thunderstorm Possible";
  } else if (temperature < 10 && humidity > 85 && adjustedPressure < 1015) {
    return "Foggy or Misty";
  } else if (humidity < 30 && adjustedPressure > 1015) {
    return "Dry and Windy";
  } else if (temperature < 5 && adjustedPressure < 1010) {
    return "Possible Snow (High Altitude)";
  } else {
    return "Uncertain";
  }
}

void setup() {
  Serial.begin(115200);

  if (!LittleFS.begin()) {
    Serial.println("LittleFS Mount Failed");
    return;
  }

  bmeStatus = bme.begin(0x76);
  if (!bmeStatus) {
    bmeStatus = bme.begin(0x77);
  }

  if (!bmeStatus) {
    Serial.println("Could not find BME280 sensor!");
  }

  WiFi.softAP(ssid, password);
  Serial.println("AP Started at: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/data.json", HTTP_GET, []() {
    handleFileRead("/data.json");
  });
  server.on("/data.json", HTTP_POST, handleDataUpdate);
  server.serveStatic("/", LittleFS, "/");

  server.begin();
}

void loop() {
  server.handleClient();

  if (bmeStatus) {
    airTemp = bme.readTemperature();
    airPressure = bme.readPressure() / 100.0F;
    Altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
    airHumidity = bme.readHumidity();
    forecast = estimateWeather(airTemp, airHumidity, airPressure, Altitude);
  }

  if (LittleFS.exists("/data.json")) {
    File file = LittleFS.open("/data.json", "r");
    if (file) {
      StaticJsonDocument<1024> doc;
      DeserializationError error = deserializeJson(doc, file);
      file.close();

      if (!error) {
        doc["airTemp"] = String(airTemp, 1) + "°C";
        doc["airHumidity"] = String(airHumidity, 1) + "%";
        doc["airPressure"] = String(airPressure, 1) + " hPa";
        doc["altitude"] = String(Altitude, 1) + " m";
        doc["weather"] = forecast;
        doc["Temperature"] = String(airTemp, 1) + "°C";
        // String airTemp = String(random(23, 27)) + "°C";
        // doc["airTemp"] = airTemp;
        // doc["airHumidity"] = String(random(45, 64.78)) + "%";
        // doc["airPressure"] = String(random(974, 1033.5)) + " hPa";
        // doc["altitude"] = String(345.73) + " m";
        // doc["weather"] = "Partly Cloudy";
        // doc["Temperature"] = airTemp;
        float m1 = random(50, 100);
        float m2 = random(35, 75);
        float m3 =  random(23, 53);
        float m = (m1+m2+m3)/3;
        doc["moisture1"] = String(m1, 2);
        doc["moisture2"] = String(m2, 2);
        doc["moisture3"] = String(m3, 2);
        doc["moistureAvg"] = String(m, 2);
        doc["soilTemp"] = String(23.47);

        File writeFile = LittleFS.open("/data.json", "w");
        if (writeFile) {
          serializeJson(doc, writeFile);
          writeFile.close();
          Serial.println("Updated data.json with BME280 readings.");
        } else {
          Serial.println("Failed to open data.json for writing.");
        }

        Serial.println("=== Sensor Data ===");
        Serial.printf("Moisture 1   : %s\n", doc["moisture1"] | "N/A");
        Serial.printf("Moisture 2   : %s\n", doc["moisture2"] | "N/A");
        Serial.printf("Moisture 3   : %s\n", doc["moisture3"] | "N/A");
        Serial.printf("Avg Moisture : %s\n", doc["moistureAvg"] | "N/A");
        Serial.printf("Temperature  : %s\n", doc["temp"] | "N/A");
        Serial.printf("Soil Temp    : %s\n", doc["soilTemp"] | "N/A");
        Serial.printf("Weather      : %s\n", doc["weather"] | "N/A");
        Serial.printf("Air Temp     : %s\n", doc["airTemp"] | "N/A");
        Serial.printf("Air Humidity : %s\n", doc["airHumidity"] | "N/A");
        Serial.printf("Air Pressure : %s\n", doc["airPressure"] | "N/A");
        Serial.printf("Altitude     : %s\n", doc["altitude"] | "N/A");
        Serial.printf("Alerts       : %s\n", doc["alerts"] | "N/A");
        Serial.println("====================");

      } else {
        Serial.println("JSON parse error");
      }
    } else {
      Serial.println("Could not open data.json");
    }
  }

  delay(5000);
}
