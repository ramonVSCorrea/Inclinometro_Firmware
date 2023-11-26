#include "Variaveis_Globais.h"

void setup() {

  Serial.begin(115200);  //Inicia o monitor serial com 115200 baud rate
  Init_FILES();          //Inicia os arquivos de configuração do inclinômetro
  Init_MPU6050();        //Inicia o acelerômetro MPU6050
  Init_RTC();            //Inicia o relógio RTC

  // Tarefa que verifica os valores de inclinação
   xTaskCreate(Task_MPU6050, "Tarefa_MPU6050", 5120, NULL, configMAX_PRIORITIES - 2, NULL);

  // Tarefa que verifica condições de sinalização do inclinômetro
  xTaskCreate(Task_Sinalizacoes, "Tarefa_Sinalizacoes", 5120, NULL, configMAX_PRIORITIES - 3, NULL);

  // Tarefa que controla servo motor
  xTaskCreate(Task_Servo, "Tarefa_Servo", 5120, NULL, configMAX_PRIORITIES - 4, NULL);

  // Tarefa que verifica mensagens bluetooth
  xTaskCreate(Task_Bluetooth, "Tarefa_Bluetooth", 5120, NULL, configMAX_PRIORITIES - 1, NULL);

  // Tarefa que verifica o RTC
  xTaskCreate(Task_RTC, "Tarefa_RTC", 4096, NULL, configMAX_PRIORITIES - 5, NULL);
}

void loop() {
}
