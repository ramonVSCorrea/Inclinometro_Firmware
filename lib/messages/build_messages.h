#ifndef BUILD_MESSAGES_H
#define BUILD_MESSAGES_H

#include <Arduino.h>
#include <cJSON.h>
#include "global_variables.h"

#define EVENT_MESSAGE_BUFFER_SIZE 320
#define INCLINATION_MESSAGE_BUFFER_SIZE 256
#define DEVICE_CONFIG_MESSAGE_BUFFER_SIZE 512

/**
 * @brief Constrói o payload JSON para um evento específico
 *
 * @param event Nome do evento a ser incluído no payload
 *
 * @return true se o payload coube no buffer
 */
bool buildEventMessage(const char* event, char* buffer, size_t bufferSize);

/**
 * @brief Constrói o payload JSON para os dados de inclinação
 *
 * @return true se o payload coube no buffer
 */
bool buildInclinationDataMessage(char* buffer, size_t bufferSize);

/**
 * @brief Constrói o payload JSON para as configurações do dispositivo
 *
 * @return true se o payload coube no buffer
 */
bool buildDeviceConfigurationsMessage(char* buffer, size_t bufferSize);

#endif  // BUILD_MESSAGES_H
