#include "configs_manager.h"
#include "debug_log.h"

namespace {
constexpr size_t CONFIG_FILE_BUFFER_SIZE = 768;
char configFileBuffer[CONFIG_FILE_BUFFER_SIZE];

bool readConfigFile(char* buffer, size_t bufferSize) {
  if (buffer == nullptr || bufferSize == 0) {
    return false;
  }

  File fp = SPIFFS.open(FILE_CONFIG, FILE_READ);
  if (!fp) {
    DBG_PRINTLN("Falha ao abrir arquivo");
    return false;
  }

  size_t bytesRead = fp.readBytes(buffer, bufferSize - 1);
  buffer[bytesRead] = '\0';
  fp.close();

  return bytesRead > 0;
}

bool writeJsonToFile(File& fp, cJSON* root) {
  char* json = cJSON_PrintUnformatted(root);
  if (json == nullptr) {
    return false;
  }

  size_t bytesWritten = fp.print(json);
  free(json);

  return bytesWritten > 0;
}
}  // namespace

void initializeFiles() {
  SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED);  // Inicia SPIFFS e formata caso falhe
  DBG_PRINTLN("SPIFFS inicializado!");

  // Criação dos arquivos caso não existam
  if (!SPIFFS.exists(FILE_CONFIG)) {
    if (createFileConfig()) {
      DBG_PRINTLN("Arquivo de configurações criado com sucesso!");
    } else {
      DBG_PRINTLN("Erro ao criar arquivo de configurações!");
    }
  }

  initializeConfigs();  // Inicia as variáveis globais
}

bool createFileConfig() {
  File fp = SPIFFS.open(FILE_CONFIG, FILE_WRITE);

  if (!fp) {
    DBG_PRINTLN("Falha ao abrir arquivo");
    return false;
  }

  // Criação do objeto JSON
  cJSON* root = cJSON_CreateObject();
  cJSON* configs = cJSON_CreateObject();
  cJSON* wifiConfigs = cJSON_CreateObject();

  if (root == nullptr || configs == nullptr || wifiConfigs == nullptr) {
    DBG_PRINTLN("Erro ao criar JSON de configurações");
    cJSON_Delete(root);
    cJSON_Delete(configs);
    cJSON_Delete(wifiConfigs);
    fp.close();
    return false;
  }

  cJSON_AddNumberToObject(configs, "blockLateralAngle", 3.5);
  cJSON_AddNumberToObject(configs, "blockFrontalAngle", 5);
  cJSON_AddNumberToObject(configs, "calibrateLateralAngle", 0);
  cJSON_AddNumberToObject(configs, "calibrateFrontalAngle", 0);

  cJSON_AddItemToObject(root, "configurations", configs);

  cJSON_AddStringToObject(wifiConfigs, "SSID", wifiSSID);
  cJSON_AddStringToObject(wifiConfigs, "password", wifiPassword);

  cJSON_AddItemToObject(root, "wifiConfigs", wifiConfigs);

  if (writeJsonToFile(fp, root)) {
    DBG_PRINTLN("Arquivo Criado!");
  } else {
    DBG_PRINTLN("Erro ao gravar arquivo");
    cJSON_Delete(root);
    fp.close();
    return false;
  }

  cJSON_Delete(root);
  fp.close();

#ifdef DEBUG_FILE
  fp = SPIFFS.open(FILE_CONFIG);

  while (fp.available()) {
    DBG_WRITE(fp.read());
  }

  fp.close();
#endif

  return true;
}

void initializeConfigs() {
  if (!readConfigFile(configFileBuffer, sizeof(configFileBuffer))) {
    return;
  }

  cJSON* root = cJSON_Parse(configFileBuffer);

  if (cJSON_IsObject(root)) {
    cJSON* configs = cJSON_GetObjectItem(root, "configurations");
    cJSON* wifiConfigs = cJSON_GetObjectItem(root, "wifiConfigs");

    cJSON* node = cJSON_GetObjectItem(configs, "blockLateralAngle");
    blockLateralAngle = cJSON_GetNumberValue(node);

    node = cJSON_GetObjectItem(configs, "blockFrontalAngle");
    blockFrontalAngle = cJSON_GetNumberValue(node);

    node = cJSON_GetObjectItem(configs, "calibrateLateralAngle");
    calibrateLateralAngle = cJSON_GetNumberValue(node);

    node = cJSON_GetObjectItem(configs, "calibrateFrontalAngle");
    calibrateFrontalAngle = cJSON_GetNumberValue(node);

    node = cJSON_GetObjectItem(wifiConfigs, "SSID");
    if (cJSON_IsString(node)) {
      snprintf(wifiSSID, sizeof(wifiSSID), "%s", cJSON_GetStringValue(node));
    }

    node = cJSON_GetObjectItem(wifiConfigs, "password");
    if (cJSON_IsString(node)) {
      snprintf(wifiPassword, sizeof(wifiPassword), "%s",
               cJSON_GetStringValue(node));
    }
  }

  cJSON_Delete(root);
}

void setBlockConfigs() {
  if (!readConfigFile(configFileBuffer, sizeof(configFileBuffer))) {
    return;
  }

  cJSON* root = cJSON_Parse(configFileBuffer);
  if (!cJSON_IsObject(root)) {
    DBG_PRINTLN("Erro ao analisar arquivo de configurações");
    cJSON_Delete(root);
    return;
  }

  cJSON* configs = cJSON_GetObjectItem(root, "configurations");

  if (cJSON_IsObject(configs)) {
    cJSON* node = cJSON_GetObjectItem(configs, "blockLateralAngle");
    cJSON_SetNumberValue(node, blockLateralAngle);

    node = cJSON_GetObjectItem(configs, "blockFrontalAngle");
    cJSON_SetNumberValue(node, blockFrontalAngle);

    node = cJSON_GetObjectItem(configs, "calibrateLateralAngle");
    cJSON_SetNumberValue(node, calibrateLateralAngle);

    node = cJSON_GetObjectItem(configs, "calibrateFrontalAngle");
    cJSON_SetNumberValue(node, calibrateFrontalAngle);
  }

  File fp = SPIFFS.open(FILE_CONFIG, FILE_WRITE);
  if (!fp) {
    DBG_PRINTLN("Falha ao abrir arquivo");
    cJSON_Delete(root);
    return;
  }

  if (writeJsonToFile(fp, root)) {
    DBG_PRINTLN("Arquivo Alterado!");
  } else {
    DBG_PRINTLN("Erro ao alterar arquivo");
  }

  cJSON_Delete(root);
  fp.close();
  initializeConfigs();
}

void setCalibrationConfigs() {
  if (!readConfigFile(configFileBuffer, sizeof(configFileBuffer))) {
    return;
  }

  cJSON* root = cJSON_Parse(configFileBuffer);
  if (!cJSON_IsObject(root)) {
    DBG_PRINTLN("Erro ao analisar arquivo de configurações");
    cJSON_Delete(root);
    return;
  }

  cJSON* configs = cJSON_GetObjectItem(root, "configurations");

  if (cJSON_IsObject(configs)) {
    cJSON* node = cJSON_GetObjectItem(configs, "calibrateLateralAngle");
    cJSON_SetNumberValue(node, calibrateLateralAngle);

    node = cJSON_GetObjectItem(configs, "calibrateFrontalAngle");
    cJSON_SetNumberValue(node, calibrateFrontalAngle);
  }

  File fp = SPIFFS.open(FILE_CONFIG, FILE_WRITE);
  if (!fp) {
    DBG_PRINTLN("Falha ao abrir arquivo");
    cJSON_Delete(root);
    return;
  }

  if (writeJsonToFile(fp, root)) {
    DBG_PRINTLN("Arquivo Alterado!");
  } else {
    DBG_PRINTLN("Erro ao alterar arquivo");
  }

  cJSON_Delete(root);
  fp.close();
  initializeConfigs();
}

void setWiFiConfigs() {
  if (!readConfigFile(configFileBuffer, sizeof(configFileBuffer))) {
    return;
  }

  cJSON* root = cJSON_Parse(configFileBuffer);
  if (!cJSON_IsObject(root)) {
    DBG_PRINTLN("Erro ao analisar arquivo de configurações");
    cJSON_Delete(root);
    return;
  }

  cJSON* wifiConfigs = cJSON_GetObjectItem(root, "wifiConfigs");

  if (cJSON_IsObject(wifiConfigs)) {
    cJSON* node = cJSON_GetObjectItem(wifiConfigs, "SSID");
    cJSON_SetValuestring(node, wifiSSID);

    node = cJSON_GetObjectItem(wifiConfigs, "password");
    cJSON_SetValuestring(node, wifiPassword);
  }

  File fp = SPIFFS.open(FILE_CONFIG, FILE_WRITE);
  if (!fp) {
    DBG_PRINTLN("Falha ao abrir arquivo");
    cJSON_Delete(root);
    return;
  }

  if (writeJsonToFile(fp, root)) {
    DBG_PRINTLN("Configurações WiFi alteradas!");

    isWiFiConfigChanged =
        true;  // Sinaliza que as configurações foram alteradas
  } else {
    DBG_PRINTLN("Erro ao alterar configurações WiFi");
  }

  cJSON_Delete(root);
  fp.close();
  initializeConfigs();
}
