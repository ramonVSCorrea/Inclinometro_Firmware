#ifndef HTTP_COMMUNICATION_H
#define HTTP_COMMUNICATION_H

#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <cJSON.h>
#include "global_variables.h"

#define SERVER_URL "http://api.tago.io/data"
#define DEVICE_TOKEN "55156222-043d-4058-8ed1-bae50449a22a"

// Configurações da fila de requisições
#define MAX_REQUEST_QUEUE 10

// Definição dos tipos de requisição HTTP
typedef enum { HTTP_GET, HTTP_POST } HttpRequestType;

// Estrutura para armazenar requisições
typedef struct {
  String payload;  // Corpo para POST ou parâmetros para GET
  String url;      // URL completa
  HttpRequestType type;
  bool inUse;
} HttpRequest;

// Variáveis externas
extern HttpRequest requestQueue[MAX_REQUEST_QUEUE];
extern SemaphoreHandle_t httpSemaphore;
extern TaskHandle_t httpTaskHandle;
extern bool isWiFiConnected;

// Semáforo e variáveis para respostas GET
extern SemaphoreHandle_t configResponseSemaphore;
extern String lastConfigResponse;
extern bool configResponseReady;

/**
 * @brief Inicializa a fila de requisições HTTP
 */
void initHttpQueue();

/**
 * @brief Configura callbacks para processamento de respostas
 */
void setupHttpCallbacks();

/**
 * @brief Adiciona uma mensagem POST à fila
 *
 * @param payload Mensagem a ser enviada
 * @return true se a mensagem foi enfileirada com sucesso, false caso contrário
 */
bool queuePostMessage(String payload);

/**
 * @brief Adiciona uma requisição GET à fila
 *
 * @param url URL da requisição GET
 * @return true se a requisição foi enfileirada com sucesso, false caso
 * contrário
 */
bool queueGetRequest(String url);

/**
 * @brief Envia uma mensagem para o servidor
 *
 * @param payload Mensagem a ser enviada
 */
void sendMessageToServer(String payload);

/**
 * @brief Solicita atualização de configuração
 */
void requestLastConfigUpdate();

/**
 * @brief Recupera a última atualização de configuração
 *
 * @return String com o metadata da configuração
 */
String retrieveLastConfigUpdate();

/**
 * @brief Task principal de processamento HTTP
 */
void taskHttpSender(void* pvParameters);

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

/**
 * @brief Envia uma mensagem para o servidor
 *
 * @param payload mensagem a ser enviada
 */
void _sendMessageToServer(String payload);

/**
 * @brief Executa uma requisição GET
 *
 * @param url URL da requisição GET
 *
 * @return String com a resposta do servidor
 */
String _executeGetRequest(String url);

/**
 * @brief Processa a resposta de uma requisição GET
 *
 * @param response Resposta recebida do servidor
 */
void processGetResponse(String response);

#endif  // HTTP_COMMUNICATION_H