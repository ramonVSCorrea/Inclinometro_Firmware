#include "Variaveis_Globais.h"

void setup() {

  Serial.begin(115200);  //Inicia o monitor serial com 115200 baud rate
  Init_FILES();          //Inicia os arquivos de configuração do inclinômetro
  Init_MPU6050();        //Inicia o acelerômetro MPU6050

  // Tarefa que verifica os valores de inclinação
  xTaskCreate(Task_MPU6050, "Tarefa_MPU6050", 2048, NULL, 1, NULL);

  // Tarefa que verifica condições de sinalização do inclinômetro
  xTaskCreate(Task_Sinalizacoes, "Tarefa_Sinalizacoes", 2048, NULL, 1, NULL);

  // Tarefa que controla servo motor
  xTaskCreate(Task_Servo, "Tarefa_Servo", 2048, NULL, 1, NULL);

  // Tarefa que verifica mensagens bluetooth
  xTaskCreate(Task_Bluetooth, "Tarefa_Bluetooth", 2048, NULL, 1, NULL);
}

void loop() {
}
