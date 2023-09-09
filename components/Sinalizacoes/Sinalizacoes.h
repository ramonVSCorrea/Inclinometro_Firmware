#include <driver/gpio.h>

#define LED_VERDE GPIO_NUM_25
#define LED_AMARELO GPIO_NUM_26
#define LED_VERMELHO GPIO_NUM_27
#define BUZZER GPIO_NUM_19

/**
 * @brief Tarefa que verifica sinalizações atuais do sistema
 */
void Task_Sinalizacoes();

/**
 * @brief Função que verifica qual a condição atual do sensor
 */
void verificaCondicaoAtual();

/**
 * @brief Função que sinaliza bloqueio através dos led e Buzzer
 */
void sinalizaBloqueio();

/**
 * @brief Função que sinaliza alerta através dos leds e Buzzer
 */
void sinalizaAlerta();
