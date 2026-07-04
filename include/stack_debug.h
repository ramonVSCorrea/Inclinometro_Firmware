#ifndef STACK_DEBUG_H
#define STACK_DEBUG_H

#include <Arduino.h>

inline void logTaskStackHighWaterMark(const char* taskName,
                                      unsigned long* lastLog,
                                      unsigned long intervalMs = 10000) {
#ifdef DBG_TASK_STACK
  const unsigned long now = millis();
  if ((now - *lastLog) < intervalMs) {
    return;
  }

  *lastLog = now;

  const UBaseType_t freeStackWords = uxTaskGetStackHighWaterMark(NULL);
  Serial.printf("[STACK] %s: %u words livres (%u bytes)\n", taskName,
                (unsigned int)freeStackWords,
                (unsigned int)(freeStackWords * sizeof(StackType_t)));
#else
  (void)taskName;
  (void)lastLog;
  (void)intervalMs;
#endif
}

#endif  // STACK_DEBUG_H
