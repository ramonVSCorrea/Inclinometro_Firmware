#include "gps.h"

TinyGPSPlus gps;
HardwareSerial gpsSerial(2);

void taskGPS(void* pvParameters) {
  Serial.println("Iniciando tarefa GPS");
  gpsSerial.begin(GPS_BAUDRATE, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
  Serial.println("GPS inicializado");

  while (1) {
    digitalWrite(YELLOW_LED_PIN, LOW);
    Serial.print("Lendo GPS:");
    Serial.println(gpsSerial.available());
    while (gpsSerial.available() > 0) {
      char c = gpsSerial.read();
      Serial.write(c);  // Imprime os dados brutos recebidos do GPS
      gps.encode(c);
    }

    if (gps.location.isUpdated()) {
      digitalWrite(YELLOW_LED_PIN, HIGH);
      Serial.print("Latitude: ");
      Serial.println(gps.location.lat(), 6);
      Serial.print("Longitude: ");
      Serial.println(gps.location.lng(), 6);
    } else {
      Serial.println("Aguardando fix do GPS...");
    }

    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
