float AnguloLateral = 0;  // Variável que guarda o valor de ângulo lateral atual
float AnguloFrontal = 0;  // Variável que guarda o valor de ângulo frontal atual

float Angulo_BLQ_Front = 5;  // Variável que guarda o valor de ângulo de bloqueio frontal
float Angulo_BLQ_Lat = 3.5;  // Variável que guarda o valor de ângulo de bloqueio lateral

float Angulo_Calib_Lat = 0; // Variável que guarda o valor de ângulo de calibração lateral
float Angulo_Calib_Front = 0; // Variável que guarda o valor de ângulo de calibração frontal

bool Flag_BLQ = false;  // Flag para situação de bloqueio
bool Flag_ALT = false;  // Flag para situação de alerta

bool cmdSubir = false;   // Comando de subida da bascula
bool cmdDescer = false;  // Comando de descida da bascula