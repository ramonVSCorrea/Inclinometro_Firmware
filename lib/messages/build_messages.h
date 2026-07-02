#ifndef BUILD_MESSAGES_H
#define BUILD_MESSAGES_H

#include <cJSON.h>
#include "global_variables.h"

/**
 * @brief Constrói o payload JSON para um evento específico
 *
 * @param event Nome do evento a ser incluído no payload
 *
 * @return String contendo o payload JSON formatado para o evento
 */
String buildEventMessage(String event);

/**
 * @brief Constrói o payload JSON para os dados de inclinação
 *
 * @return String contendo o payload JSON formatado para os dados de inclinação
 */
String buildInclinationDataMessage();

/**
 * @brief Constrói o payload JSON para as configurações do dispositivo
 *
 * @return String contendo o payload JSON formatado para as configurações do
 * dispositivo
 */
String buildDeviceConfigurationsMessage();

#endif  // BUILD_MESSAGES_H