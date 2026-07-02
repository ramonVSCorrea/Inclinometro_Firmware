#include "build_messages.h"

String buildEventMessage(String event) {
  return "{"
         "\"sensorId\":\"" +
         id +
         "\","
         "\"event\":\"" +
         event +
         "\","
         "\"lateralAngle\":\"" +
         String(lateralAngle) +
         "\","
         "\"frontalAngle\":\"" +
         String(frontalAngle) +
         "\","
         "\"latitude\":\"" +
         String(latitude, 6) +
         "\","
         "\"longitude\":\"" +
         String(longitude, 6) +
         "\""
         "}";
}

String buildInclinationDataMessage() {
  return "{"
         "\"sensorId\":\"" +
         id +
         "\","
         "\"lateralAngle\":\"" +
         String(lateralAngle) +
         "\","
         "\"frontalAngle\":\"" +
         String(frontalAngle) +
         "\","
         "\"latitude\":\"" +
         String(latitude, 6) +
         "\","
         "\"longitude\":\"" +
         String(longitude, 6) +
         "\""
         "}";
}

String buildDeviceConfigurationsMessage() {
  return "{"
         "\"sensorId\":\"" +
         id +
         "\","
         "\"angleConfigurations\":{"
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
         "\"wifiConfigurations\":{"
         "\"ssid\":\"" +
         wifiSSID +
         "\","
         "\"password\":\"" +
         wifiPassword +
         "\""
         "}"
         "}";
}
