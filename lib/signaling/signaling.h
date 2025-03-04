#ifndef SIGNALING_H
#define SIGNALING_H

#include "define_pins.h"
#include "events_constants.h"
#include "global_variables.h"
#include "http_communication.h"

/**
 * @brief Verifica se o inclinômetro está em condição de bloqueio ou alerta
 */
void checkCurrentCondition();

/**
 * @brief Sinaliza condição de bloqueio com LED vermelho e buzzer piscando
 * rápido
 */
void signalBlock();

/**
 * @brief Sinaliza condição de alerta com LED vermelho e buzzer piscando devagar
 */
void signalAlert();

/**
 * @brief Tarefa responsável por gerenciar as sinalizações do inclinômetro
 */
void taskSignals(void* pvParameters);

#endif  // SIGNALING_H