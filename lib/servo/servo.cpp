#include "servo.h"
#include "debug_log.h"
#include "stack_debug.h"

Servo servoMotor;

float servoAngle = 0.0;  // Ângulo atual do servo motor

namespace {
char eventPayload[EVENT_MESSAGE_BUFFER_SIZE];
}

void taskServoMotor(void* pvParameters) {
  unsigned long stackLogTimer = 0;

  // Inicializa os pinos do botão e do servo
  servoMotor.attach(SERVO_PIN);
  servoMotor.write(0);

  while (1) {
    logTaskStackHighWaterMark("Tarefa_Servo", &stackLogTimer);

    /**
     * Se BUTTON_UP for pressionado, a bascula sobe caso esteja baixa.
     * Durante a subida, se a condição de bloqueio ocorrer ou o usuário
     * pressionar BUTTON_DOWN, o movimento para.
     */
    if (commandRaise) {
#ifdef DEBUG_SERVO
      DBG_PRINTLN("SUBINDO");
#endif
      if (servoAngle < 50) {
        while (servoAngle < 50) {
          if (isBlocked || commandLower)
            break;

          servoMotor.write(servoAngle);
          servoAngle += 0.5;
          vTaskDelay(10 / portTICK_PERIOD_MS);
        }
      }
      commandRaise = false;
      commandLower = false;

      if (isMqttReady() && buildEventMessage(EVT_START_TIPPING, eventPayload,
                                             sizeof(eventPayload))) {
        publishMessageToMqtt(MQTT_EVENT_TOPIC, eventPayload);
      }
    }

    /**
     * Se BUTTON_DOWN for pressionado, a bascula desce caso esteja alta.
     * Durante a descida, se o usuário pressionar BUTTON_UP, o movimento para.
     */
    else if (commandLower) {
#ifdef DEBUG_SERVO
      DBG_PRINTLN("DESCENDO");
#endif
      if (servoAngle > 0) {
        while (servoAngle > 0) {
          if (commandRaise)
            break;

          servoMotor.write(servoAngle);
          servoAngle -= 0.5;
          vTaskDelay(10 / portTICK_PERIOD_MS);
        }
      }
      commandRaise = false;
      commandLower = false;

      if (isMqttReady() && buildEventMessage(EVT_END_TIPPING, eventPayload,
                                             sizeof(eventPayload))) {
        publishMessageToMqtt(MQTT_EVENT_TOPIC, eventPayload);
      }
    }

    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}
