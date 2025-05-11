#include <Arduino.h>
#include "accelerometer.h"
#include "bluetooth.h"
#include "configs_manager.h"
#include "global_variables.h"
#include "gps.h"
#include "http_communication.h"
#include "servo.h"
#include "signaling.h"
#include "update_configs.h"
#include "wifiConnection.h"

void setup() {
  Serial.begin(115200);

  initializeFiles();
  initializeAccelerometerMPU6050();

  // Tarefa de conexão Wi-Fi
  xTaskCreate(taskWiFiConnection, "Tarefa_WiFi", 8192, NULL,
              configMAX_PRIORITIES - 1, NULL);

  // Tarefa que verifica os valores de inclinação
  xTaskCreate(taskAccelerometerMPU6050, "taskAccelerometerMPU6050", 4096, NULL,
              configMAX_PRIORITIES - 2, NULL);

  // Tarefa que verifica condições de sinalização do inclinômetro
  xTaskCreate(taskSignals, "Tarefa_Sinalizacoes", 2048, NULL,
              configMAX_PRIORITIES - 3, NULL);

  // Tarefa que controla servo motor
  xTaskCreate(taskServoMotor, "Tarefa_Servo", 3072, NULL,
              configMAX_PRIORITIES - 4, NULL);

  // Tarefa que verifica mensagens bluetooth
  xTaskCreate(taskBluetooth, "Tarefa_Bluetooth", 4096, NULL,
              configMAX_PRIORITIES - 1, NULL);


  // Tarefa de GPS
  xTaskCreate(taskGPS, "Tarefa_GPS", 4096, NULL, configMAX_PRIORITIES - 5,
              NULL);

  // Tarefa de verificação de novas configurações
  xTaskCreate(taskUpdateConfigs, "Tarefa_Update_Configs", 4096, NULL,
              configMAX_PRIORITIES - 7, NULL);
}

void loop() {}