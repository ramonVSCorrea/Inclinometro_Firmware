#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <stdlib.h>
#include "BluetoothSerial.h"
#include "FS.h"
#include "SPIFFS.h"
#include "cJSON.h"
#include "configs_manager.h"
#include "define_pins.h"
#include "global_variables.h"

// #define DBG_MSG_BLUE
#define CONFIG_BLQ "configuracoesBLQ"
#define CONFIG_CALIB "configuraCalib"
#define JSON_BLQ_LAT "bloqueioLateral"
#define JSON_BLQ_FRT "bloqueioFrontal"
#define TOTAL_EVENTOS "totalEventos"
#define LER_EVENTOS "numEvento"
#define CMD_BASC "comandoBascula"
#define CMD_LEITURA "requisicaoLeitura"
#define CMD_LER_CFG "requisitaCfg"
#define CMD_ALTERA_DATA "alteraData"
#define JSON_SUBIR "subir"
#define JSON_DESCER "descer"

/**
 * @brief Tarefa Bluetooth
 */
void taskBluetooth(void* pvParameters);

/**
 * @brief Função que envia valores de leitura de angulo para o app
 */
void sendBluetoothReadings();

/**
 * @brief Função que lê informações recebidas via bluetooth
 */
void readBluetoothData();

#endif  // BLUETOOTH_H