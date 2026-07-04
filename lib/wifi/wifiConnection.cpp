#include "wifiConnection.h"
#include "debug_log.h"
#include "mqtt_communication.h"
#include "stack_debug.h"

void taskWiFiConnection(void* pvParameters) {
  unsigned long stackLogTimer = 0;

  DBG_PRINTLN("\nConectando ao Wi-Fi...");
  DBG_PRINT("SSID: ");
  DBG_PRINTLN(wifiSSID);
  DBG_PRINT("Senha: ");
  DBG_PRINTLN(wifiPassword);
  WiFi.mode(WIFI_STA);    // Modo estação (evita manter um AP ativo)
  WiFi.disconnect(true);  // Desconecta e limpa as configurações prévias
  delay(100);             // Aguardar a limpeza

  WiFi.begin(wifiSSID, wifiPassword);

  int timeoutWifiConnection = 30;

  // Conectando pela primeira vez
  while (WiFi.status() != WL_CONNECTED && timeoutWifiConnection > 0) {
    DBG_PRINT(".");
    vTaskDelay(pdMS_TO_TICKS(1000));
    timeoutWifiConnection--;
  }

  if (WiFi.status() == WL_CONNECTED) {
    isWiFiConnected = true;
    DBG_PRINTLN("\n✅ Wi-Fi conectado!");
    DBG_PRINT("Endereço IP: ");
    DBG_PRINTLN(WiFi.localIP());
    setupMqtt();  // Configura o MQTT após conectar ao Wi-Fi
  } else {
    DBG_PRINTLN("\n❌ Falha ao conectar ao Wi-Fi.");
    printWiFiError(WiFi.status());
    isWiFiConnected = false;
  }

  while (1) {
    logTaskStackHighWaterMark("Tarefa_WiFi", &stackLogTimer);

    // Verifica se as credenciais foram alteradas
    if (isWiFiConfigChanged || WiFi.status() != WL_CONNECTED) {
      DBG_PRINTLN("\nReconectando ao Wi-Fi...");
      isWiFiConfigChanged = false;
      isWiFiConnected = false;
      isMqttConnected = false;

      // Desconecta e tenta novamente
      WiFi.disconnect(true);  // Limpa a conexão anterior
      delay(100);
      WiFi.begin(wifiSSID, wifiPassword);

      int attempts = 0;
      while (WiFi.status() != WL_CONNECTED && attempts < 15) {
        DBG_PRINT(".");
        vTaskDelay(pdMS_TO_TICKS(1000));
        attempts++;
      }

      if (WiFi.status() == WL_CONNECTED) {
        isWiFiConnected = true;
        DBG_PRINTLN("\n✅ Reconectado ao Wi-Fi!");
        DBG_PRINT("Novo IP: ");
        DBG_PRINTLN(WiFi.localIP());
        setupMqtt();
      } else {
        DBG_PRINTLN(
            "\n❌ Rede indisponível. Tentando novamente em 10 segundos...");
        vTaskDelay(pdMS_TO_TICKS(10000));  // Espera antes de tentar novamente
      }
    } else {
      maintainMqttConnection();
    }

    vTaskDelay(pdMS_TO_TICKS(5000));  // Verifica a cada 5 segundos
  }
}

void printWiFiError(int status) {
  DBG_PRINT("⚠️ Erro de conexão Wi-Fi: ");
  switch (status) {
    case WL_NO_SSID_AVAIL:
      DBG_PRINTLN("Rede Wi-Fi não encontrada! Verifique o SSID.");
      break;
    case WL_CONNECT_FAILED:
      DBG_PRINTLN("Falha na autenticação! Verifique a senha.");
      break;
    case WL_CONNECTION_LOST:
      DBG_PRINTLN("Conexão perdida! Verifique o sinal.");
      break;
    case WL_DISCONNECTED:
      DBG_PRINTLN("ESP32 não está conectado a nenhuma rede.");
      break;
    default:
      DBG_PRINTLN("Motivo desconhecido.");
      break;
  }
}
