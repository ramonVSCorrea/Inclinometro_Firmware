#include "wifiConnection.h"

void taskWiFiConnection(void* pvParameters) {
  Serial.println("\nConectando ao Wi-Fi...");
  WiFi.begin(wifiSSID, wifiPassword);

  int timeoutWifiConnection = 30;

  while (WiFi.status() != WL_CONNECTED && timeoutWifiConnection > 0) {
    Serial.print(".");
    delay(1000);
    timeoutWifiConnection--;
  }

  if (WiFi.status() == WL_CONNECTED) {
    isWiFiConnected = true;
    Serial.println("\nWi-Fi conectado!");
    Serial.print("Endereço IP: ");
    Serial.println(WiFi.localIP());

    // Inicia a fila de requisições HTTP
    // e configura os callbacks
    initHttpQueue();
    setupHttpCallbacks();
  } else {
    Serial.println("\n❌ Falha ao conectar ao Wi-Fi.");
    printWiFiError(WiFi.status());
  }

  while (1) {
    // Verifica se as credenciais foram alteradas
    if (isWiFiConfigChanged) {
      Serial.println("\nCredenciais Wi-Fi alteradas. Reconectando...");
      WiFi.disconnect();
      delay(1000);
      WiFi.begin(wifiSSID, wifiPassword);

      int attempts = 0;
      while (WiFi.status() != WL_CONNECTED && attempts < 15) {
        Serial.print(".");
        delay(1000);
        attempts++;
      }

      if (WiFi.status() == WL_CONNECTED) {
        isWiFiConnected = true;
        Serial.println("\nConectado com novas credenciais!");
        Serial.print("Novo IP: ");
        Serial.println(WiFi.localIP());
      } else {
        Serial.println("\n❌ Falha ao conectar com as novas credenciais.");
        printWiFiError(WiFi.status());
        isWiFiConnected = false;
      }

      // Reseta o sinalizador
      isWiFiConfigChanged = false;
    }

    // Verificação de desconexão normal (mantém o código existente)
    else if (WiFi.status() != WL_CONNECTED) {
      isWiFiConnected = false;
      Serial.println("\nWi-Fi desconectado! Tentando reconectar...");
      WiFi.disconnect();
      WiFi.begin(wifiSSID, wifiPassword);

      int attempts = 0;
      while (WiFi.status() != WL_CONNECTED && attempts < 15) {
        Serial.print(".");
        delay(1000);
        attempts++;
      }

      if (WiFi.status() == WL_CONNECTED) {
        isWiFiConnected = true;
        Serial.println("\nReconectado ao Wi-Fi!");
        Serial.print("Novo IP: ");
        Serial.println(WiFi.localIP());
      } else {
        Serial.println(
            "\nRede indisponível. Tentando novamente em 10 segundos...");
        vTaskDelay(pdMS_TO_TICKS(10000));
        isWiFiConnected = false;
      }
    }

    vTaskDelay(pdMS_TO_TICKS(5000));  // Verifica a cada 5 segundos
  }
}

void printWiFiError(int status) {
  Serial.print("⚠️ Erro de conexão Wi-Fi: ");
  switch (status) {
    case WL_NO_SSID_AVAIL:
      Serial.println("Rede Wi-Fi não encontrada! Verifique o SSID.");
      break;
    case WL_CONNECT_FAILED:
      Serial.println("Falha na autenticação! Verifique a senha.");
      break;
    case WL_CONNECTION_LOST:
      Serial.println("Conexão perdida! Verifique o sinal.");
      break;
    case WL_DISCONNECTED:
      Serial.println("ESP32 não está conectado a nenhuma rede.");
      break;
    default:
      Serial.println("Motivo desconhecido.");
      break;
  }
}