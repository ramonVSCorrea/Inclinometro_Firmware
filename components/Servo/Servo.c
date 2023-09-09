#include "Servo.h"
#include <esp_log.h>
#include <stdio.h>
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Configurações do servo motor
// #define SERVO_MIN_PULSEWIDTH 1000  // Valor mínimo do pulso em microssegundos
// #define SERVO_MAX_PULSEWIDTH 2000  // Valor máximo do pulso em microssegundos
#define SERVO_MAX_DEGREE 180  // Ângulo máximo do servo

// // Configurações do MCPWM
// #define MCPWM_UNIT MCPWM_UNIT_0
// #define MCPWM_TIMER MCPWM_TIMER_0
// #define MCPWM_GPIO_NUM 0  // Pinagem do GPIO para o servo

char tag[] = "SERVO";

void servo_pwm_init() {
  ledc_timer_config_t timer_conf = {
      .speed_mode = LEDC_HIGH_SPEED_MODE,
      .duty_resolution = LEDC_TIMER_13_BIT,
      .timer_num = LEDC_TIMER_0,
      .freq_hz = SERVO_PWM_FREQ,
  };
  ledc_timer_config(&timer_conf);

  ledc_channel_config_t channel_conf = {
      .channel = SERVO_PWM_CHANNEL,
      .duty = 0,
      .gpio_num = SERVO_GPIO_PIN,
      .speed_mode = LEDC_HIGH_SPEED_MODE,
      .timer_sel = LEDC_TIMER_0,
  };
  ledc_channel_config(&channel_conf);
}

void set_servo_position(int degrees) {
  int duty =
      (SERVO_MIN_PULSE_WIDTH_US +
       degrees * (SERVO_MAX_PULSE_WIDTH_US - SERVO_MIN_PULSE_WIDTH_US) / 180);
  ledc_set_duty(LEDC_HIGH_SPEED_MODE, SERVO_PWM_CHANNEL, duty);
  ledc_update_duty(LEDC_HIGH_SPEED_MODE, SERVO_PWM_CHANNEL);
  ESP_LOGE(tag, "ESTOU AQUI");
}

void Task_Servo() {
  gpio_set_direction(BUTTON, GPIO_MODE_INPUT);
  gpio_set_pull_mode(BUTTON, GPIO_PULLUP_ONLY);

  int anguloServo = 0;
  // set_servo_position(anguloServo);

  while (1) {
    if (!gpio_get_level(BUTTON) && anguloServo < 180) {
      anguloServo++;
      set_servo_position(anguloServo);
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    // set_servo_position(0);
    // vTaskDelay(1000 / portTICK_PERIOD_MS);
    // set_servo_position(90);
    // vTaskDelay(1000 / portTICK_PERIOD_MS);
    // set_servo_position(180);
    // vTaskDelay(1000 / portTICK_PERIOD_MS);
    // // if (!gpio_get_level(BUTTON)) {
    // //   set_servo_position(90);
    // // }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}