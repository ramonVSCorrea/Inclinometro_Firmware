#ifndef HTTP_COMMUNICATION_H
#define HTTP_COMMUNICATION_H

#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <cJSON.h>
#include "global_variables.h"

#define SERVER_URL "http://api.tago.io/data"
#define DEVICE_TOKEN "55156222-043d-4058-8ed1-bae50449a22a"

extern bool isWiFiConnected;

/**
 * @brief Envia uma mensagem para o servidor
 *
 * @param payload Mensagem a ser enviada
 */
void sendMessageToServer(String payload);

/**
 * @brief Recupera a última atualização de configuração
 *
 * @return String com o metadata da configuração
 */
String retrieveLastConfigUpdate();

/**
 * @brief Constrói o payload para dados de inclinação
 *
 * @return String com o payload formatado
 */
String buildInclinationDataPayload();

/**
 * @brief Constrói o payload para eventos
 *
 * @param event Nome do evento
 * @param description Descrição do evento
 *
 * @return String com o payload formatado
 */
String buildEventPayload(String event, String description);

/**
 * @brief Constrói o payload para configurações do dispositivo
 *
 * @return String com o payload formatado
 */
String buildDeviceConfigurationsPayload();

#endif  // HTTP_COMMUNICATION_H