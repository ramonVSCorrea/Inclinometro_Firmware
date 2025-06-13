#include "accelerometer.h"
#include <MPU6050_light.h>
#include <Wire.h>
#include "global_variables.h"

// #define DBG_MSG_MPU6050

MPU6050 mpu(Wire);

void initializeAccelerometerMPU6050() {
  Wire.begin();

  // Verifique se o MPU6050 está respondendo no endereço 0x68 ou 0x69
  Wire.beginTransmission(0x68);
  if (Wire.endTransmission() == 0) {
    Serial.println("MPU6050 encontrado no endereço 0x68");
    mpu.setAddress(0x68);
  } else {
    Wire.beginTransmission(0x69);
    if (Wire.endTransmission() == 0) {
      Serial.println("MPU6050 encontrado no endereço 0x69");
      mpu.setAddress(0x69);
    } else {
      Serial.println("MPU6050 não encontrado");
      while (1)
        ;  // Pare o programa se o MPU6050 não for encontrado
    }
  }

  byte status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);
  while (status != 0) {
  }

  Serial.println("Offsets calculados!\n");

  mpu.upsideDownMounting = true;
}

void taskAccelerometerMPU6050(void* parameter) {
  unsigned long timer = 0;

  while (1) {
    // xSemaphoreTake(i2cMutex, portMAX_DELAY);

    mpu.update();
    lateralAngle = mpu.getAngleY() + (calibrateLateralAngle * (-1));
    frontalAngle = mpu.getAngleX() + (calibrateFrontalAngle * (-1));

    if ((millis() - timer) > 10000) {
#ifdef DBG_MSG_MPU6050
      Serial.print("Lateral : ");
      Serial.print(lateralAngle);
      Serial.print("\tFrontal : ");
      Serial.println(frontalAngle);
#endif
      while (isHttpRequest) {
        vTaskDelay(100 / portTICK_PERIOD_MS);
      }
      sendMessageToServer(buildInclinationDataPayload());
      timer = millis();
    }

    // xSemaphoreGive(i2cMutex);
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}