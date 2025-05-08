#ifndef HTTP_COMMUNICATION_H
#define HTTP_COMMUNICATION_H

#include <Arduino.h>
#include <HTTPClient.h>
#include <cJSON.h>
#include "global_variables.h"

#define SERVER_URL "http://api.tago.io/data"
#define DEVICE_TOKEN "55156222-043d-4058-8ed1-bae50449a22a"

/**
 * @brief Constrói o payload de dados de inclinação
 */
String buildInclinationDataPayload();

/**
 * @brief Constrói o payload de evento
 *
 * @param event Tipo do Evento
 * @param description Descrição do Evento
 */
String buildEventPayload(String event, String description);

/**
 * @brief Constrói o payload de configurações do dispositivo
 */
String buildDeviceConfigurationsPayload();

/**
 * @brief Envia uma mensagem para o servidor
 *
 * @param payload Mensagem a ser enviada
 */
void sendMessageToServer(String payload);

/**
 * @brief Recupera a última atualização de configuração do dispositivo
 *
 * @return String com a última atualização de configuração
 */
String retrieveLastConfigUpdate();

#endif  // HTTP_COMMUNICATION_H