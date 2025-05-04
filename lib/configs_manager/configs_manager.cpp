#include "configs_manager.h"

void initializeFiles() {
  SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED);  // Inicia SPIFFS e formata caso falhe
  Serial.println("SPIFFS inicializado!");
  // SPIFFS.remove(FILE_CONFIG);
  // SPIFFS.remove(FILE_EVENTS);

  // Criação dos arquivos caso não existam
  if (!SPIFFS.exists(FILE_CONFIG)) {
    if (createFileConfig()) {
      Serial.println("Arquivo de configurações criado com sucesso!");
    } else {
      Serial.println("Erro ao criar arquivo de configurações!");
    }
  }

  if (!SPIFFS.exists(FILE_EVENTS)) {
    if (createFileEvents()) {
      Serial.println("Arquivo de eventos criado com sucesso!");
    } else {
      Serial.println("Erro ao criar arquivo de eventos!");
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

bool createFileEvents() {
  File fp = SPIFFS.open(FILE_EVENTS, FILE_WRITE);

  if (!fp) {
    Serial.println("Falha ao abrir arquivo");
    return false;
  }

  String csvHeaders = "Data;Hora;Evento;Lat;Front\n";

  if (fp.print(csvHeaders)) {
    Serial.println("Arquivo Criado!");
  } else {
    Serial.println("Erro ao gravar arquivo");
    fp.close();
    return false;
  }

  fp.close();
  return true;
}
