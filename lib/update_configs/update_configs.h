#ifndef UPDATE_CONFIGS_H
#define UPDATE_CONFIGS_H

#include "configs_manager.h"
#include "global_variables.h"
#include "http_communication.h"

/**
 * @brief Tarefa para atualizar as configurações do dispositivo
 *
 * @param pvParameters Parâmetros da tarefa (não utilizados)
 */
void taskUpdateConfigs(void* pvParameters);

#endif  // UPDATE_CONFIGS_H