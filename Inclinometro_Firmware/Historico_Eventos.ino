#include <FS.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>

//#define DBG_MSG_EVENTS

/**
* @brief Função para salvar evento na SPIFFS
* 
* @param evt Tipo do evento
*/
void Add_Event(int evt) {
  String evento = data + ";" + hora + ";";

  switch (evt) {
    case EVT_BLOQUEIO:
      evento += "BLOQUEIO;";
      break;

    case EVT_INICIO_BASCULAMENTO:
      evento += "INICIO BASCULAMENTO;";
      break;

    case EVT_FIM_BASCULAMENTO:
      evento += "FIM BASCULAMENTO;";
      break;

    case EVT_SENSOR_CONECTADO:
      evento += "SENSOR CONECTADO AO APP;";
      break;

    case EVT_SENSOR_DESCONECTADO:
      evento += "SENSOR DESCONECTADO DO APP;";
      break;

    case EVT_SENSOR_CALIBRADO:
      evento += "SENSOR CALIBRADO;";
      break;

    case EVT_SENSOR_LIMPO:
      evento += "SENSOR LIMPO;";
      break;

    case EVT_BLQ_ALTERADO:
      evento += "VALOR DE BLQ ALTERADO;";
      break;
  }

  evento += String(fabs(AnguloLateral)) + ";" + String(fabs(AnguloFrontal)) + "\n";

  /*
    Cria um arquivo contendo o conteúdo 
    do evento.
  */
  File fp = SPIFFS.open(FILE_EVENTS, "a");  //Abre o arquivo para escrita

  if (!fp) {
    Serial.println("Falha ao abrir arquivo");
  } else {
    if (fp.print(evento)) {
      //Serial.println("Evento Adicionado");
    } else {
      Serial.println("Erro ao gravar evento");
    }

    fp.close();
  }
#ifdef DBG_MSG_EVENTS
  Serial.print("Número de eventos: ");
  Serial.print(lerNumEventos());
#endif
}


/**
* @brief Função para ler número de eventos registrados
*
* @return Retorna o número de eventos
*/
int lerNumEventos() {
  File fp = SPIFFS.open(FILE_EVENTS, "r");

  if (fp) {
    unsigned int numEventos = 0;

    while (fp.available()) {
      String linha = fp.readStringUntil('\n');
#ifdef DBG_MSG_EVENTS
      Serial.println(linha);
#endif
      numEventos++;
    }
    fp.close();
    return numEventos - 1;
  } else {
    Serial.println("Erro ao abrir o arquivo");
    return 0;
  }
}


/**
* @brief Função para ler o conteúdo de um arquivo de um evento
*
* @param numEvento Número do evento que você deseja ler
* @return Conteúdo do arquivo do evento no formato JSON
*/
String lerEvento(int numEvento) {
  File fp = SPIFFS.open(FILE_EVENTS, "r");
  String linha;

  if (fp) {
    for (int i = 0; i <= numEvento; i++) {
      if (fp.available()) {
        linha = fp.readStringUntil('\n');
      }
    }
    fp.close();
    // Parse da linha de dados
    String data, hora, tipoEvento, angLat, angFront;

    int pos1 = linha.indexOf(';');
    data = linha.substring(0, pos1);

    int pos2 = linha.indexOf(';', pos1 + 1);
    hora = linha.substring(pos1 + 1, pos2);

    int pos3 = linha.indexOf(';', pos2 + 1);
    tipoEvento = linha.substring(pos2 + 1, pos3);

    int pos4 = linha.indexOf(';', pos3 + 1);
    angLat = linha.substring(pos3 + 1, pos4);
    angFront = linha.substring(pos4 + 1);

    // Criar objeto JSON
    DynamicJsonDocument doc(200);
    JsonObject evento = doc.createNestedObject("evento");
    evento["data"] = data;
    evento["hora"] = hora;
    evento["tipoEvento"] = tipoEvento;
    evento["AngLat"] = angLat;
    evento["AngFront"] = angFront;

    // Serializar o JSON para uma string
    String jsonString;
    serializeJson(doc, jsonString);

#ifdef DBG_MSG_EVENTS
    Serial.println(jsonString);
#endif

    return jsonString;
  } else {
    Serial.println("Erro ao abrir o arquivo");
    return "X";
  }
}
