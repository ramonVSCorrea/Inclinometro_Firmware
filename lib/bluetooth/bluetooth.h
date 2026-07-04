#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <stdlib.h>
#include <NimBLEDevice.h>
#include <string.h>
#include <string>
#include "build_messages.h"
#include "configs_manager.h"
#include "define_pins.h"
#include "events_constants.h"
#include "global_variables.h"
#include "mqtt_communication.h"

// #define DBG_MSG_BLUE

#define INCLIMAX_SERVICE_UUID "b8f8a000-2f3f-4a1b-9f70-000000000001"
#define COMMAND_CHAR_UUID "b8f8a000-2f3f-4a1b-9f70-000000000002"
#define RESPONSE_CHAR_UUID "b8f8a000-2f3f-4a1b-9f70-000000000003"
#define READINGS_CHAR_UUID "b8f8a000-2f3f-4a1b-9f70-000000000004"
#define STATUS_CHAR_UUID "b8f8a000-2f3f-4a1b-9f70-000000000005"

#define BLE_DEVICE_NAME "IncliMax - AAAA"
#define BLE_MAX_MESSAGE_LENGTH 180

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

void sendBluetoothMessage(const char* message);
void processBluetoothCommand(const char* command);

#endif  // BLUETOOTH_H
