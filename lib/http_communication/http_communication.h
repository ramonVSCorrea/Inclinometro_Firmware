#ifndef HTTP_COMMUNICATION_H
#define HTTP_COMMUNICATION_H

#include <Arduino.h>
#include <HTTPClient.h>
#include "global_variables.h"

#define SERVER_URL "http://api.tago.io/data"
#define DEVICE_TOKEN "55156222-043d-4058-8ed1-bae50449a22a"

/**
 * @brief Tarefa que envia os dados de inclinação para o servidor TAGO.IO
 *
 * @param pvParameters Parâmetros da tarefa
 */
void taskHttpCommunication(void* pvParameters);

/**
 * @brief Envia os dados de inclinação para o servidor TAGO.IO
 */
void sendDataInclinationToServer();

#endif  // HTTP_COMMUNICATION_H