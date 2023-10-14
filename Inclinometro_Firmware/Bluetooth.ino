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
#include <stdlib.h>
#include "cJSON.h"

#define LED_AMARELO 26  //LED que irá indicar conexão com celular

#define CONFIG_BLQ "configuracoesBLQ"
#define JSON_BLQ_LAT "bloqueioLateral"
#define JSON_BLQ_FRT "bloqueioFrontal"

BluetoothSerial SerialBT;

/**
* @brief Tarefa Bluetooth
*/
void Task_Bluetooth(void* pvParameters) {
  SerialBT.begin("Inclinometro");
  pinMode(LED_AMARELO, OUTPUT);

  while (1) {

    /**
    * Verifica conexão do bluetooth, se estiver
    * conectado o LED amarelo irá ficar aceso.
    */
    if (SerialBT.connected()) {
      digitalWrite(LED_AMARELO, HIGH);
      BT_Write_Leituras();
      BT_Read_Info();
    } else {
      digitalWrite(LED_AMARELO, LOW);
    }

    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

/**
* @brief Função que envia valores de leitura de angulo para o app
*/
void BT_Write_Leituras() {
  String msgBT = "{\"leituras\":{\"anguloLateral\":" + String(AnguloLateral) + ",\"anguloFrontal\":" + String(AnguloFrontal) + "}}";
  SerialBT.println(msgBT);
}

/**
* @brief Função que lê informações recebidas via bluetooth
*/
void BT_Read_Info(){
  if(SerialBT.available()){

    String msgBT = SerialBT.readString();
    Serial.println("Mensagem recebida: " + msgBT);
    
    cJSON* root = cJSON_Parse(msgBT.c_str());

    if(cJSON_IsObject(root)){
      cJSON* configuracoesBLQ = cJSON_GetObjectItem(root, CONFIG_BLQ);

      if(configuracoesBLQ){
        //Serial.println("Configurações de bloqueio");
        cJSON* node = cJSON_GetObjectItem(configuracoesBLQ, JSON_BLQ_LAT);
        Angulo_BLQ_Lat = cJSON_GetNumberValue(node);

        node = cJSON_GetObjectItem(configuracoesBLQ, JSON_BLQ_FRT);
        Angulo_BLQ_Front = cJSON_GetNumberValue(node);
      }
    }
    Set_BLQ_Configs();
    cJSON_Delete(root);
  }
}




