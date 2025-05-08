#ifndef CONFIGS_MANAGER_H
#define CONFIGS_MANAGER_H

#include <ArduinoJson.h>
#include <FS.h>
#include <SPIFFS.h>
#include <cJSON.h>
#include "global_variables.h"

// Definição de constantes
#define FILE_CONFIG "/configs.json"
#define FILE_WIFI_CONFIG "/wifi_config.json"
#define FORMAT_SPIFFS_IF_FAILED true
// #define DEBUG_FILE

/**
 * @brief Inicializa a partição SPIFFS e os arquivos necessários
 */
void initializeFiles();

/**
 * @brief Cria um arquivo JSON na partição SPIFFS do ESP32
 * @return Retorna true se o arquivo foi criado com sucesso, false caso
 * contrário
 */
bool createFileConfig();

/**
 * @brief Inicializa os valores de configuração do inclinômetro
 */
void initializeConfigs();

/**
 * @brief Altera os valores de bloqueio no arquivo de configurações
 */
void setBlockConfigs();

/**
 * @brief Altera os valores de calibração no arquivo de configurações
 */
void setCalibrationConfigs();

/**
 * @brief Altera os valores de configuração do WiFi no arquivo de configurações
 */
void setWiFiConfigs();

#endif  // CONFIGS_MANAGER_H
