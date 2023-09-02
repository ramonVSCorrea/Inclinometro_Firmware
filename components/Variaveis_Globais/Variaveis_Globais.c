#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

float AnguloLateral = 0;  // Variável que guarda o valor de ângulo lateral atual
float AnguloFrontal = 0;  // Variável que guarda o valor de ângulo frontal atual

float Angulo_BLQ_Front =
    5;  // Variável que guarda o valor de ângulo de bloqueio frontal
float Angulo_BLQ_Lat =
    3.5;  // Variável que guarda o valor de ângulo de bloqueio lateral

int Flag_BLQ = 0;  // Flag para situação de bloqueio
int Flag_ALT = 0;  // Flag para situação de alerta
