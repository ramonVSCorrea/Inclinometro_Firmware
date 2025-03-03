#include "servo.h"

Servo servoMotor;

float servoAngle = 0.0;  // Ângulo atual do servo motor

void taskServoMotor(void* pvParameters) {
  // Inicializa os pinos do botão e do servo
  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);
  servoMotor.attach(SERVO_PIN);
  servoMotor.write(0);

  while (1) {
    /**
     * Se BUTTON_UP for pressionado, a bascula sobe caso esteja baixa.
     * Durante a subida, se a condição de bloqueio ocorrer ou o usuário
     * pressionar BUTTON_DOWN, o movimento para.
     */
    if (digitalRead(BUTTON_UP) == LOW || commandRaise) {
#ifdef DEBUG_SERVO
      Serial.println("SUBINDO");
#endif
      if (servoAngle < 50) {
        while (servoAngle < 50) {
          if (isBlocked || digitalRead(BUTTON_DOWN) == LOW || commandLower)
            break;

          servoMotor.write(servoAngle);
          servoAngle += 0.2;
          vTaskDelay(10 / portTICK_PERIOD_MS);
        }
      }
      commandRaise = false;
      commandLower = false;
    }

    /**
     * Se BUTTON_DOWN for pressionado, a bascula desce caso esteja alta.
     * Durante a descida, se o usuário pressionar BUTTON_UP, o movimento para.
     */
    else if (digitalRead(BUTTON_DOWN) == LOW || commandLower) {
#ifdef DEBUG_SERVO
      Serial.println("DESCENDO");
#endif
      if (servoAngle > 0) {
        while (servoAngle > 0) {
          if (digitalRead(BUTTON_UP) == LOW || commandRaise)
            break;

          servoMotor.write(servoAngle);
          servoAngle -= 0.2;
          vTaskDelay(10 / portTICK_PERIOD_MS);
        }
      }
      commandRaise = false;
      commandLower = false;
    }

    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}
