#include <ESP32Servo.h>

#define BUTTON_UP 5    //Botão para realizar subida de bascula
#define BUTTON_DOWN 4  //Botão para realizar descida de Bascula
#define SERVO_PIN 13

//#define DBG_MSG_SERVO

Servo servo;

int posServo = 0;  //Variável que indica ângulo de inclinação do motor

/**
* @brief Tarefa que comanda o servo motor
*/
void Task_Servo(void* pvParameters) {
  //Inicializa os pinos de botão e servo
  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);
  servo.attach(SERVO_PIN);
  servo.write(0);

  while (1) {
    /**
    * Se BUTTON_UP for pressionado sobe a bascula se estiver baixa.
    * Durante o processo se entrar em condição de bloqueio ou
    * o usuário pressionar BUTTON_DOWN, ele para de elevar a bascula.
    */
    if (digitalRead(BUTTON_UP) == LOW || cmdSubir) {
#ifdef DBG_MSG_SERVO
      Serial.println("SUBINDO");
#endif
      if (posServo < 180) {
        while (posServo < 180) {
          if (Flag_BLQ || digitalRead(BUTTON_DOWN) == LOW || cmdDescer) break;

          servo.write(posServo);
          posServo++;

          vTaskDelay(20 / portTICK_PERIOD_MS);
        }
      }
      cmdSubir = false;
      cmdDescer = false;
    }

    /**
    * Se BUTTON_DOWN for pressionado desce a bascula se estiver alta.
    * Durante o processeo, se o usuário pressionar BUTTON_UP, a descida
    * da bascula irá parar.
    */
    else if (digitalRead(BUTTON_DOWN) == LOW || cmdDescer) {
#ifdef DBG_MSG_SERVO
      Serial.println("DESCENDO");
#endif
      if (posServo > 0) {
        while (posServo > 0) {
          if (digitalRead(BUTTON_UP) == LOW || cmdSubir) break;

          servo.write(posServo);
          posServo--;

          vTaskDelay(20 / portTICK_PERIOD_MS);
        }
      }
      cmdSubir = false;
      cmdDescer = false;
    }

    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}