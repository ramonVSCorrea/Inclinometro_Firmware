#ifndef DEBUG_LOG_H
#define DEBUG_LOG_H

#include <Arduino.h>

#ifdef DBG_SERIAL
#define DBG_PRINT(...) Serial.print(__VA_ARGS__)
#define DBG_PRINTLN(...) Serial.println(__VA_ARGS__)
#define DBG_PRINTF(...) Serial.printf(__VA_ARGS__)
#define DBG_WRITE(...) Serial.write(__VA_ARGS__)
#else
#define DBG_PRINT(...) \
  do {                 \
  } while (0)
#define DBG_PRINTLN(...) \
  do {                   \
  } while (0)
#define DBG_PRINTF(...) \
  do {                  \
  } while (0)
#define DBG_WRITE(...) \
  do {                 \
  } while (0)
#endif

#endif  // DEBUG_LOG_H
