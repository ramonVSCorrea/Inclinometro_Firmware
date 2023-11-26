#define LED_VERDE 25     //LED que indica condição segura de operação
#define LED_VERMELHO 27  //LED para indicar condição insegura (ALERTA e BLOQUEIO)
#define BUZZER 19

bool evt_blq = false;

/**
* @brief Função que verifica se o inclinômetro está em condição de bloqueio
*/
void verificaCondicaoAtual() {
  // Verifica se está em condição de bloqueio
  if (((AnguloLateral >= Angulo_BLQ_Lat) || (AnguloFrontal >= Angulo_BLQ_Front)) || ((AnguloLateral * (-1) >= Angulo_BLQ_Lat) || (AnguloFrontal * (-1) >= Angulo_BLQ_Front))) {
    Flag_BLQ = true;
    if (!evt_blq) {
      Add_Event(EVT_BLOQUEIO);
      evt_blq = true;
    }
  } else {
    Flag_BLQ = false;
    evt_blq = false;
  }

  // Verifica se está em condição de Alerta
  if (!Flag_BLQ && (((AnguloLateral >= Angulo_BLQ_Lat * 0.7) || (AnguloFrontal >= Angulo_BLQ_Front * 0.7)) || ((AnguloLateral * (-1) >= Angulo_BLQ_Lat * 0.7) || (AnguloFrontal * (-1) >= Angulo_BLQ_Front * 0.7)))) {
    Flag_ALT = true;
  } else {
    Flag_ALT = false;
  }
}


/**
* @brief Função que acende sinaliza Bloqueio  
*/
void sinalizaBloqueio() {
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_VERMELHO, HIGH);
  digitalWrite(BUZZER, HIGH);
  vTaskDelay(50 / portTICK_PERIOD_MS);
  digitalWrite(LED_VERMELHO, LOW);
  digitalWrite(BUZZER, LOW);
  vTaskDelay(50 / portTICK_PERIOD_MS);
}


/**
* @brief Função que acende sinaliza Alerta 
*/
void sinalizaAlerta() {
  digitalWrite(LED_VERMELHO, HIGH);
  digitalWrite(BUZZER, HIGH);
  vTaskDelay(200 / portTICK_PERIOD_MS);
  digitalWrite(LED_VERMELHO, LOW);
  digitalWrite(BUZZER, LOW);
  vTaskDelay(200 / portTICK_PERIOD_MS);
}


/**
* @brief Tarefa de sinalizações do inclinômetro
*/
void Task_Sinalizacoes(void* pvParameters) {
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  while (1) {
    verificaCondicaoAtual();

    if (Flag_BLQ)
      sinalizaBloqueio();
    else if (Flag_ALT)
      sinalizaAlerta();
    else {
      digitalWrite(LED_VERDE, HIGH);
      digitalWrite(LED_VERMELHO, LOW);
      digitalWrite(BUZZER, LOW);
    }
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}