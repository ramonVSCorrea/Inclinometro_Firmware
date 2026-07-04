#include "bluetooth.h"
#include "debug_log.h"
#include "stack_debug.h"

bool evtConnected = false;

namespace {
bool bleDeviceConnected = false;
NimBLECharacteristic* responseCharacteristic = nullptr;
NimBLECharacteristic* readingsCharacteristic = nullptr;
NimBLECharacteristic* statusCharacteristic = nullptr;
char eventPayload[EVENT_MESSAGE_BUFFER_SIZE];
char configPayload[DEVICE_CONFIG_MESSAGE_BUFFER_SIZE];

void publishBluetoothEvent(const char* event) {
  if (isMqttReady() &&
      buildEventMessage(event, eventPayload, sizeof(eventPayload))) {
    publishMessageToMqtt(MQTT_EVENT_TOPIC, eventPayload);
  }
}

void publishBluetoothConfig() {
  if (isMqttReady() &&
      buildDeviceConfigurationsMessage(configPayload, sizeof(configPayload))) {
    publishMessageToMqtt(MQTT_SENSOR_CONFIGS, configPayload);
  }
}

void trimCommand(char* command) {
  size_t len = strlen(command);
  while (len > 0 &&
         (command[len - 1] == '\n' || command[len - 1] == '\r' ||
          command[len - 1] == ' ' || command[len - 1] == '\t')) {
    command[len - 1] = '\0';
    len--;
  }

  char* start = command;
  while (*start == ' ' || *start == '\t') {
    start++;
  }

  if (start != command) {
    memmove(command, start, strlen(start) + 1);
  }
}

bool getFieldValue(const char* params,
                   const char* field,
                   char* output,
                   size_t outputSize) {
  if (params == nullptr || field == nullptr || output == nullptr ||
      outputSize == 0) {
    return false;
  }

  const size_t fieldLen = strlen(field);
  const char* current = params;

  while (*current != '\0') {
    while (*current == ';') {
      current++;
    }

    if (strncmp(current, field, fieldLen) == 0 && current[fieldLen] == '=') {
      current += fieldLen + 1;
      const char* end = strchr(current, ';');
      size_t valueLen =
          end == nullptr ? strlen(current) : (size_t)(end - current);
      if (valueLen >= outputSize) {
        valueLen = outputSize - 1;
      }

      memcpy(output, current, valueLen);
      output[valueLen] = '\0';
      return true;
    }

    current = strchr(current, ';');
    if (current == nullptr) {
      break;
    }
    current++;
  }

  return false;
}

bool getFloatField(const char* params, const char* field, float* value) {
  char fieldValue[24];
  if (!getFieldValue(params, field, fieldValue, sizeof(fieldValue))) {
    return false;
  }

  char* end = nullptr;
  float parsedValue = strtof(fieldValue, &end);
  if (end == fieldValue || *end != '\0') {
    return false;
  }

  *value = parsedValue;
  return true;
}

bool getIntField(const char* params, const char* field, int* value) {
  char fieldValue[8];
  if (!getFieldValue(params, field, fieldValue, sizeof(fieldValue))) {
    return false;
  }

  char* end = nullptr;
  long parsedValue = strtol(fieldValue, &end, 10);
  if (end == fieldValue || *end != '\0') {
    return false;
  }

  *value = (int)parsedValue;
  return true;
}

void sendAck(const char* command) {
  char response[64];
  snprintf(response, sizeof(response), "ACK:cmd=%s;ok=1", command);
  sendBluetoothMessage(response);
}

void sendError(const char* command, const char* code) {
  char response[96];
  snprintf(response, sizeof(response), "ERR:cmd=%s;code=%s", command, code);
  sendBluetoothMessage(response);
}

void sendBluetoothConfig() {
  char response[BLE_MAX_MESSAGE_LENGTH];
  snprintf(response, sizeof(response),
           "CONFIG:blqLat=%.2f;blqFrt=%.2f;calLat=%.2f;calFrt=%.2f;ssid=%s",
           blockLateralAngle, blockFrontalAngle, calibrateLateralAngle,
           calibrateFrontalAngle, wifiSSID);
  sendBluetoothMessage(response);
}

void sendStatusMessage(NimBLECharacteristic* characteristic,
                       const char* message) {
  if (characteristic == nullptr) {
    return;
  }

  characteristic->setValue((uint8_t*)message, strlen(message));
  if (bleDeviceConnected) {
    characteristic->notify();
  }
}

void handleCommandWrite(const std::string& value) {
  if (value.empty()) {
    sendError("UNKNOWN", "invalid_payload");
    return;
  }

  char command[BLE_MAX_MESSAGE_LENGTH + 1];
  size_t commandLength = value.length();
  if (commandLength > BLE_MAX_MESSAGE_LENGTH) {
    commandLength = BLE_MAX_MESSAGE_LENGTH;
  }

  memcpy(command, value.data(), commandLength);
  command[commandLength] = '\0';
  trimCommand(command);

#ifdef DBG_MSG_BLUE
  DBG_PRINT("Comando BLE recebido: ");
  DBG_PRINTLN(command);
#endif

  processBluetoothCommand(command);
}

class IncliMaxServerCallbacks : public NimBLEServerCallbacks {
  void onConnect(NimBLEServer* server) {
    bleDeviceConnected = true;
    evtConnected = true;
    digitalWrite(YELLOW_LED_PIN, HIGH);
    sendBluetoothMessage("EVENT:type=SENSOR_CONNECTED");
    publishBluetoothEvent(EVT_SENSOR_CONNECTED);
  }

  void onConnect(NimBLEServer* server, NimBLEConnInfo& connInfo) {
    onConnect(server);
  }

  void onDisconnect(NimBLEServer* server) {
    bleDeviceConnected = false;
    evtConnected = false;
    digitalWrite(YELLOW_LED_PIN, LOW);
    publishBluetoothEvent(EVT_SENSOR_DISCONNECTED);
    NimBLEDevice::startAdvertising();
  }

  void onDisconnect(NimBLEServer* server, NimBLEConnInfo& connInfo, int reason) {
    onDisconnect(server);
  }
};

class CommandCallbacks : public NimBLECharacteristicCallbacks {
  void onWrite(NimBLECharacteristic* characteristic) {
    handleCommandWrite(characteristic->getValue());
  }

  void onWrite(NimBLECharacteristic* characteristic, NimBLEConnInfo& connInfo) {
    onWrite(characteristic);
  }
};
}  // namespace

void taskBluetooth(void* pvParameters) {
  unsigned long stackLogTimer = 0;

  pinMode(YELLOW_LED_PIN, OUTPUT);
  digitalWrite(YELLOW_LED_PIN, LOW);

  NimBLEDevice::init(BLE_DEVICE_NAME);
  NimBLEDevice::setPower(ESP_PWR_LVL_P9);
  NimBLEDevice::setMTU(185);

  NimBLEServer* server = NimBLEDevice::createServer();
  server->setCallbacks(new IncliMaxServerCallbacks());

  NimBLEService* service = server->createService(INCLIMAX_SERVICE_UUID);

  NimBLECharacteristic* commandCharacteristic = service->createCharacteristic(
      COMMAND_CHAR_UUID,
      NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR);
  commandCharacteristic->setCallbacks(new CommandCallbacks());

  responseCharacteristic = service->createCharacteristic(
      RESPONSE_CHAR_UUID, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);

  readingsCharacteristic = service->createCharacteristic(
      READINGS_CHAR_UUID, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);

  statusCharacteristic = service->createCharacteristic(
      STATUS_CHAR_UUID, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);

  responseCharacteristic->setValue("READY");
  readingsCharacteristic->setValue("READINGS:lat=0.00;frt=0.00");
  statusCharacteristic->setValue("STATUS:ready=1");

  NimBLEAdvertising* advertising = NimBLEDevice::getAdvertising();
  advertising->addServiceUUID(INCLIMAX_SERVICE_UUID);
  advertising->setName(BLE_DEVICE_NAME);
  advertising->start();

  DBG_PRINTLN("BLE NimBLE iniciado: " BLE_DEVICE_NAME);

  while (1) {
    logTaskStackHighWaterMark("Tarefa_Bluetooth", &stackLogTimer);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void sendBluetoothMessage(const char* message) {
  sendStatusMessage(responseCharacteristic, message);
}

void sendBluetoothReadings() {
  char response[96];
  snprintf(response, sizeof(response), "READINGS:lat=%.2f;frt=%.2f",
           lateralAngle, frontalAngle);

  sendStatusMessage(readingsCharacteristic, response);
  sendBluetoothMessage(response);
}

void readBluetoothData() {
  // No BLE com NimBLE, os comandos chegam por callback de escrita.
}

void processBluetoothCommand(const char* command) {
  if (command == nullptr || command[0] == '\0') {
    sendError("UNKNOWN", "invalid_payload");
    return;
  }

  if (strcmp(command, "GET_READINGS") == 0) {
    sendBluetoothReadings();
    return;
  }

  if (strcmp(command, "GET_CONFIG") == 0) {
    sendBluetoothConfig();
    return;
  }

  if (strcmp(command, "CALIBRATE") == 0) {
    calibrateLateralAngle += lateralAngle;
    calibrateFrontalAngle += frontalAngle;
    setCalibrationConfigs();
    publishBluetoothConfig();
    publishBluetoothEvent(EVT_SENSOR_CALIBRATED);
    sendAck("CALIBRATE");
    sendBluetoothMessage("EVENT:type=SENSOR_CALIBRATED");
    return;
  }

  if (strcmp(command, "CLEAR_CALIB") == 0) {
    calibrateLateralAngle = 0;
    calibrateFrontalAngle = 0;
    setCalibrationConfigs();
    publishBluetoothConfig();
    publishBluetoothEvent(EVT_SENSOR_CLEARED);
    sendAck("CLEAR_CALIB");
    sendBluetoothMessage("EVENT:type=SENSOR_CLEARED");
    return;
  }

  if (strncmp(command, "SET_BLOCK:", 10) == 0) {
    const char* params = command + 10;
    float newBlockLateralAngle = 0;
    float newBlockFrontalAngle = 0;

    if (!getFloatField(params, "lat", &newBlockLateralAngle) ||
        !getFloatField(params, "frt", &newBlockFrontalAngle)) {
      sendError("SET_BLOCK", "missing_value");
      return;
    }

    blockLateralAngle = newBlockLateralAngle;
    blockFrontalAngle = newBlockFrontalAngle;
    setBlockConfigs();
    publishBluetoothConfig();
    publishBluetoothEvent(EVT_BLOCK_VALUE_CHANGED);
    sendAck("SET_BLOCK");
    sendBluetoothMessage("EVENT:type=BLOCK_VALUE_CHANGED");
    return;
  }

  if (strncmp(command, "SET_WIFI:", 9) == 0) {
    const char* params = command + 9;
    char ssid[64];
    char password[64];

    if (!getFieldValue(params, "ssid", ssid, sizeof(ssid)) ||
        !getFieldValue(params, "pwd", password, sizeof(password))) {
      sendError("SET_WIFI", "missing_value");
      return;
    }

    snprintf(wifiSSID, sizeof(wifiSSID), "%s", ssid);
    snprintf(wifiPassword, sizeof(wifiPassword), "%s", password);
    setWiFiConfigs();
    publishBluetoothConfig();
    sendAck("SET_WIFI");
    return;
  }

  if (strncmp(command, "TIP:", 4) == 0) {
    const char* params = command + 4;
    int raise = 0;
    int lower = 0;

    if (!getIntField(params, "raise", &raise) ||
        !getIntField(params, "lower", &lower)) {
      sendError("TIP", "missing_value");
      return;
    }

    commandRaise = raise == 1;
    commandLower = lower == 1;
    sendAck("TIP");
    return;
  }

  sendError("UNKNOWN", "unknown_command");
}
