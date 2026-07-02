#ifndef UPDATE_CONFIGS_H
#define UPDATE_CONFIGS_H

#include "configs_manager.h"
#include "global_variables.h"
#include "http_communication.h"

/**
 * @brief Aplica atualizações de configuração a partir de um objeto JSON
 *
 * @param root Objeto JSON com angleConfigurations e wifiConfigurations
 */
bool applyConfigUpdateJson(cJSON* root);

/**
 * @brief Tarefa para atualizar as configurações do dispositivo
 *
 * @param pvParameters Parâmetros da tarefa (não utilizados)
 */
void taskUpdateConfigs(void* pvParameters);

void updateConfigs();

#endif  // UPDATE_CONFIGS_H
