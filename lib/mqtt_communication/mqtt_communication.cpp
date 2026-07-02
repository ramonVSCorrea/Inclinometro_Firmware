#include "mqtt_communication.h"
#include "build_messages.h"
#include "global_variables.h"
#include "update_configs.h"

espMqttClientSecure mqttClient(espMqttClientTypes::UseInternalTask::YES);

namespace {
String updateConfigsBuffer;

String getUpdateConfigsTopic() {
  return String(MQTT_UPDATE_CONFIGS_PREFIX) + id + MQTT_UPDATE_CONFIGS_SUFFIX;
}
}

void onMqttConnect(bool sessionPresent) {
  Serial.println("Conectado ao MQTT!");

  String updateConfigsTopic = getUpdateConfigsTopic();
  mqttClient.subscribe(updateConfigsTopic.c_str(), 0);

  isMqttConnected = true;
}

void onMqttDisconnect(espMqttClientTypes::DisconnectReason reason) {
  isMqttConnected = false;
  Serial.printf("Desconectado do MQTT. Motivo: %d\n", (int)reason);
}

void onMqttMessage(const espMqttClientTypes::MessageProperties& properties,
                   const char* topic,
                   const uint8_t* payload,
                   size_t len,
                   size_t index,
                   size_t total) {
  String topicString = String(topic);

  if (topicString != getUpdateConfigsTopic()) {
    return;
  }

  if (index == 0) {
    updateConfigsBuffer = "";
  }

  for (size_t i = 0; i < len; i++) {
    updateConfigsBuffer += static_cast<char>(payload[i]);
  }

  if ((index + len) < total) {
    return;
  }

  Serial.printf("Mensagem recebida [%s]: %s\n", topic,
                updateConfigsBuffer.c_str());

  cJSON* root = cJSON_Parse(updateConfigsBuffer.c_str());
  if (root == NULL) {
    Serial.println("Erro ao analisar JSON recebido via MQTT.");
    updateConfigsBuffer = "";
    return;
  }

  cJSON* sensorIdJson = cJSON_GetObjectItem(root, "sensorId");
  if (!cJSON_IsString(sensorIdJson)) {
    Serial.println("JSON MQTT sem sensorId válido.");
    cJSON_Delete(root);
    updateConfigsBuffer = "";
    return;
  }

  if (id != String(sensorIdJson->valuestring)) {
    Serial.println(
        "Mensagem MQTT ignorada: sensorId não corresponde ao id do produto.");
    cJSON_Delete(root);
    updateConfigsBuffer = "";
    return;
  }

  applyConfigUpdateJson(root);
  publishMessageToMqtt(MQTT_SENSOR_CONFIGS, buildDeviceConfigurationsMessage());
  cJSON_Delete(root);
  updateConfigsBuffer = "";
}

void onMqttPublish(uint16_t packetId) {
  Serial.printf("Publicação confirmada, packetId: %u\n", packetId);
}

// Setup
void setupMqtt() {
  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.onPublish(onMqttPublish);

  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
  mqttClient.setClientId(MQTT_CLIENT_ID);
  mqttClient.setCredentials(MQTT_USERNAME, MQTT_PASSWORD);  // Se precisar de
  // autenticação
  mqttClient.setInsecure();

  mqttClient.connect();
}

void publishMessageToMqtt(String topic, String payload) {
  if (!mqttClient.connected()) {
    Serial.println("⚠️ MQTT não conectado, ignorando publicação.");
    return;
  }

  Serial.printf("📤 Publicando - Tópico: %s, Payload: %s\n", topic.c_str(),
                payload.c_str());

  uint16_t packetId =
      mqttClient.publish(topic.c_str(), 0, false, payload.c_str());

  Serial.printf("PacketId: %u\n", packetId);
}
