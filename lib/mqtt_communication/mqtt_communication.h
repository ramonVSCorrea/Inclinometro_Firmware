#ifndef MQTT_COMMUNICATION_H
#define MQTT_COMMUNICATION_H

#include <Arduino.h>
#include <espMqttClient.h>

#define MQTT_BASE_TOPIC "inclinometro"
#define MQTT_COMMAND_TOPIC "inclinometro/comandos"
#define MQTT_BROKER "3786eac24c584452a6b76d59c10725d8.s1.eu.hivemq.cloud"
#define MQTT_PORT 8883
#define MQTT_CLIENT_ID "inclimax_AAAA"
#define MQTT_USERNAME "sensor_inclimax"
#define MQTT_PASSWORD "Sensor1ncl1max"

/**
 * @brief Configura o cliente MQTT e estabelece a conexão
 *
 * @param sessionPresent Indica se a sessão MQTT já estava presente
 */
void onMqttConnect(bool sessionPresent);

/**
 * @brief Callback para lidar com a desconexão do MQTT
 *
 * @param reason Razão da desconexão, utilizando o enum DisconnectReason
 */
void onMqttDisconnect(espMqttClientTypes::DisconnectReason reason);

/**
 * @brief Callback para lidar com mensagens recebidas do MQTT
 *
 * @param properties Propriedades da mensagem, incluindo QoS, DUP, Retain e
 * Packet ID
 * @param topic Tópico da mensagem recebida
 * @param payload Payload da mensagem recebida
 * @param len Tamanho do payload
 * @param index Índice atual da mensagem (para mensagens fragmentadas)
 * @param total Tamanho total da mensagem (para mensagens fragmentadas)
 */
void onMqttMessage(const espMqttClientTypes::MessageProperties& properties,
                   const char* topic,
                   const uint8_t* payload,
                   size_t len,
                   size_t index,
                   size_t total);

/**
 * @brief Callback para lidar com a confirmação de publicação de mensagens MQTT
 *
 * @param packetId ID do pacote publicado, utilizado para rastrear a confirmação
 */
void onMqttPublish(uint16_t packetId);

/**
 * @brief Configura o cliente MQTT, define os callbacks e inicia a conexão
 */
void setupMqtt();

/**
 * @brief Mantém a conexão MQTT ativa, reconectando quando necessário.
 */
void maintainMqttConnection();

/**
 * @brief Indica se o cliente MQTT está conectado e pronto para publicar.
 */
bool isMqttReady();

/**
 * @brief Publica uma mensagem em um tópico MQTT específico
 *
 * @param topic Tópico onde a mensagem será publicada
 * @param payload Payload da mensagem a ser publicada
 */
void publishMessageToMqtt(const char* topic, const char* payload);

#endif  // MQTT_COMMUNICATION_H
