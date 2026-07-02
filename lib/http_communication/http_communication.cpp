#include "http_communication.h"

#define DBG_MSG_HTTP

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
         "\"angleConfigurations\":{"
         "\"blockLateralAngle\":" +
         String(blockLateralAngle) +
         ","
         "\"blockFrontalAngle\":" +
         String(blockFrontalAngle) +
         "},"
         "\"wifiConfigurations\":{"
         "\"ssid\":\"" +
         wifiSSID +
         "\""
         "}"
         "},"
         "\"group\":\"" +
         id +
         "\""
         "}";
}

void sendMessageToServer(String payload) {
  isHttpRequest = true;

  if (xSemaphoreTake(httpMutex, pdMS_TO_TICKS(5000)) == pdTRUE) {
    if (isWiFiConnected) {
      WiFiClient client;  // Criar um novo cliente para cada requisição
      HTTPClient http;

      // Usar o cliente explicitamente
      if (http.begin(client, SERVER_URL)) {
        http.addHeader("Content-Type", "application/json");
        http.addHeader("Device-Token", DEVICE_TOKEN);

        int httpResponseCode = http.POST(payload);
        Serial.println("🛫 Payload enviado: " + payload);

        // Ler a resposta (importante para liberar recursos)
        if (httpResponseCode > 0) {
          String response = http.getString();
          Serial.println("✅ Código de resposta HTTP: " + response + "\n\n");
        }

        // Garantir que a conexão seja fechada corretamente
        http.end();
      } else {
        Serial.println("Não foi possível iniciar a requisição HTTP");
      }
    } else {
      Serial.println("WiFi not connected");
    }
    xSemaphoreGive(httpMutex);
    isHttpRequest = false;
  } else {
    Serial.println("Não foi possível obter o mutex para HTTP.");
  }
}

String retrieveLastConfigUpdate() {
  isHttpRequest = true;
  String metadata = "{}";  // Valor padrão caso ocorra algum erro

  if (xSemaphoreTake(httpMutex, pdMS_TO_TICKS(5000)) == pdTRUE) {
    if (isWiFiConnected) {
      WiFiClient client;  // Criar um novo cliente para cada requisição
      HTTPClient http;

      // Constrói a URL com os parâmetros de consulta
      String url = String(SERVER_URL) +
                   "?variables=updateDeviceConfigurations" + "&groups=" + id +
                   "&query=last_value";

      // Usar o cliente explicitamente
      if (http.begin(client, url)) {
        http.addHeader("Device-Token", DEVICE_TOKEN);

        int httpResponseCode = http.GET();

#ifdef DBG_MSG_HTTP
        Serial.print("Requisição GET para: ");
        Serial.println(url);
#endif

        if (httpResponseCode > 0) {
#ifdef DBG_MSG_HTTP
          Serial.print("HTTP Response code (GET): ");
          Serial.println(httpResponseCode);
#endif

          String response = http.getString();

#ifdef DBG_MSG_HTTP
          Serial.print("Resposta completa: ");
          Serial.println(response);
#endif

          // Parse do JSON para extrair apenas o metadata
          cJSON* root = cJSON_Parse(response.c_str());
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
                      free(metadataStr);  // Libera a memória alocada pelo
                                          // cJSON_Print
                    }
                  }
                }
              }
            }

            cJSON_Delete(root);  // Libera a memória do objeto JSON
          }
        } else {
#ifdef DBG_MSG_HTTP
          Serial.print("Erro na requisição GET. Código: ");
          Serial.println(httpResponseCode);
#endif
        }

        // Garantir que a conexão seja fechada corretamente
        http.end();
      } else {
        Serial.println("Não foi possível iniciar a requisição HTTP");
      }
    } else {
      Serial.println("WiFi não conectado");
    }
    xSemaphoreGive(httpMutex);
    isHttpRequest = false;
  } else {
    Serial.println("Não foi possível obter o mutex para HTTP.");
  }

  return metadata;
}
