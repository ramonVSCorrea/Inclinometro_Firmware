#ifndef ACCERLOMETER_H
#define ACCERLOMETER_H

#include <Arduino.h>
#include "http_communication.h"

/**
 * @brief Inicializa o acelerômetro MPU6050
 */
void initializeAccelerometerMPU6050();

/**
 * @brief Tarefa que atualiza os ângulos do acelerômetro MPU6050
 *
 * @param parameter
 */
void taskAccelerometerMPU6050(void* parameter);

#endif  // ACCERLOMETER_H