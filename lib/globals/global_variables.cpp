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
String wifiSSID = "LIVE TIM_0820_2G";
String wifiPassword = "a3ehn6rep6";
bool isWiFiConfigChanged = false;

// HTTP variables
bool isHttpRequest = false;

// GPS variables
double latitude = 0.0;
double longitude = 0.0;

String id = "AAAA";

SemaphoreHandle_t httpMutex = NULL;
