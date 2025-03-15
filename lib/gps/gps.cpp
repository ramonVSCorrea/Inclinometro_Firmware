#include "gps.h"

TinyGPSPlus gps;

void taskGPS(void* pvParameters) {
  Serial.println("Iniciando tarefa GPS");
  Serial2.begin(GPS_BAUDRATE, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);

  while (1) {
    Serial.print("Lendo GPS:");
    Serial.println(Serial2.available());

    while (Serial2.available() > 0) {
      char c = Serial2.read();
      gps.encode(c);
    }

    if (gps.location.isUpdated()) {
      latitude = gps.location.lat();
      longitude = gps.location.lng();
    } else {
      Serial.println("Aguardando fix do GPS...");
    }

    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
