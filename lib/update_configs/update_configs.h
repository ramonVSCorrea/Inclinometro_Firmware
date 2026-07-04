#ifndef UPDATE_CONFIGS_H
#define UPDATE_CONFIGS_H

#include "configs_manager.h"
#include "global_variables.h"

/**
 * @brief Aplica atualizações de configuração a partir de um objeto JSON
 *
 * @param root Objeto JSON com angleConfigurations e wifiConfigurations
 */
bool applyConfigUpdateJson(cJSON* root);

#endif  // UPDATE_CONFIGS_H
