#include "build_messages.h"

bool buildEventMessage(const char* event, char* buffer, size_t bufferSize) {
  int written = snprintf(buffer, bufferSize,
                         "{\"sensorId\":\"%s\",\"event\":\"%s\","
                         "\"lateralAngle\":\"%.2f\","
                         "\"frontalAngle\":\"%.2f\","
                         "\"latitude\":\"%.6f\",\"longitude\":\"%.6f\"}",
                         id, event, lateralAngle, frontalAngle,
                         latitude, longitude);

  return written > 0 && (size_t)written < bufferSize;
}

bool buildInclinationDataMessage(char* buffer, size_t bufferSize) {
  int written = snprintf(buffer, bufferSize,
                         "{\"sensorId\":\"%s\",\"lateralAngle\":\"%.2f\","
                         "\"frontalAngle\":\"%.2f\",\"latitude\":\"%.6f\","
                         "\"longitude\":\"%.6f\"}",
                         id, lateralAngle, frontalAngle, latitude,
                         longitude);

  return written > 0 && (size_t)written < bufferSize;
}

bool buildDeviceConfigurationsMessage(char* buffer, size_t bufferSize) {
  int written = snprintf(buffer, bufferSize,
                         "{\"sensorId\":\"%s\",\"angleConfigurations\":{"
                         "\"blockLateralAngle\":%.2f,"
                         "\"blockFrontalAngle\":%.2f,"
                         "\"calibrateLateralAngle\":%.2f,"
                         "\"calibrateFrontalAngle\":%.2f},"
                         "\"wifiConfigurations\":{\"ssid\":\"%s\","
                         "\"password\":\"%s\"}}",
                         id, blockLateralAngle, blockFrontalAngle,
                         calibrateLateralAngle, calibrateFrontalAngle,
                         wifiSSID, wifiPassword);

  return written > 0 && (size_t)written < bufferSize;
}
