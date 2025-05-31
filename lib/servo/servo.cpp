#include "servo.h"

Servo servoMotor;

float servoAngle = 0.0;  // Ângulo atual do servo motor

void taskServoMotor(void* pvParameters) {
  // Inicializa os pinos do botão e do servo
  servoMotor.attach(SERVO_PIN);
  servoMotor.write(0);

  while (1) {
    /**
     * Se BUTTON_UP for pressionado, a bascula sobe caso esteja baixa.
     * Durante a subida, se a condição de bloqueio ocorrer ou o usuário
     * pressionar BUTTON_DOWN, o movimento para.
     */
    if (commandRaise) {
#ifdef DEBUG_SERVO
      Serial.println("SUBINDO");
#endif
      if (servoAngle < 50) {
        while (servoAngle < 50) {
          if (isBlocked || commandLower)
            break;

          servoMotor.write(servoAngle);
          servoAngle += 0.2;
          vTaskDelay(10 / portTICK_PERIOD_MS);
        }
      }
      commandRaise = false;
      commandLower = false;

      while (isHttpRequest) {
        vTaskDelay(100 / portTICK_PERIOD_MS);
      }
      sendMessageToServer(buildEventPayload(EVENT_START_TIPPING,
                                            EVENT_START_TIPPING_DESCRIPTION));
    }

    /**
     * Se BUTTON_DOWN for pressionado, a bascula desce caso esteja alta.
     * Durante a descida, se o usuário pressionar BUTTON_UP, o movimento para.
     */
    else if (commandLower) {
#ifdef DEBUG_SERVO
      Serial.println("DESCENDO");
#endif
      if (servoAngle > 0) {
        while (servoAngle > 0) {
          if (commandRaise)
            break;

          servoMotor.write(servoAngle);
          servoAngle -= 0.2;
          vTaskDelay(10 / portTICK_PERIOD_MS);
        }
      }
      commandRaise = false;
      commandLower = false;

      while (isHttpRequest) {
        vTaskDelay(100 / portTICK_PERIOD_MS);
      }
      sendMessageToServer(
          buildEventPayload(EVENT_END_TIPPING, EVENT_END_TIPPING_DESCRIPTION));
    }

    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}
