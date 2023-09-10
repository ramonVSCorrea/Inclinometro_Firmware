#include <MPU6050_light.h>
#include <Wire.h>

MPU6050 mpu(Wire);

/**
* @brief Função que inica o acelerômetro MPU6050
*/
void Init_MPU6050() {
  Wire.begin();

  byte status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);
  while (status != 0) {}

  Serial.println(F("Calculando Offsets, não movimento o sensor!"));
  delay(1000);
  mpu.upsideDownMounting = false;
  mpu.calcOffsets();
  Serial.println("Offsets calculados!\n");
}


/**
* @brief Tarefa que pega os valores de angulo do acelerômetro
*/
void Task_MPU6050(void* pvParameters) {
  unsigned long timer = 0;

  while (1) {
    mpu.update();

    if ((millis() - timer) > 1000) {  // print data every 10ms
      AnguloLateral = mpu.getAngleY();
      AnguloFrontal = mpu.getAngleX();

      Serial.print("Lateral : ");
      Serial.print(AnguloLateral);
      Serial.print("\tFrontal : ");
      Serial.println(AnguloFrontal);

      timer = millis();
    }

    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}
