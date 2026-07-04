#ifndef GLOBAL_VARIABLES_H
#define GLOBAL_VARIABLES_H

#include <Arduino.h>

// Tópicos MQTT
#define MQTT_EVENT_TOPIC "inclimax/event"
#define MQTT_DATA_TOPIC "inclimax/inclination-data"
#define MQTT_UPDATE_CONFIGS_PREFIX "/inclimax/sensors/"
#define MQTT_UPDATE_CONFIGS_SUFFIX "/update"
#define MQTT_SENSOR_CONFIGS "inclimax/sensor-configs"

#define EVT_BLOCK "BLOCK"
#define EVT_START_TIPPING "START_TIPPING"
#define EVT_END_TIPPING "END_TIPPING"
#define EVT_SENSOR_CONNECTED "SENSOR_CONNECTED"
#define EVT_SENSOR_DISCONNECTED "SENSOR_DISCONNECTED"
#define EVT_SENSOR_CALIBRATED "SENSOR_CALIBRATED"
#define EVT_SENSOR_CLEARED "SENSOR_CLEARED"
#define EVT_BLOCK_VALUE_CHANGED "BLOCK_VALUE_CHANGED"
#define EVT_WIFI_CONNECTED "WIFI_CONNECTED"
#define EVT_WIFI_DISCONNECTED "WIFI_DISCONNECTED"

#define WIFI_SSID_MAX_LENGTH 64
#define WIFI_PASSWORD_MAX_LENGTH 64
#define SENSOR_ID_MAX_LENGTH 16

// Angle variables
extern float lateralAngle;
extern float frontalAngle;

// Block angle variables
extern float blockLateralAngle;
extern float blockFrontalAngle;

// Calibration variables
extern float calibrateLateralAngle;
extern float calibrateFrontalAngle;

// Alert variables
extern bool isBlocked;
extern bool isAlert;

// Command variables
extern bool commandRaise;
extern bool commandLower;

// WiFi variables
extern bool isWiFiConnected;
extern char wifiSSID[WIFI_SSID_MAX_LENGTH];
extern char wifiPassword[WIFI_PASSWORD_MAX_LENGTH];
extern bool isWiFiConfigChanged;
extern bool isMqttConnected;

// GPS variables
extern double latitude;
extern double longitude;

extern char id[SENSOR_ID_MAX_LENGTH];

#endif  // GLOBAL_VARIABLES_H
