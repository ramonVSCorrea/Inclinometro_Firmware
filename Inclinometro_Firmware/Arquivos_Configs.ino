#include <SPIFFS.h>
#include <FS.h>
#include <cJSON.h>
#include <ArduinoJson.h>

//#define DBG_MSG_FILE

#define FILE_CFG "/configs.json"
#define FILE_EVENTS "/events.csv"
#define FORMAT_SPIFFS_IF_FAILED true


/**
* Função que inicia o arquivo de configurações
*/
void Init_FILES() {


  SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED);  //Inicia a partição SPIFFS e formata caso falhar
  Serial.println("SPIFFS inicializado!");
  // SPIFFS.remove(FILE_CFG);
  // SPIFFS.remove(FILE_EVENTS);

  /**
    * Se o arquivo de configurações NÃO existe na 
    * partição, ele é criado.
    */
  if (!SPIFFS.exists(FILE_CFG)) {
    if (createFileConfig())
      Serial.println("Arquivo de configurações criado com sucesso!");
    else
      Serial.println("Erro ao criar arquivo de configurações!");
  }

  /**
  *  Se o arquivo de eventos NÃO existe na 
  *  partição, ele é criado.
  */
  if(!SPIFFS.exists(FILE_EVENTS)){
    if(createFileEvents())
      Serial.println("Arquivo de Eventos criado com sucesso!");
    else
     Serial.println("Erro ao criar aquivo de eventos!");
  }

  Init_Configs();  //Inicia as variáveis globais
}


/**
* Função que cria um arquivo JSON na partição
* SPIFFS do ESP32. Modelo do arquivo:
*
* {
*	 "configuracoes":	{
*		 "bloqueioLateral":	3.5,
*		 "bloqueioFrontal":	5,
*		 "calibracaoLateral":	0,
*		 "calibracaoFrontal":	0
*	 }
* }
*
* @return Retorna true ou false
*/

bool createFileConfig() {
  File fp = SPIFFS.open(FILE_CFG, FILE_WRITE);  //Abre o arquivo para escrita

  if (!fp) {
    Serial.println("Falha ao abrir arquivo");
    return false;
  }

  //Escreve o texto no arquivo utilizando a biblioteca cJSON.h
  cJSON* root = cJSON_CreateObject();

  cJSON* configs = cJSON_CreateObject();

  cJSON_AddNumberToObject(configs, "bloqueioLateral", 3.5);
  cJSON_AddNumberToObject(configs, "bloqueioFrontal", 5);
  cJSON_AddNumberToObject(configs, "calibracaoLateral", 0);
  cJSON_AddNumberToObject(configs, "calibracaoFrontal", 0);

  cJSON_AddItemToObject(root, "configuracoes", configs);

  if (fp.print(cJSON_Print(root))) {
    Serial.println("Arquivo Criado!");
  } else {
    Serial.println("Erro ao gravar arquivo");
    cJSON_Delete(root);
    fp.close();
    return false;
  }

  cJSON_Delete(root);

  fp.close();

#ifdef DBG_MSG_FILE
  fp = SPIFFS.open(FILE_CFG);

  while (fp.available()) {
    Serial.write(fp.read());
  }

  fp.close();
#endif

  return true;
}


/**
* Função que inicia os valores de configurações
* do inclinômetro.
*/

void Init_Configs() {
  File fp = SPIFFS.open(FILE_CFG);

  String conteudo = fp.readString();
  const char* fileContent = conteudo.c_str();

  fp.close();

  cJSON* root = cJSON_Parse(fileContent);

  if (cJSON_IsObject(root)) {
    cJSON* configs = cJSON_GetObjectItem(root, "configuracoes");

    cJSON* node = cJSON_GetObjectItem(configs, "bloqueioLateral");
    Angulo_BLQ_Lat = cJSON_GetNumberValue(node);

    node = cJSON_GetObjectItem(configs, "bloqueioFrontal");
    Angulo_BLQ_Front = cJSON_GetNumberValue(node);

    node = cJSON_GetObjectItem(configs, "calibracaoLateral");
    Angulo_Calib_Lat = cJSON_GetNumberValue(node);

    node = cJSON_GetObjectItem(configs, "calibracaoFrontal");
    Angulo_Calib_Front = cJSON_GetNumberValue(node);
  }

  cJSON_Delete(root);
}



/**
* Função que altera os valores de bloqueio do arquivo de configurações
*/
void Set_BLQ_Configs() {
  File fp = SPIFFS.open(FILE_CFG, "r");

  String conteudo = fp.readString();
  const char* fileContent = conteudo.c_str();

  fp.close();

  fp = SPIFFS.open(FILE_CFG, "w");

  cJSON* root = cJSON_Parse(fileContent);

  if (cJSON_IsObject(root)) {
    cJSON* configs = cJSON_GetObjectItem(root, "configuracoes");

    if (cJSON_IsObject(configs)) {
      cJSON* node = cJSON_GetObjectItem(configs, "bloqueioLateral");
      cJSON_SetNumberValue(node, Angulo_BLQ_Lat);

      node = cJSON_GetObjectItem(configs, "bloqueioFrontal");
      cJSON_SetNumberValue(node, Angulo_BLQ_Front);
    }
  }

  if (fp.print(cJSON_Print(root))) {
    Serial.println("Arquivo Alterado!");
  } else {
    Serial.println("Erro ao alterar arquivo");
  }

  cJSON_Delete(root);
  fp.close();
  Init_Configs();
}

/**
* Função que altera os valores de calibração do sensor no arquivo
*/
void Set_Calib_Configs() {
  File fp = SPIFFS.open(FILE_CFG, "r");

  String conteudo = fp.readString();
  const char* fileContent = conteudo.c_str();

  fp.close();

  fp = SPIFFS.open(FILE_CFG, "w");

  cJSON* root = cJSON_Parse(fileContent);

  if (cJSON_IsObject(root)) {
    cJSON* configs = cJSON_GetObjectItem(root, "configuracoes");

    if (cJSON_IsObject(configs)) {
      cJSON* node = cJSON_GetObjectItem(configs, "calibracaoLateral");
      cJSON_SetNumberValue(node, Angulo_Calib_Lat);

      node = cJSON_GetObjectItem(configs, "calibracaoFrontal");
      cJSON_SetNumberValue(node, Angulo_Calib_Front);
    }
  }

  if (fp.print(cJSON_Print(root))) {
    Serial.println("Arquivo Alterado!");
    Serial.println(fp.readString());
  } else {
    Serial.println("Erro ao alterar arquivo");
  }

  cJSON_Delete(root);
  fp.close();
  Init_Configs();
}

bool createFileEvents(){
   File fp = SPIFFS.open(FILE_EVENTS, FILE_WRITE);  //Abre o arquivo para escrita

  if (!fp) {
    Serial.println("Falha ao abrir arquivo");
    return false;
  } 

  String titulosCsv = "Data;Hora;Evento;Lat;Front\n";

  if (fp.print(titulosCsv)) {
    Serial.println("Arquivo Criado!");
  } else {
    Serial.println("Erro ao gravar arquivo");
    fp.close();
    return false;
  }

  fp.close();
  return true;
}
