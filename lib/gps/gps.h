#ifndef GPS_H
#define GPS_H

#include <HardwareSerial.h>
#include <TinyGPS++.h>
#include "define_pins.h"
#include "global_variables.h"

#define GPS_BAUDRATE 9600

/**
 * @brief Tarefa que lê os dados do GPS e os imprime no monitor serial.
 *
 * @param pvParameters ponteiro para parâmetros a serem passados para a tarefa
 */
void taskGPS(void* pvParameters);

#endif  // GPS_H