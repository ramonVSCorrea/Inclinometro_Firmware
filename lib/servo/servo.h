#ifndef SERVO_H
#define SERVO_H

#include "ESP32Servo.h"
#include "define_pins.h"
#include "events_constants.h"
#include "global_variables.h"
#include "http_communication.h"

#define DEBUG_SERVO

/**
 * @brief Tarefa que controla o movimento do servo motor
 */
void taskServoMotor(void* pvParameters);

#endif  // SERVO_H