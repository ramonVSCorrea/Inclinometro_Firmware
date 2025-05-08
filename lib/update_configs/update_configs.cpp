#include "update_configs.h"

void taskUpdateConfigs(void* pvParameters) {
  Serial.println("Iniciando tarefa de atualização de configurações...");

  // Envia o payload de configurações para o servidor
  sendMessageToServer(buildDeviceConfigurationsPayload());
  Serial.println("Payload de configurações enviado ao servidor.");

  const int CHECK_INTERVAL =
      30000;  // Intervalo de 30 segundos entre verificações

  while (1) {
    // Busca configurações atualizadas do servidor
    String metadataStr = retrieveLastConfigUpdate();

    if (metadataStr != "{}") {
      // Parse do JSON de resposta
      cJSON* metadata = cJSON_Parse(metadataStr.c_str());

      if (metadata != NULL) {
        bool configsChanged = false;
        bool wifiChanged = false;

        // Verifica configurações de inclinação
        cJSON* configurations = cJSON_GetObjectItem(metadata, "configurations");
        if (cJSON_IsObject(configurations)) {
          // Verifica ângulo lateral de bloqueio
          cJSON* blockLateralAngleJson =
              cJSON_GetObjectItem(configurations, "blockLateralAngle");
          if (cJSON_IsNumber(blockLateralAngleJson) &&
              blockLateralAngle != blockLateralAngleJson->valuedouble) {
            blockLateralAngle = blockLateralAngleJson->valuedouble;
            configsChanged = true;
            Serial.print("Novo ângulo lateral de bloqueio: ");
            Serial.println(blockLateralAngle);
          }

          // Verifica ângulo frontal de bloqueio
          cJSON* blockFrontalAngleJson =
              cJSON_GetObjectItem(configurations, "blockFrontalAngle");
          if (cJSON_IsNumber(blockFrontalAngleJson) &&
              blockFrontalAngle != blockFrontalAngleJson->valuedouble) {
            blockFrontalAngle = blockFrontalAngleJson->valuedouble;
            configsChanged = true;
            Serial.print("Novo ângulo frontal de bloqueio: ");
            Serial.println(blockFrontalAngle);
          }

          // Verifica configurações de WiFi
          cJSON* wifiConfigs = cJSON_GetObjectItem(metadata, "wifiConfigs");
          if (cJSON_IsObject(wifiConfigs)) {
            // Verifica SSID
            cJSON* ssidJson = cJSON_GetObjectItem(wifiConfigs, "SSID");
            if (cJSON_IsString(ssidJson) &&
                wifiSSID != String(ssidJson->valuestring)) {
              wifiSSID = ssidJson->valuestring;
              wifiChanged = true;
              Serial.print("Novo SSID: ");
              Serial.println(wifiSSID);
            }

            // Verifica senha
            cJSON* passwordJson = cJSON_GetObjectItem(wifiConfigs, "password");
            if (cJSON_IsString(passwordJson) &&
                wifiPassword != String(passwordJson->valuestring)) {
              wifiPassword = passwordJson->valuestring;
              wifiChanged = true;
              Serial.print("Nova senha WiFi configurada");
            }
          }

          // Salva as configurações se houve mudanças
          if (configsChanged) {
            Serial.println("Atualizando configurações...");
            setBlockConfigs();  // Atualiza os valores de bloqueio
          }

          if (wifiChanged) {
            Serial.println("Atualizando configurações WiFi...");
            setWiFiConfigs();  // Atualiza as configurações WiFi
          }

          // Se houve mudanças, notifica o servidor das configurações
          // atualizadas
          if (configsChanged || wifiChanged) {
            sendMessageToServer(buildDeviceConfigurationsPayload());
            Serial.println("Configurações atualizadas e enviadas ao servidor.");
          }

          cJSON_Delete(metadata);
        } else {
          Serial.println("Erro ao analisar JSON de configuração");
        }
      }

      // Aguarda intervalo entre verificações
      vTaskDelay(CHECK_INTERVAL / portTICK_PERIOD_MS);
    }
  }
}