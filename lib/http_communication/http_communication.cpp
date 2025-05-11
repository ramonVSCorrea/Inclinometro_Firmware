#include "http_communication.h"

// Variáveis globais
HttpRequest requestQueue[MAX_REQUEST_QUEUE];
SemaphoreHandle_t httpSemaphore = NULL;
TaskHandle_t httpTaskHandle = NULL;

// Variáveis para respostas GET
SemaphoreHandle_t configResponseSemaphore = NULL;
String lastConfigResponse = "{}";
bool configResponseReady = false;

// Timestamp da última requisição
unsigned long lastHttpRequestTime = 0;
const unsigned long MIN_HTTP_INTERVAL = 3000;  // 3 segundos entre requisições

// Contadores para análise de desempenho
int httpSuccessCount = 0;
int httpErrorCount = 0;

// Define modo debug
#define DBG_MSG_HTTP

// Inicializa a fila de requisições HTTP
void initHttpQueue() {
  // Inicializa a fila
  for (int i = 0; i < MAX_REQUEST_QUEUE; i++) {
    requestQueue[i].inUse = false;
  }

  // Cria semáforos para proteção de acesso
  httpSemaphore = xSemaphoreCreateMutex();
  configResponseSemaphore = xSemaphoreCreateMutex();

  // Cria task dedicada para envio HTTP
  xTaskCreatePinnedToCore(taskHttpSender, "HTTPSender",
                          4096,  // Stack size
                          NULL,
                          1,  // Prioridade
                          &httpTaskHandle,
                          1  // Core 1
  );

  Serial.println("🚀 Sistema de fila HTTP inicializado");
}

// Configura callbacks para processamento de respostas
void setupHttpCallbacks() {
  // Inicializar variáveis de estado
  configResponseReady = false;
}

// Adiciona uma mensagem POST à fila
bool queuePostMessage(String payload) {
  if (!isWiFiConnected) {
    Serial.println("WiFi não conectado, descartando requisição");
    return false;
  }

  if (httpSemaphore == NULL) {
    Serial.println("❌ Semáforo HTTP não inicializado!");
    return false;
  }

  if (xSemaphoreTake(httpSemaphore, pdMS_TO_TICKS(100)) == pdTRUE) {
    // Procura por um slot livre na fila
    for (int i = 0; i < MAX_REQUEST_QUEUE; i++) {
      if (!requestQueue[i].inUse) {
        requestQueue[i].payload = payload;
        requestQueue[i].url = SERVER_URL;
        requestQueue[i].type = HTTP_POST;
        requestQueue[i].inUse = true;

        xSemaphoreGive(httpSemaphore);

        // Notifica task de envio que há nova mensagem
        if (httpTaskHandle != NULL) {
          xTaskNotify(httpTaskHandle, 0, eNoAction);
        }

#ifdef DBG_MSG_HTTP
        Serial.println("📤 Mensagem POST enfileirada");
#endif

        return true;
      }
    }

    xSemaphoreGive(httpSemaphore);
    Serial.println("⚠️ Fila HTTP cheia!");
    return false;
  }

  Serial.println("⚠️ Não foi possível obter semáforo HTTP!");
  return false;
}

// Adiciona uma requisição GET à fila
bool queueGetRequest(String url) {
  if (!isWiFiConnected) {
    Serial.println("WiFi não conectado, descartando requisição");
    return false;
  }

  if (xSemaphoreTake(httpSemaphore, pdMS_TO_TICKS(100)) == pdTRUE) {
    // Procura por um slot livre na fila
    for (int i = 0; i < MAX_REQUEST_QUEUE; i++) {
      if (!requestQueue[i].inUse) {
        requestQueue[i].payload = "";
        requestQueue[i].url = url;
        requestQueue[i].type = HTTP_GET;
        requestQueue[i].inUse = true;

        xSemaphoreGive(httpSemaphore);

        // Notifica task de envio que há nova requisição
        if (httpTaskHandle != NULL) {
          xTaskNotify(httpTaskHandle, 0, eNoAction);
        }

#ifdef DBG_MSG_HTTP
        Serial.println("📥 Requisição GET enfileirada");
#endif

        return true;
      }
    }

    xSemaphoreGive(httpSemaphore);
    Serial.println("⚠️ Fila HTTP cheia!");
    return false;
  }

  Serial.println("⚠️ Não foi possível obter semáforo HTTP!");
  return false;
}

// Task principal de processamento HTTP
void taskHttpSender(void* pvParameters) {
  Serial.println("👷 Task HTTP Sender iniciada");

  while (1) {
    // Aguarda notificação ou timeout
    uint32_t notification;
    xTaskNotifyWait(0, 0, &notification, pdMS_TO_TICKS(5000));

    // Verifica se há requisições para processar
    bool requestProcessed = false;

    // Respeita o intervalo mínimo entre requisições
    unsigned long currentTime = millis();
    if (currentTime - lastHttpRequestTime < MIN_HTTP_INTERVAL) {
      vTaskDelay(pdMS_TO_TICKS(MIN_HTTP_INTERVAL -
                               (currentTime - lastHttpRequestTime)));
    }

    if (xSemaphoreTake(httpSemaphore, pdMS_TO_TICKS(100)) == pdTRUE) {
      for (int i = 0; i < MAX_REQUEST_QUEUE; i++) {
        if (requestQueue[i].inUse) {
          HttpRequest request = requestQueue[i];
          requestQueue[i].inUse = false;  // Marca como processado
          xSemaphoreGive(httpSemaphore);

          // Processa a requisição conforme o tipo
          if (request.type == HTTP_POST) {
#ifdef DBG_MSG_HTTP
            Serial.println("📤 Processando requisição POST");
#endif
            _sendMessageToServer(request.payload);
          } else {  // HTTP_GET
#ifdef DBG_MSG_HTTP
            Serial.println("📥 Processando requisição GET");
#endif
            String response = _executeGetRequest(request.url);
            processGetResponse(response);
          }

          lastHttpRequestTime = millis();
          requestProcessed = true;
          break;
        }
      }

      if (!requestProcessed) {
        xSemaphoreGive(httpSemaphore);
      }
    }

    // Se não processou nenhuma requisição, aguarda um tempo
    if (!requestProcessed) {
      vTaskDelay(pdMS_TO_TICKS(500));
    }
  }
}

// Função interna para processar requisições POST
void _sendMessageToServer(String payload) {
  if (!isWiFiConnected) {
    Serial.println("WiFi não conectado!");
    return;
  }

  WiFiClient client;
  HTTPClient http;

  client.setTimeout(15);
  http.setTimeout(15000);

  if (http.begin(client, SERVER_URL)) {
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Device-Token", DEVICE_TOKEN);

    int httpResponseCode = http.POST(payload);

#ifdef DBG_MSG_HTTP
    if (httpResponseCode > 0) {
      String response = http.getString();  // Esvazia buffer
      Serial.print("✅ HTTP POST Response: ");
      Serial.println(httpResponseCode);
      httpSuccessCount++;
    } else {
      Serial.print("❌ HTTP POST Error: ");
      Serial.println(httpResponseCode);
      httpErrorCount++;
    }
#endif

    http.end();
    vTaskDelay(pdMS_TO_TICKS(50));  // Pequeno delay para finalizar a desconexão
  } else {
    Serial.println("❌ Falha ao iniciar HTTP client");
    httpErrorCount++;
  }
}

// Função interna para processar requisições GET
String _executeGetRequest(String url) {
  String response = "{}";

  if (!isWiFiConnected) {
    return response;
  }

  WiFiClient client;
  HTTPClient http;

  client.setTimeout(15);
  http.setTimeout(15000);

  if (http.begin(client, url)) {
    http.addHeader("Device-Token", DEVICE_TOKEN);

    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      response = http.getString();

#ifdef DBG_MSG_HTTP
      Serial.print("✅ HTTP GET Response: ");
      Serial.println(httpResponseCode);
      httpSuccessCount++;
#endif
    } else {
#ifdef DBG_MSG_HTTP
      Serial.print("❌ HTTP GET Error: ");
      Serial.println(httpResponseCode);
      httpErrorCount++;
#endif
    }

    http.end();
    vTaskDelay(pdMS_TO_TICKS(50));
  } else {
    Serial.println("❌ Falha ao iniciar HTTP client para GET");
    httpErrorCount++;
  }

  return response;
}

// Processa a resposta de uma requisição GET
void processGetResponse(String response) {
  // Verifica se é uma resposta de configuração
  if (response.indexOf("updateDeviceConfigurations") > 0) {
    if (xSemaphoreTake(configResponseSemaphore, pdMS_TO_TICKS(100)) == pdTRUE) {
      lastConfigResponse = response;
      configResponseReady = true;
      xSemaphoreGive(configResponseSemaphore);

#ifdef DBG_MSG_HTTP
      Serial.println("📝 Recebida resposta de configuração");
#endif
    }
  }
}

// Implementação da API pública - compatibilidade com código existente
void sendMessageToServer(String payload) {
  if (!isWiFiConnected || httpSemaphore == NULL) {
    Serial.println("WiFi não conectado ou sistema HTTP não inicializado");
    return;
  }
  queuePostMessage(payload);
}

// Função para solicitar atualização de configuração
void requestLastConfigUpdate() {
  String url = String(SERVER_URL) + "?variables=updateDeviceConfigurations" +
               "&groups=" + id + "&query=last_value";
  queueGetRequest(url);
}

// Verifica se há resposta de configuração disponível
// e retorna o metadata (bloqueante)
String retrieveLastConfigUpdate() {
  String metadata = "{}";

  // Solicita atualização de configuração
  requestLastConfigUpdate();

  // Aguarda até que a resposta esteja pronta ou timeout
  for (int i = 0; i < 50; i++) {  // Espera até 5 segundos
    if (xSemaphoreTake(configResponseSemaphore, pdMS_TO_TICKS(100)) == pdTRUE) {
      if (configResponseReady) {
        // Parse do JSON para extrair apenas o metadata
        cJSON* root = cJSON_Parse(lastConfigResponse.c_str());
        if (root != NULL) {
          // Verifica se a resposta foi bem-sucedida
          cJSON* status = cJSON_GetObjectItem(root, "status");
          if (cJSON_IsTrue(status)) {
            // Obtém o array de resultados
            cJSON* result = cJSON_GetObjectItem(root, "result");
            if (cJSON_IsArray(result) && cJSON_GetArraySize(result) > 0) {
              // Obtém o primeiro item do array
              cJSON* item = cJSON_GetArrayItem(result, 0);
              if (item != NULL) {
                // Obtém o objeto metadata
                cJSON* metadataObj = cJSON_GetObjectItem(item, "metadata");
                if (metadataObj != NULL) {
                  // Converte o objeto metadata para string
                  char* metadataStr = cJSON_Print(metadataObj);
                  if (metadataStr != NULL) {
                    metadata = String(metadataStr);
                    free(metadataStr);
                  }
                }
              }
            }
          }

          cJSON_Delete(root);
        }

        // Marca como processada
        configResponseReady = false;
        xSemaphoreGive(configResponseSemaphore);
        break;
      }
      xSemaphoreGive(configResponseSemaphore);
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }

  return metadata;
}

// Funções para construção de payloads
String buildInclinationDataPayload() {
  return "{"
         "\"variable\":\"inclinationData\","
         "\"value\":\"inclinationData\","
         "\"metadata\":{"
         "\"lateralAngle\":" +
         String(lateralAngle) +
         ","
         "\"frontalAngle\":" +
         String(frontalAngle) +
         ","
         "\"latitude\":" +
         String(latitude, 6) +
         ","
         "\"longitude\":" +
         String(longitude, 6) +
         "},"
         "\"group\":\"" +
         id +
         "\""
         "}";
}

String buildEventPayload(String event, String description) {
  return "{"
         "\"variable\":\"event\","
         "\"value\":\"event\","
         "\"metadata\":{"
         "\"event\":\"" +
         event +
         "\","
         "\"description\":\"" +
         description +
         "\","
         "\"lateralAngle\":" +
         String(lateralAngle) +
         ","
         "\"frontalAngle\":" +
         String(frontalAngle) +
         ","
         "\"latitude\":" +
         String(latitude, 6) +
         ","
         "\"longitude\":" +
         String(longitude, 6) +
         "},"
         "\"group\":\"" +
         id +
         "\""
         "}";
}

String buildDeviceConfigurationsPayload() {
  return "{"
         "\"variable\":\"deviceConfigurations\","
         "\"value\":\"deviceConfigurations\","
         "\"metadata\":{"
         "\"configurations\":{"
         "\"blockLateralAngle\":" +
         String(blockLateralAngle) +
         ","
         "\"blockFrontalAngle\":" +
         String(blockFrontalAngle) +
         ","
         "\"calibrateLateralAngle\":" +
         String(calibrateLateralAngle) +
         ","
         "\"calibrateFrontalAngle\":" +
         String(calibrateFrontalAngle) +
         "},"
         "\"wifiConfigs\":{"
         "\"SSID\":\"" +
         wifiSSID +
         "\""
         "}"
         "},"
         "\"group\":\"" +
         id +
         "\""
         "}";
}