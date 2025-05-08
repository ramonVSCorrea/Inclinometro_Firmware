#include "configs_manager.h"

void initializeFiles() {
  SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED);  // Inicia SPIFFS e formata caso falhe
  Serial.println("SPIFFS inicializado!");
  SPIFFS.remove(FILE_CONFIG);

  // Criação dos arquivos caso não existam
  if (!SPIFFS.exists(FILE_CONFIG)) {
    if (createFileConfig()) {
      Serial.println("Arquivo de configurações criado com sucesso!");
    } else {
      Serial.println("Erro ao criar arquivo de configurações!");
    }
  }

  initializeConfigs();  // Inicia as variáveis globais
}

bool createFileConfig() {
  File fp = SPIFFS.open(FILE_CONFIG, FILE_WRITE);

  if (!fp) {
    Serial.println("Falha ao abrir arquivo");
    return false;
  }

  // Criação do objeto JSON
  cJSON* root = cJSON_CreateObject();
  cJSON* configs = cJSON_CreateObject();

  cJSON_AddNumberToObject(configs, "blockLateralAngle", 3.5);
  cJSON_AddNumberToObject(configs, "blockFrontalAngle", 5);
  cJSON_AddNumberToObject(configs, "calibrateLateralAngle", 0);
  cJSON_AddNumberToObject(configs, "calibrateFrontalAngle", 0);

  cJSON_AddItemToObject(root, "configurations", configs);

  cJSON* wifiConfigs = cJSON_CreateObject();

  cJSON_AddStringToObject(wifiConfigs, "SSID", wifiSSID.c_str());
  cJSON_AddStringToObject(wifiConfigs, "password", wifiPassword.c_str());

  cJSON_AddItemToObject(root, "wifiConfigs", wifiConfigs);

  if (fp.print(cJSON_Print(root))) {
    Serial.println("Arquivo Criado!");
  } else {
    Serial.println("Erro ao gravar arquivo");
    cJSON_Delete(root);
    fp.close();
    return false;
  }

  cJSON_Delete(root);
  fp.close();

#ifdef DEBUG_FILE
  fp = SPIFFS.open(FILE_CONFIG);

  while (fp.available()) {
    Serial.write(fp.read());
  }

  fp.close();
#endif

  return true;
}

void initializeConfigs() {
  File fp = SPIFFS.open(FILE_CONFIG);
  String content = fp.readString();
  const char* fileContent = content.c_str();
  fp.close();

  cJSON* root = cJSON_Parse(fileContent);

  if (cJSON_IsObject(root)) {
    cJSON* configs = cJSON_GetObjectItem(root, "configurations");

    cJSON* node = cJSON_GetObjectItem(configs, "blockLateralAngle");
    blockLateralAngle = cJSON_GetNumberValue(node);

    node = cJSON_GetObjectItem(configs, "blockFrontalAngle");
    blockFrontalAngle = cJSON_GetNumberValue(node);

    node = cJSON_GetObjectItem(configs, "calibrateLateralAngle");
    calibrateLateralAngle = cJSON_GetNumberValue(node);

    node = cJSON_GetObjectItem(configs, "calibrateFrontalAngle");
    calibrateFrontalAngle = cJSON_GetNumberValue(node);
  }

  cJSON_Delete(root);
}

void setBlockConfigs() {
  File fp = SPIFFS.open(FILE_CONFIG, "r");
  String content = fp.readString();
  const char* fileContent = content.c_str();
  fp.close();

  fp = SPIFFS.open(FILE_CONFIG, "w");
  cJSON* root = cJSON_Parse(fileContent);

  if (cJSON_IsObject(root)) {
    cJSON* configs = cJSON_GetObjectItem(root, "configurations");

    if (cJSON_IsObject(configs)) {
      cJSON* node = cJSON_GetObjectItem(configs, "blockLateralAngle");
      cJSON_SetNumberValue(node, blockLateralAngle);

      node = cJSON_GetObjectItem(configs, "blockFrontalAngle");
      cJSON_SetNumberValue(node, blockFrontalAngle);
    }
  }

  if (fp.print(cJSON_Print(root))) {
    Serial.println("Arquivo Alterado!");
  } else {
    Serial.println("Erro ao alterar arquivo");
  }

  cJSON_Delete(root);
  fp.close();
  initializeConfigs();
}

void setCalibrationConfigs() {
  File fp = SPIFFS.open(FILE_CONFIG, "r");
  String content = fp.readString();
  const char* fileContent = content.c_str();
  fp.close();

  fp = SPIFFS.open(FILE_CONFIG, "w");
  cJSON* root = cJSON_Parse(fileContent);

  if (cJSON_IsObject(root)) {
    cJSON* configs = cJSON_GetObjectItem(root, "configurations");

    if (cJSON_IsObject(configs)) {
      cJSON* node = cJSON_GetObjectItem(configs, "calibrateLateralAngle");
      cJSON_SetNumberValue(node, calibrateLateralAngle);

      node = cJSON_GetObjectItem(configs, "calibrateFrontalAngle");
      cJSON_SetNumberValue(node, calibrateFrontalAngle);
    }
  }

  if (fp.print(cJSON_Print(root))) {
    Serial.println("Arquivo Alterado!");
    Serial.println(fp.readString());
  } else {
    Serial.println("Erro ao alterar arquivo");
  }

  cJSON_Delete(root);
  fp.close();
  initializeConfigs();
}

void setWiFiConfigs() {
  File fp = SPIFFS.open(FILE_CONFIG, "r");
  String content = fp.readString();
  const char* fileContent = content.c_str();
  fp.close();

  fp = SPIFFS.open(FILE_CONFIG, "w");
  cJSON* root = cJSON_Parse(fileContent);

  if (cJSON_IsObject(root)) {
    cJSON* wifiConfigs = cJSON_GetObjectItem(root, "wifiConfigs");

    if (cJSON_IsObject(wifiConfigs)) {
      cJSON* node = cJSON_GetObjectItem(wifiConfigs, "SSID");
      cJSON_SetValuestring(node, wifiSSID.c_str());

      node = cJSON_GetObjectItem(wifiConfigs, "password");
      cJSON_SetValuestring(node, wifiPassword.c_str());
    }
  }

  if (fp.print(cJSON_Print(root))) {
    Serial.println("Configurações WiFi alteradas!");
    Serial.println(fp.readString());

    isWiFiConfigChanged =
        true;  // Sinaliza que as configurações foram alteradas
  } else {
    Serial.println("Erro ao alterar configurações WiFi");
  }

  cJSON_Delete(root);
  fp.close();
  initializeConfigs();
}