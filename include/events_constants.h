#ifndef EVENTS_CONSTANTS_H
#define EVENTS_CONSTANTS_H

// Tipos de eventos
#define EVENT_BLOCK "Bloqueio"
#define EVENT_BLOCK_DESCRIPTION                                          \
  "O basculamento foi bloqueado ao atingir / ultrapassar os ângulos de " \
  "bloqueio configurados."

#define EVENT_START_TIPPING "Início do Basculamento"
#define EVENT_START_TIPPING_DESCRIPTION \
  "O operador iniciou a operação de basculamento."

#define EVENT_END_TIPPING "Fim do Basculamento"
#define EVENT_END_TIPPING_DESCRIPTION \
  "A operação de basculamento foi finalizada."

#define EVENT_SENSOR_CONNECTED "Inclinômetro Conectado"
#define EVENT_SENSOR_CONNECTED_DESCRIPTION \
  "O inclinômetro foi conectado ao aplicativo."

#define EVENT_SENSOR_DISCONNECTED "Inclinômetro Desconectado"
#define EVENT_SENSOR_DISCONNECTED_DESCRIPTION \
  "O inclinômetro foi desconectado do aplicativo."

#define EVENT_SENSOR_CALIBRATED "Sensor Calibrado"
#define EVENT_SENSOR_CALIBRATED_DESCRIPTION "O sensor foi calibrado."

#define EVENT_SENSOR_CLEARED "Sensor Zerado"
#define EVENT_SENSOR_CLEARED_DESCRIPTION                                   \
  "Os valores de calibragem do sensor foram limpos, voltando aos valores " \
  "padrão."

#define EVENT_BLOCK_VALUE_CHANGED "Valores de Bloqueio Alterados"
#define EVENT_BLOCK_VALUE_CHANGED_DESCRIPTION \
  "Os valores dos ângulos de bloqueio foram alterados."

#define EVENT_WIFI_CONNECTED "Wi-Fi Conectado"
#define EVENT_WIFI_CONNECTED_DESCRIPTION \
  "O inclinômetro se conectou a rede Wi-Fi."

#define EVENT_WIFI_DISCONNECTED "Wi-Fi Desconectado"
#define EVENT_WIFI_DISCONNECTED_DESCRIPTION \
  "O inclinômetro se desconectou da rede Wi-Fi."

#endif  // EVENTS_CONSTANTS_H