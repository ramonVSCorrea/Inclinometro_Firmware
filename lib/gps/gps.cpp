#include "gps.h"
#include "debug_log.h"
#include "stack_debug.h"

// #define DBG_GPS

TinyGPSPlus gps;

void taskGPS(void* pvParameters) {
  unsigned long stackLogTimer = 0;

  DBG_PRINTLN("Iniciando tarefa GPS");
  Serial2.begin(GPS_BAUDRATE, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);

  while (1) {
    logTaskStackHighWaterMark("Tarefa_GPS", &stackLogTimer);

#ifdef DBG_GPS
    DBG_PRINT("Lendo GPS:");
    DBG_PRINTLN(Serial2.available());
#endif

    while (Serial2.available() > 0) {
      char c = Serial2.read();
      gps.encode(c);
    }

    if (gps.location.isUpdated()) {
      latitude = gps.location.lat();
      longitude = gps.location.lng();
    }
#ifdef DBG_GPS
    else {
      DBG_PRINTLN("Aguardando fix do GPS...");
    }
#endif

    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
