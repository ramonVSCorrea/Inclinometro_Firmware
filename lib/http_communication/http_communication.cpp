#include "http_communication.h"

String buildInclinationDataPayload() {
  return "{"
         "\"variable\":\"inclinationData\","
         "\"value\":\"inclinationData\","
         "\"metadata\":{"
         "\"lateralAngle\":" +
         String(lateralAngle) +
         ","
         "\"frontalAngle\":" +
         String(frontalAngle) +
         "},"
         "\"group\":\"" +
         id +
         "\""
         "}";
}

String buildEventPayload(String event, String description) {
  return "{"
         "\"variable\":\"event\","
         "\"value\":\"event\","
         "\"metadata\":{"
         "\"event\":\"" +
         event +
         "\","
         "\"description\":\"" +
         description +
         "\","
         "\"lateralAngle\":" +
         String(lateralAngle) +
         ","
         "\"frontalAngle\":" +
         String(frontalAngle) +
         "},"
         "\"group\":\"" +
         id +
         "\""
         "}";
}

void sendMessageToServer(String payload) {
  if (isWiFiConnected) {
    HTTPClient http;
    http.begin(SERVER_URL);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Device-Token", DEVICE_TOKEN);

    int httpResponseCode = http.POST(payload);
    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.print(httpResponseCode);
      Serial.print(" - Body: ");
      Serial.println(payload);
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
  } else {
    Serial.println("WiFi not connected");
  }
}

void sendDataInclinationToServer() {
  if (isWiFiConnected) {
    HTTPClient http;
    http.begin(SERVER_URL);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Device-Token", DEVICE_TOKEN);

    String payload = "{";
    payload += "\"variable\":\"id\",";
    payload += "\"value\": \"AAAA\",";
    payload += "\"metadata\":{";
    payload += "\"anguloLateral\":" + String(lateralAngle) + ",";
    payload += "\"anguloFrontal\":" + String(frontalAngle);
    payload += "}";
    payload += "}";

    int httpResponseCode = http.POST(payload);
    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
  } else {
    Serial.println("WiFi not connected");
  }
}
