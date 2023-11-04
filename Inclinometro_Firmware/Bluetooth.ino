#include "BluetoothSerial.h"
#include <stdlib.h>
#include <ArduinoJson.h>
#include "cJSON.h"
#include <FS.h>
#include <SPIFFS.h>


#define LED_AMARELO 26  //LED que irá indicar conexão com celular

#define CONFIG_BLQ "configuracoesBLQ"
#define CONFIG_CALIB "configuraCalib"
#define JSON_BLQ_LAT "bloqueioLateral"
#define JSON_BLQ_FRT "bloqueioFrontal"

#define CMD_BASC "comandoBascula"
#define CMD_LEITURA "requisicaoLeitura"
#define CMD_LER_CFG "requisitaCfg"
#define JSON_SUBIR "subir"
#define JSON_DESCER "descer"

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
void BT_Read_Info() {
  if (SerialBT.available()) {

    String msgBT = SerialBT.readString();
    Serial.println("Mensagem recebida: " + msgBT);

    cJSON* root = cJSON_Parse(msgBT.c_str());

    if (cJSON_IsObject(root)) {
      cJSON* configuracoesBLQ = cJSON_GetObjectItem(root, CONFIG_BLQ);
      cJSON* comandoBascula = cJSON_GetObjectItem(root, CMD_BASC);
      cJSON* requisitaLeitura = cJSON_GetObjectItem(root, CMD_LEITURA);
      cJSON* requisitaConfig = cJSON_GetObjectItem(root, CMD_LER_CFG);
      cJSON* configCalib = cJSON_GetObjectItem(root, CONFIG_CALIB);

      /**
          Se recebeu uma mensagem de requisicao de leitura
          chama a função de envio de mensagem de leitura.
          
            --- Mensagem Recebida:
          
          {
            "requisicaoLeitura": 1
          }
          
            --- Mensagem Enviada: 
          
          {
            "leituras": 
            {
              "anguloLateral": x,
              "anguloFrontal": y
            }
          }
      */
      if (requisitaLeitura) {
        if (cJSON_GetNumberValue(requisitaLeitura) == 1)
          BT_Write_Leituras();
      }

      /*
          Se recebeu requisição de configuração, envia
          os dados de configuração para o App.
          
            --- Mensagem Recebida ---
          
          {
            "requisitaCfg": 1 
          }
          
          --- Mensagem Enviada ---
          
          {
            "configuracoes":	{
              "bloqueioLateral":	3.5,
              "bloqueioFrontal":	5,
              "calibracaoLateral":	0,
              "calibracaoFrontal":	0
            }
          }
      */
      else if (requisitaConfig) {
        if (cJSON_GetNumberValue(requisitaConfig) == 1) {
          File fp = SPIFFS.open(FILE_CFG);
          String conteudo = fp.readString();
          fp.close();
          Serial.println(conteudo);
          SerialBT.println(conteudo);
        }
      }

      /*
        Se recebeu mensagem de ajuste de bloqueio
        grava os valores de bloqueio no arquivo de 
        configurações.
        
        --- Mensagem Recebida ---
        
            {
              "configuracoesBLQ":
              {
                "bloqueioLateral": x,
                "bloqueioFrontal": y
              }
            }
      */
      else if (configuracoesBLQ) {
        cJSON* node = cJSON_GetObjectItem(configuracoesBLQ, JSON_BLQ_LAT);
        Angulo_BLQ_Lat = cJSON_GetNumberValue(node);

        node = cJSON_GetObjectItem(configuracoesBLQ, JSON_BLQ_FRT);
        Angulo_BLQ_Front = cJSON_GetNumberValue(node);
        Set_BLQ_Configs();
      }

      /*
        Se recebeu comando de bascula, verifica se é para subir ou descer 
        e movimenta a bascula.

        --- Mensagem Recebida ---

        {
          "comandoBascula":
          {
            "subir": 1,
            "descer": 0
          }
        }
      */
      else if (comandoBascula) {
        cJSON* node = cJSON_GetObjectItem(comandoBascula, JSON_SUBIR);
        cmdSubir = cJSON_GetNumberValue(node);
        node = cJSON_GetObjectItem(comandoBascula, JSON_DESCER);
        cmdDescer = cJSON_GetNumberValue(node);
      }

      /*
          Se recebeu mensagem de calibração do sensor
          faz a calibração e salva no arquivo.

          --- Mensagem de Calibração ---

          {
            "configCalib": 1
          }

          --- Mensagem de Zeragem ---

          {
            "configCalib": 0
          }
      */
      else if(configCalib){
        if (cJSON_GetNumberValue(configCalib) == 1){
          Angulo_Calib_Lat += AnguloLateral;
          Angulo_Calib_Front += AnguloFrontal;
          Set_Calib_Configs();
        } else if(cJSON_GetNumberValue(configCalib) == 0){
          Angulo_Calib_Lat = 0;
          Angulo_Calib_Front = 0;
          Set_Calib_Configs();
        }
      }

      while (SerialBT.available()) {
        SerialBT.read();
      }
    }
    cJSON_Delete(root);
  }
}
