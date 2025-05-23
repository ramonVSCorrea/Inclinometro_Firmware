#ifndef WIFI_CONNECTION_H
#define WIFI_CONNECTION_H

#include <WiFi.h>
#include "global_variables.h"
#include "update_configs.h"

// #define WIFI_SSID "LIVE TIM_0820_2G"
// #define WIFI_PASSWORD "a3ehn6rep6"

#define WIFI_SSID "S23 de Ramon"
#define WIFI_PASSWORD "s3nh4roteador"

/**
 * @brief Debug de erro de conexão WiFi
 *
 * @param status Status de erro
 */
void printWiFiError(int status);

/**
 * @brief Tarefa que verifica a conexão WiFi
 *
 * @param pvParameters Parâmetros da tarefa
 */
void taskWiFiConnection(void* pvParameters);

#endif  // WIFI_CONNECTION_H