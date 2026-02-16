#include "wifiConnection.h"

void taskWiFiConnection(void* pvParameters) {
  Serial.println("\nConectando ao Wi-Fi...");
  WiFi.mode(WIFI_STA);    // Modo estação (evita manter um AP ativo)
  WiFi.disconnect(true);  // Desconecta e limpa as configurações prévias
  delay(100);             // Aguardar a limpeza

  WiFi.begin(wifiSSID, wifiPassword);

  int timeoutWifiConnection = 30;
  unsigned long timer = 0;

  // Conectando pela primeira vez
  while (WiFi.status() != WL_CONNECTED && timeoutWifiConnection > 0) {
    Serial.print(".");
    vTaskDelay(pdMS_TO_TICKS(1000));
    timeoutWifiConnection--;
  }

  if (WiFi.status() == WL_CONNECTED) {
    isWiFiConnected = true;
    Serial.println("\n✅ Wi-Fi conectado!");
    Serial.print("Endereço IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n❌ Falha ao conectar ao Wi-Fi.");
    printWiFiError(WiFi.status());
    isWiFiConnected = false;
  }

  while (1) {
    // Verifica se as credenciais foram alteradas
    if (isWiFiConfigChanged || WiFi.status() != WL_CONNECTED) {
      Serial.println("\nReconectando ao Wi-Fi...");
      isWiFiConfigChanged = false;
      isWiFiConnected = false;

      // Desconecta e tenta novamente
      WiFi.disconnect(true);  // Limpa a conexão anterior
      delay(100);
      WiFi.begin(wifiSSID, wifiPassword);

      int attempts = 0;
      while (WiFi.status() != WL_CONNECTED && attempts < 15) {
        Serial.print(".");
        vTaskDelay(pdMS_TO_TICKS(1000));
        attempts++;
      }

      if (WiFi.status() == WL_CONNECTED) {
        isWiFiConnected = true;
        Serial.println("\n✅ Reconectado ao Wi-Fi!");
        Serial.print("Novo IP: ");
        Serial.println(WiFi.localIP());
      } else {
        Serial.println(
            "\n❌ Rede indisponível. Tentando novamente em 10 segundos...");
        vTaskDelay(pdMS_TO_TICKS(10000));  // Espera antes de tentar novamente
      }
    } else {
      if ((millis() - timer) > 30000) {
        updateConfigs();  // Verifica se há novas configurações
        timer = millis();
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
