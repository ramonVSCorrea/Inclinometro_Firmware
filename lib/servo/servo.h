#ifndef SERVO_H
#define SERVO_H

#include "ESP32Servo.h"
#include "define_pins.h"
#include "global_variables.h"

#define DEBUG_SERVO

/**
 * @brief Tarefa que controla o movimento do servo motor
 */
void taskServoMotor(void* pvParameters);

#endif  // SERVO_H