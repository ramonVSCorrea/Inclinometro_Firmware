/*
    A ideia é utilizar o bluetooth serial para o app se comunicar 
    com o ESP32.
    Quando o usuário se conectar no app, o ESP irá mandar informações via
    bluetooth com a seguinte mensagem:

      {
        "leituras": 
        {
          "anguloLateral": x,
          "AnguloFrontal": y
        }
      }

    Com essa mensagem o app irá interpretá-la e mostrar na tela principal
    os valores de inclinação atual. O app também mandará informações para o 
    ESP como por exemplo quando ele for alterar as configurações do inclinômetro

    {
      "configuracoesBLQ":
      {
        "bloqueioLateral": x,
        "bloqueioFrontal": y
      }
    }

    O ESP irá interpretar a mensagem alterar os valores de bloqueio do inclinômetro

*/

#include "BluetoothSerial.h"

#define LED_AMARELO 26   //LED que irá indicar conexão com celular

BluetoothSerial SerialBT;

/**
* @brief Tarefa Bluetooth
*/
void Task_Bluetooth(void* pvParameters){
  SerialBT.begin("Inclinometro");
  pinMode(LED_AMARELO, OUTPUT);
  
  while(1){

    /**
    * Verifica conexão do bluetooth, se estiver
    * conectado o LED amarelo irá ficar aceso.
    */
    if(SerialBT.connected()){
      digitalWrite(LED_AMARELO, HIGH);
    }
    else{
      digitalWrite(LED_AMARELO, LOW);
    }

    if(SerialBT.available()){
      Serial.write(SerialBT.read());
    }
    vTaskDelay(20 / portTICK_PERIOD_MS);
  }
}