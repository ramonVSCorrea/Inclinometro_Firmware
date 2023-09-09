#include "Sinalizacoes.h"
#include <driver/gpio.h>
#include <driver/i2c.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <math.h>
#include "Sensor_MPU6050.h"
#include "Variaveis_Globais.h"
#include "sdkconfig.h"

static char tag[] = "SINALIZAÇÔES";

void Task_Sinalizacoes() {
  // gpio_pad_select_gpio(LED_VERDE);
  gpio_set_direction(LED_VERDE, GPIO_MODE_OUTPUT);

  // gpio_pad_select_gpio(LED_AMARELO);
  gpio_set_direction(LED_AMARELO, GPIO_MODE_OUTPUT);

  // gpio_pad_select_gpio(LED_VERMELHO);
  gpio_set_direction(LED_VERMELHO, GPIO_MODE_OUTPUT);

  // gpio_pad_select_gpio(BUZZER);
  gpio_set_direction(BUZZER, GPIO_MODE_OUTPUT);

  while (1) {
    verificaCondicaoAtual();
    // ESP_LOGI(tag, "Flag_BLQ: %d, Flag_ALT: %d", Flag_BLQ, Flag_ALT);
    // ESP_LOGE(tag, "Lateral: %.2f, Frontal: %.2f", Angulo_BLQ_Lat,
    //          Angulo_BLQ_Front);

    if (Flag_BLQ)
      sinalizaBloqueio();
    else if (Flag_ALT)
      sinalizaAlerta();
    else {
      gpio_set_level(LED_VERDE, 1);
      gpio_set_level(LED_VERMELHO, 0);
      gpio_set_level(BUZZER, 0);
    }

    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void verificaCondicaoAtual() {
  // Verifica se está em condição de bloqueio
  if (((AnguloLateral >= Angulo_BLQ_Lat) ||
       (AnguloFrontal >= Angulo_BLQ_Front)) ||
      ((AnguloLateral * (-1) >= Angulo_BLQ_Lat) ||
       (AnguloFrontal * (-1) >= Angulo_BLQ_Front))) {
    Flag_BLQ = 1;
  } else {
    Flag_BLQ = 0;
  }

  // ESP_LOGE(tag, "Lateral: %.2f, Frontal: %.2f", Angulo_BLQ_Lat,
  //          Angulo_BLQ_Front);

  // Verifica se está em condição de Alerta
  if (!Flag_BLQ && (((AnguloLateral >= Angulo_BLQ_Lat * 0.7) ||
                     (AnguloFrontal >= Angulo_BLQ_Front * 0.7)) ||
                    ((AnguloLateral * (-1) >= Angulo_BLQ_Lat * 0.7) ||
                     (AnguloFrontal * (-1) >= Angulo_BLQ_Front * 0.7)))) {
    Flag_ALT = 1;
  } else {
    Flag_ALT = 0;
  }
}

void sinalizaBloqueio() {
  // ESP_LOGE(tag, "BLOQUEIO");
  gpio_set_level(LED_VERDE, 0);
  gpio_set_level(LED_VERMELHO, 1);
  gpio_set_level(BUZZER, 1);
  vTaskDelay(50 / portTICK_PERIOD_MS);
  gpio_set_level(LED_VERMELHO, 0);
  gpio_set_level(BUZZER, 0);
  vTaskDelay(50 / portTICK_PERIOD_MS);
}

void sinalizaAlerta() {
  // ESP_LOGW(tag, "ALERTA");
  gpio_set_level(LED_VERMELHO, 1);
  gpio_set_level(BUZZER, 1);
  vTaskDelay(500 / portTICK_PERIOD_MS);
  gpio_set_level(LED_VERMELHO, 0);
  gpio_set_level(BUZZER, 0);
}
