#ifndef GLOBAL_VARIABLES_H
#define GLOBAL_VARIABLES_H

#include <Arduino.h>

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
extern String wifiSSID;
extern String wifiPassword;
extern bool isWiFiConfigChanged;

// HTTP variables
extern bool isHttpRequest;

// GPS variables
extern double latitude;
extern double longitude;

extern String id;

extern SemaphoreHandle_t httpMutex;

#endif  // GLOBAL_VARIABLES_H