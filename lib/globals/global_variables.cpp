#include "global_variables.h"

// Angle variables
float lateralAngle = 0.0;
float frontalAngle = 0.0;

// Block angle variables
float blockLateralAngle = 5;
float blockFrontalAngle = 3.5;

// Calibration variables
float calibrateLateralAngle = 0.0;
float calibrateFrontalAngle = 0.0;

// Alert variables
bool isBlocked = false;
bool isAlert = false;

// Command variables
bool commandRaise = false;
bool commandLower = false;

// WiFi variables
bool isWiFiConnected = false;
char wifiSSID[WIFI_SSID_MAX_LENGTH] = "LIVE TIM_0820_2G";
char wifiPassword[WIFI_PASSWORD_MAX_LENGTH] = "a3ehn6rep6";
bool isWiFiConfigChanged = false;
bool isMqttConnected = false;

// GPS variables
double latitude = 0.0;
double longitude = 0.0;

char id[SENSOR_ID_MAX_LENGTH] = "AAAA";
