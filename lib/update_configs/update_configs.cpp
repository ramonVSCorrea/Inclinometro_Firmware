#include "update_configs.h"
#include "debug_log.h"

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
      DBG_PRINT("Novo ângulo lateral de bloqueio: ");
      DBG_PRINTLN(blockLateralAngle);
    }

    cJSON* blockFrontalAngleJson =
        cJSON_GetObjectItem(angleConfigurations, "blockFrontalAngle");
    if (cJSON_IsNumber(blockFrontalAngleJson) &&
        blockFrontalAngle != blockFrontalAngleJson->valuedouble) {
      blockFrontalAngle = blockFrontalAngleJson->valuedouble;
      configsChanged = true;
      DBG_PRINT("Novo ângulo frontal de bloqueio: ");
      DBG_PRINTLN(blockFrontalAngle);
    }

    cJSON* calibrateLateralAngleJson =
        cJSON_GetObjectItem(angleConfigurations, "calibrateLateralAngle");
    if (cJSON_IsNumber(calibrateLateralAngleJson) &&
        calibrateLateralAngle != calibrateLateralAngleJson->valuedouble) {
      calibrateLateralAngle = calibrateLateralAngleJson->valuedouble;
      configsChanged = true;
      DBG_PRINT("Novo ângulo lateral de calibração: ");
      DBG_PRINTLN(calibrateLateralAngle);
    }

    cJSON* calibrateFrontalAngleJson =
        cJSON_GetObjectItem(angleConfigurations, "calibrateFrontalAngle");
    if (cJSON_IsNumber(calibrateFrontalAngleJson) &&
        calibrateFrontalAngle != calibrateFrontalAngleJson->valuedouble) {
      calibrateFrontalAngle = calibrateFrontalAngleJson->valuedouble;
      configsChanged = true;
      DBG_PRINT("Novo ângulo frontal de calibração: ");
      DBG_PRINTLN(calibrateFrontalAngle);
    }
  }

  cJSON* wifiConfigurations = cJSON_GetObjectItem(root, "wifiConfigurations");
  if (cJSON_IsObject(wifiConfigurations)) {
    cJSON* ssidJson = cJSON_GetObjectItem(wifiConfigurations, "ssid");
    if (cJSON_IsString(ssidJson) &&
        strcmp(wifiSSID, ssidJson->valuestring) != 0) {
      snprintf(wifiSSID, sizeof(wifiSSID), "%s", ssidJson->valuestring);
      wifiChanged = true;
      DBG_PRINT("Novo SSID: ");
      DBG_PRINTLN(wifiSSID);
    }

    cJSON* passwordJson = cJSON_GetObjectItem(wifiConfigurations, "password");
    if (cJSON_IsString(passwordJson) &&
        strcmp(wifiPassword, passwordJson->valuestring) != 0) {
      snprintf(wifiPassword, sizeof(wifiPassword), "%s",
               passwordJson->valuestring);
      wifiChanged = true;
      DBG_PRINT("Nova senha WiFi configurada");
    }
  }

  if (configsChanged) {
    DBG_PRINTLN("Atualizando configurações...");
    setBlockConfigs();
  }

  if (wifiChanged) {
    DBG_PRINTLN("Atualizando configurações WiFi...");
    setWiFiConfigs();
  }

  if (configsChanged || wifiChanged) {
    DBG_PRINTLN("Configurações atualizadas via MQTT.");
  }

  return configsChanged || wifiChanged;
}
