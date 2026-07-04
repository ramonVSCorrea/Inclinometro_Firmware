#include "accelerometer.h"
#include <MPU6050_light.h>
#include <Wire.h>
#include "debug_log.h"
#include "global_variables.h"
#include "stack_debug.h"

// #define DBG_MSG_MPU6050

MPU6050 mpu(Wire);

namespace {
char inclinationPayload[INCLINATION_MESSAGE_BUFFER_SIZE];
}

void initializeAccelerometerMPU6050() {
  Wire.begin();

  // Verifique se o MPU6050 está respondendo no endereço 0x68 ou 0x69
  Wire.beginTransmission(0x68);
  if (Wire.endTransmission() == 0) {
    DBG_PRINTLN("MPU6050 encontrado no endereço 0x68");
    mpu.setAddress(0x68);
  } else {
    Wire.beginTransmission(0x69);
    if (Wire.endTransmission() == 0) {
      DBG_PRINTLN("MPU6050 encontrado no endereço 0x69");
      mpu.setAddress(0x69);
    } else {
      DBG_PRINTLN("MPU6050 não encontrado");
      DBG_PRINTLN("Reiniciando o ESP...");
      delay(1000);
      ESP.restart();
    }
  }

  byte status = mpu.begin();
  DBG_PRINT(F("MPU6050 status: "));
  DBG_PRINTLN(status);
  while (status != 0) {
  }

  DBG_PRINTLN("Offsets calculados!\n");

  mpu.upsideDownMounting = false;
}

void taskAccelerometerMPU6050(void* parameter) {
  unsigned long timer = 0;
  unsigned long stackLogTimer = 0;

  while (1) {
    logTaskStackHighWaterMark("taskAccelerometerMPU6050", &stackLogTimer);

    // xSemaphoreTake(i2cMutex, portMAX_DELAY);

    mpu.update();
    lateralAngle = mpu.getAngleY() + (calibrateLateralAngle * (-1));
    frontalAngle = mpu.getAngleX() + (calibrateFrontalAngle * (-1));

    if ((millis() - timer) > 10000) {
#ifdef DBG_MSG_MPU6050
      DBG_PRINT("Lateral : ");
      DBG_PRINT(lateralAngle);
      DBG_PRINT("\tFrontal : ");
      DBG_PRINTLN(frontalAngle);
#endif

      if (isMqttReady() &&
          buildInclinationDataMessage(inclinationPayload,
                                      sizeof(inclinationPayload))) {
        publishMessageToMqtt(MQTT_DATA_TOPIC, inclinationPayload);
      }

      timer = millis();
    }

    // xSemaphoreGive(i2cMutex);
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}
