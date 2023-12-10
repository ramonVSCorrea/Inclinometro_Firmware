//Tipos de eventos
#define EVT_BLOQUEIO 1             //"BLOQUEIO"
#define EVT_INICIO_BASCULAMENTO 2  //"INICIO BASCULAMENTO"
#define EVT_FIM_BASCULAMENTO 3     //"FIM BASCULAMENTO"
#define EVT_SENSOR_CONECTADO 4     //"SENSOR CONECTADO AO APP"
#define EVT_SENSOR_DESCONECTADO 5  //"SENSOR DESCONECTADO DO APP"
#define EVT_SENSOR_CALIBRADO 6     //"SENSOR CALIBRADO"
#define EVT_SENSOR_LIMPO 7         //"SENSOR LIMPO"
#define EVT_BLQ_ALTERADO 8         //"VALOR DE BLQ ALTERADO"

float AnguloLateral = 0;  // Variável que guarda o valor de ângulo lateral atual
float AnguloFrontal = 0;  // Variável que guarda o valor de ângulo frontal atual

float Angulo_BLQ_Front = 5;  // Variável que guarda o valor de ângulo de bloqueio frontal
float Angulo_BLQ_Lat = 3.5;  // Variável que guarda o valor de ângulo de bloqueio lateral

float Angulo_Calib_Lat = 0;    // Variável que guarda o valor de ângulo de calibração lateral
float Angulo_Calib_Front = 0;  // Variável que guarda o valor de ângulo de calibração frontal

bool Flag_BLQ = false;  // Flag para situação de bloqueio
bool Flag_ALT = false;  // Flag para situação de alerta

bool cmdSubir = false;   // Comando de subida da bascula
bool cmdDescer = false;  // Comando de descida da bascula

String data = "00/00/0000";  // String para guardar data
String hora = "00:00:00";    //String para guardar hora

unsigned char diaAtual = 0;
unsigned char mesAtual = 0;
unsigned int anoAtual = 0;

unsigned char horaAtual = 0;
unsigned char minutoAtual = 0;

bool flag_altera_data = false;

SemaphoreHandle_t i2cMutex = xSemaphoreCreateMutex();  // Mutex para I2C