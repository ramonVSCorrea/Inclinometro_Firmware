#include "update_configs.h"

bool applyConfigUpdateJson(cJSON* root) {
  if (root == NULL) {
    return false;
  }

  bool configsChanged = false;
  bool wifiChanged = false;

  cJSON* angleConfigurations = cJSON_GetObjectItem(root, "angleConfigurations");
  if (cJSON_IsObject(angleConfigurations)) {
    cJSON* blockLateralAngleJson =
        cJSON_GetObjectItem(angleConfigurations, "blockLateralAngle");
    if (cJSON_IsNumber(blockLateralAngleJson) &&
        blockLateralAngle != blockLateralAngleJson->valuedouble) {
      blockLateralAngle = blockLateralAngleJson->valuedouble;
      configsChanged = true;
      Serial.print("Novo ângulo lateral de bloqueio: ");
      Serial.println(blockLateralAngle);
    }

    cJSON* blockFrontalAngleJson =
        cJSON_GetObjectItem(angleConfigurations, "blockFrontalAngle");
    if (cJSON_IsNumber(blockFrontalAngleJson) &&
        blockFrontalAngle != blockFrontalAngleJson->valuedouble) {
      blockFrontalAngle = blockFrontalAngleJson->valuedouble;
      configsChanged = true;
      Serial.print("Novo ângulo frontal de bloqueio: ");
      Serial.println(blockFrontalAngle);
    }

    cJSON* calibrateLateralAngleJson =
        cJSON_GetObjectItem(angleConfigurations, "calibrateLateralAngle");
    if (cJSON_IsNumber(calibrateLateralAngleJson) &&
        calibrateLateralAngle != calibrateLateralAngleJson->valuedouble) {
      calibrateLateralAngle = calibrateLateralAngleJson->valuedouble;
      configsChanged = true;
      Serial.print("Novo ângulo lateral de calibração: ");
      Serial.println(calibrateLateralAngle);
    }

    cJSON* calibrateFrontalAngleJson =
        cJSON_GetObjectItem(angleConfigurations, "calibrateFrontalAngle");
    if (cJSON_IsNumber(calibrateFrontalAngleJson) &&
        calibrateFrontalAngle != calibrateFrontalAngleJson->valuedouble) {
      calibrateFrontalAngle = calibrateFrontalAngleJson->valuedouble;
      configsChanged = true;
      Serial.print("Novo ângulo frontal de calibração: ");
      Serial.println(calibrateFrontalAngle);
    }
  }

  cJSON* wifiConfigurations = cJSON_GetObjectItem(root, "wifiConfigurations");
  if (cJSON_IsObject(wifiConfigurations)) {
    cJSON* ssidJson = cJSON_GetObjectItem(wifiConfigurations, "ssid");
    if (cJSON_IsString(ssidJson) && wifiSSID != String(ssidJson->valuestring)) {
      wifiSSID = ssidJson->valuestring;
      wifiChanged = true;
      Serial.print("Novo SSID: ");
      Serial.println(wifiSSID);
    }

    cJSON* passwordJson = cJSON_GetObjectItem(wifiConfigurations, "password");
    if (cJSON_IsString(passwordJson) &&
        wifiPassword != String(passwordJson->valuestring)) {
      wifiPassword = passwordJson->valuestring;
      wifiChanged = true;
      Serial.print("Nova senha WiFi configurada");
    }
  }

  if (configsChanged) {
    Serial.println("Atualizando configurações...");
    setBlockConfigs();
  }

  if (wifiChanged) {
    Serial.println("Atualizando configurações WiFi...");
    setWiFiConfigs();
  }

  if (configsChanged || wifiChanged) {
    sendMessageToServer(buildDeviceConfigurationsPayload());
    Serial.println("Configurações atualizadas e enviadas ao servidor.");
  }

  return configsChanged || wifiChanged;
}

void updateConfigs() {
  Serial.println("Iniciando atualização de configurações...");

  // Envia o payload de configurações para o servidor
  sendMessageToServer(buildDeviceConfigurationsPayload());
  Serial.println("Payload de configurações enviado ao servidor.");

  // Busca configurações atualizadas do servidor
  String metadataStr = retrieveLastConfigUpdate();

  if (metadataStr != "{}") {
    // Parse do JSON de resposta
    cJSON* metadata = cJSON_Parse(metadataStr.c_str());

    if (metadata != NULL) {
      applyConfigUpdateJson(metadata);

      cJSON_Delete(metadata);
    } else {
      Serial.println("Erro ao analisar JSON de configuração");
    }
  }
}
