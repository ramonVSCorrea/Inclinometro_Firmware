#include "signaling.h"
#include "stack_debug.h"

bool isEventBlocked =
    false;  // Indica se um evento de bloqueio já foi registrado

namespace {
char eventPayload[EVENT_MESSAGE_BUFFER_SIZE];
}

void checkCurrentCondition() {
  // Verifica condição de bloqueio
  if (((lateralAngle >= blockLateralAngle) ||
       (frontalAngle >= blockFrontalAngle)) ||
      ((lateralAngle * (-1) >= blockLateralAngle) ||
       (frontalAngle * (-1) >= blockFrontalAngle))) {
    isBlocked = true;
    if (!isEventBlocked) {
      if (isMqttReady() &&
          buildEventMessage(EVT_BLOCK, eventPayload, sizeof(eventPayload))) {
        publishMessageToMqtt(MQTT_EVENT_TOPIC, eventPayload);
      }
      isEventBlocked = true;
    }
  } else {
    isBlocked = false;
    isEventBlocked = false;
  }

  // Verifica condição de alerta (70% do limite de bloqueio)
  if (!isBlocked && (((lateralAngle >= blockLateralAngle * 0.7) ||
                      (frontalAngle >= blockFrontalAngle * 0.7)) ||
                     ((lateralAngle * (-1) >= blockLateralAngle * 0.7) ||
                      (frontalAngle * (-1) >= blockFrontalAngle * 0.7)))) {
    isAlert = true;
  } else {
    isAlert = false;
  }
}

void signalBlock() {
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(RED_LED_PIN, HIGH);
  digitalWrite(BUZZER_PIN, HIGH);
  vTaskDelay(50 / portTICK_PERIOD_MS);
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  vTaskDelay(50 / portTICK_PERIOD_MS);
}

void signalAlert() {
  digitalWrite(RED_LED_PIN, HIGH);
  digitalWrite(BUZZER_PIN, HIGH);
  vTaskDelay(200 / portTICK_PERIOD_MS);
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  vTaskDelay(200 / portTICK_PERIOD_MS);
}

void taskSignals(void* pvParameters) {
  unsigned long stackLogTimer = 0;

  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  while (1) {
    logTaskStackHighWaterMark("Tarefa_Sinalizacoes", &stackLogTimer);

    checkCurrentCondition();

    if (isBlocked) {
      signalBlock();
    } else if (isAlert) {
      signalAlert();
    } else {
      digitalWrite(GREEN_LED_PIN, HIGH);
      digitalWrite(RED_LED_PIN, LOW);
      digitalWrite(BUZZER_PIN, LOW);
    }
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}
