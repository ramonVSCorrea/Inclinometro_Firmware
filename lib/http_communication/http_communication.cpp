#include "http_communication.h"

void taskHttpCommunication(void* pvParameters) {
  while (true) {
    sendDataInclinationToServer();
    vTaskDelay(5000 / portTICK_PERIOD_MS);
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
