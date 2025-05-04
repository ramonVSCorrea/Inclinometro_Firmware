#include "bluetooth.h"

bool evtConnected = false;
BluetoothSerial SerialBT;

void taskBluetooth(void* pvParameters) {
  SerialBT.begin("IncliMax - 1234");
  pinMode(YELLOW_LED_PIN, OUTPUT);

  while (1) {
    /**
     * Verifica conexão do bluetooth, se estiver
     * conectado o LED amarelo irá ficar aceso.
     */
    if (SerialBT.connected()) {
      digitalWrite(YELLOW_LED_PIN, HIGH);
      if (!evtConnected) {
        sendMessageToServer(buildEventPayload(
            EVENT_SENSOR_CONNECTED, EVENT_SENSOR_CONNECTED_DESCRIPTION));
        evtConnected = true;
      }
      readBluetoothData();
    } else {
      digitalWrite(YELLOW_LED_PIN, LOW);
      evtConnected = false;
    }

    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void sendBluetoothReadings() {
  String msgBT = "{\"leituras\":{\"anguloLateral\":" + String(lateralAngle) +
                 ",\"anguloFrontal\":" + String(frontalAngle) + "}}";
  SerialBT.println(msgBT);
}

void readBluetoothData() {
  if (SerialBT.available()) {
    String msgBT = SerialBT.readStringUntil('\n');

#ifdef DBG_MSG_BLUE
    Serial.println("Mensagem recebida: " + msgBT);
#endif

    if (msgBT == "{\"requisicaoLeitura\": 1}\n") {
      sendBluetoothReadings();
    } else {
      cJSON* root = cJSON_Parse(msgBT.c_str());

      if (cJSON_IsObject(root)) {
        cJSON* configuracoesBLQ = cJSON_GetObjectItem(root, CONFIG_BLQ);
        cJSON* comandoBascula = cJSON_GetObjectItem(root, CMD_BASC);
        cJSON* requisitaLeitura = cJSON_GetObjectItem(root, CMD_LEITURA);
        cJSON* requisitaConfig = cJSON_GetObjectItem(root, CMD_LER_CFG);
        cJSON* configCalib = cJSON_GetObjectItem(root, CONFIG_CALIB);
        cJSON* totalEventos = cJSON_GetObjectItem(root, TOTAL_EVENTOS);
        cJSON* lerEventoJSON = cJSON_GetObjectItem(root, LER_EVENTOS);
        cJSON* alteraData = cJSON_GetObjectItem(root, CMD_ALTERA_DATA);

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
            sendBluetoothReadings();
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
            File fp = SPIFFS.open(FILE_CONFIG);
            String conteudo = fp.readString();
            fp.close();

#ifdef DBG_MSG_BLUE
            Serial.println(conteudo);
#endif

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
          blockLateralAngle = cJSON_GetNumberValue(node);

          node = cJSON_GetObjectItem(configuracoesBLQ, JSON_BLQ_FRT);
          blockFrontalAngle = cJSON_GetNumberValue(node);
          setBlockConfigs();

          sendMessageToServer(
              buildEventPayload(EVENT_BLOCK_VALUE_CHANGED,
                                EVENT_BLOCK_VALUE_CHANGED_DESCRIPTION));
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
          commandRaise = cJSON_GetNumberValue(node);
          node = cJSON_GetObjectItem(comandoBascula, JSON_DESCER);
          commandLower = cJSON_GetNumberValue(node);
          String resposta = "{\"bascula\":1}";
          SerialBT.println(resposta);
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
        else if (configCalib) {
          if (cJSON_GetNumberValue(configCalib) == 1) {
            calibrateLateralAngle += lateralAngle;
            calibrateFrontalAngle += frontalAngle;
            setCalibrationConfigs();
            sendMessageToServer(buildEventPayload(
                EVENT_SENSOR_CALIBRATED, EVENT_SENSOR_CALIBRATED_DESCRIPTION));
          } else if (cJSON_GetNumberValue(configCalib) == 0) {
            calibrateLateralAngle = 0;
            calibrateFrontalAngle = 0;
            setCalibrationConfigs();
            sendMessageToServer(buildEventPayload(
                EVENT_SENSOR_CLEARED, EVENT_SENSOR_CLEARED_DESCRIPTION));
          }
        }

        while (SerialBT.available()) {
          SerialBT.read();
        }
      }
      cJSON_Delete(root);
    }
  }
}
