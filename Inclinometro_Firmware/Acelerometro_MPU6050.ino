#include <MPU6050_light.h>
#include <Wire.h>

//#define DBG_MSG_MPU6050

MPU6050 mpu(Wire);

/**
* @brief Função que inicia o acelerômetro MPU6050
*/
void Init_MPU6050() {
  Wire.begin();

  byte status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);
  while (status != 0) {}

  Serial.println("Offsets calculados!\n");
}


/**
* @brief Tarefa que pega os valores de angulo do acelerômetro
*/
void Task_MPU6050(void* pvParameters) {
  unsigned long timer = 0;

  while (1) {
    mpu.update();
    AnguloLateral = mpu.getAngleY() + (Angulo_Calib_Lat*(-1));
    AnguloFrontal = mpu.getAngleX() + (Angulo_Calib_Front*(-1));

#ifdef DBG_MSG_MPU6050
    if ((millis() - timer) > 1000) {  // print data every 10ms
      Serial.print("Lateral : ");
      Serial.print(AnguloLateral);
      Serial.print("\tFrontal : ");
      Serial.println(AnguloFrontal);
      timer = millis();
    }
#endif
    //Add_Event();
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}
