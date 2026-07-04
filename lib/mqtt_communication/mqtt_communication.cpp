#include "mqtt_communication.h"
#include "build_messages.h"
#include "debug_log.h"
#include "global_variables.h"
#include "update_configs.h"

espMqttClientSecure mqttClient(espMqttClientTypes::UseInternalTask::YES);

namespace {
constexpr size_t UPDATE_CONFIGS_BUFFER_SIZE = 512;
constexpr unsigned long MQTT_RECONNECT_INTERVAL_MS = 10000;
char updateConfigsBuffer[UPDATE_CONFIGS_BUFFER_SIZE];
size_t updateConfigsBufferLength = 0;
bool updateConfigsBufferOverflow = false;
char configPayload[DEVICE_CONFIG_MESSAGE_BUFFER_SIZE];
char updateConfigsTopic[96];
bool mqttConfigured = false;
unsigned long lastMqttConnectAttempt = 0;

void buildUpdateConfigsTopic() {
  snprintf(updateConfigsTopic, sizeof(updateConfigsTopic), "%s%s%s",
           MQTT_UPDATE_CONFIGS_PREFIX, id, MQTT_UPDATE_CONFIGS_SUFFIX);
}

void configureMqttClient() {
  if (mqttConfigured) {
    return;
  }

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.onPublish(onMqttPublish);

  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
  mqttClient.setClientId(MQTT_CLIENT_ID);
  mqttClient.setCredentials(MQTT_USERNAME, MQTT_PASSWORD);
  mqttClient.setInsecure();

  mqttConfigured = true;
}
}

void onMqttConnect(bool sessionPresent) {
  DBG_PRINTLN("Conectado ao MQTT!");

  buildUpdateConfigsTopic();
  mqttClient.subscribe(updateConfigsTopic, 0);

  isMqttConnected = true;
}

void onMqttDisconnect(espMqttClientTypes::DisconnectReason reason) {
  isMqttConnected = false;
  DBG_PRINTF("Desconectado do MQTT. Motivo: %d\n", (int)reason);
}

void onMqttMessage(const espMqttClientTypes::MessageProperties& properties,
                   const char* topic,
                   const uint8_t* payload,
                   size_t len,
                   size_t index,
                   size_t total) {
  buildUpdateConfigsTopic();
  if (strcmp(topic, updateConfigsTopic) != 0) {
    return;
  }

  if (index == 0) {
    updateConfigsBufferLength = 0;
    updateConfigsBufferOverflow = false;
    updateConfigsBuffer[0] = '\0';
  }

  if (updateConfigsBufferOverflow ||
      updateConfigsBufferLength + len >= sizeof(updateConfigsBuffer)) {
    updateConfigsBufferOverflow = true;
  } else {
    memcpy(updateConfigsBuffer + updateConfigsBufferLength, payload, len);
    updateConfigsBufferLength += len;
    updateConfigsBuffer[updateConfigsBufferLength] = '\0';
  }

  if ((index + len) < total) {
    return;
  }

  if (updateConfigsBufferOverflow) {
    DBG_PRINTLN("Mensagem MQTT de configuração maior que o buffer.");
    updateConfigsBufferLength = 0;
    updateConfigsBufferOverflow = false;
    updateConfigsBuffer[0] = '\0';
    return;
  }

  DBG_PRINTF("Mensagem recebida [%s]: %s\n", topic, updateConfigsBuffer);

  cJSON* root = cJSON_Parse(updateConfigsBuffer);
  if (root == NULL) {
    DBG_PRINTLN("Erro ao analisar JSON recebido via MQTT.");
    updateConfigsBufferLength = 0;
    updateConfigsBuffer[0] = '\0';
    return;
  }

  cJSON* sensorIdJson = cJSON_GetObjectItem(root, "sensorId");
  if (!cJSON_IsString(sensorIdJson)) {
    DBG_PRINTLN("JSON MQTT sem sensorId válido.");
    cJSON_Delete(root);
    updateConfigsBufferLength = 0;
    updateConfigsBuffer[0] = '\0';
    return;
  }

  if (strcmp(id, sensorIdJson->valuestring) != 0) {
    DBG_PRINTLN(
        "Mensagem MQTT ignorada: sensorId não corresponde ao id do produto.");
    cJSON_Delete(root);
    updateConfigsBufferLength = 0;
    updateConfigsBuffer[0] = '\0';
    return;
  }

  applyConfigUpdateJson(root);
  if (isMqttReady() &&
      buildDeviceConfigurationsMessage(configPayload, sizeof(configPayload))) {
    publishMessageToMqtt(MQTT_SENSOR_CONFIGS, configPayload);
  }
  cJSON_Delete(root);
  updateConfigsBufferLength = 0;
  updateConfigsBuffer[0] = '\0';
}

void onMqttPublish(uint16_t packetId) {
  DBG_PRINTF("Publicação confirmada, packetId: %u\n", packetId);
}

// Setup
void setupMqtt() {
  configureMqttClient();
  lastMqttConnectAttempt = 0;
  maintainMqttConnection();
}

void maintainMqttConnection() {
  configureMqttClient();

  if (!isWiFiConnected) {
    isMqttConnected = false;
    return;
  }

  if (mqttClient.connected()) {
    isMqttConnected = true;
    return;
  }

  isMqttConnected = false;

  unsigned long now = millis();
  if (lastMqttConnectAttempt != 0 &&
      (now - lastMqttConnectAttempt) < MQTT_RECONNECT_INTERVAL_MS) {
    return;
  }

  lastMqttConnectAttempt = now;
  DBG_PRINTLN("Tentando conectar ao MQTT...");
  mqttClient.connect();
}

bool isMqttReady() {
  bool connected = mqttClient.connected();
  isMqttConnected = connected;
  return connected;
}

void publishMessageToMqtt(const char* topic, const char* payload) {
  if (!isMqttReady()) {
    DBG_PRINTLN("⚠️ MQTT não conectado, ignorando publicação.");
    return;
  }

  DBG_PRINTF("📤 Publicando - Tópico: %s, Payload: %s\n", topic, payload);

  uint16_t packetId =
      mqttClient.publish(topic, 0, false, payload);

  DBG_PRINTF("PacketId: %u\n", packetId);
}
