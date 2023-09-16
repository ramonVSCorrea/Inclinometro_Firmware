#include <SPIFFS.h>
#include <FS.h>
#include <cJSON.h>

//#define DBG_MSG_FILES

#define FILE_CFG "/configs.json"


/**
* Função que inicia o arquivo de configurações
*/
void Init_FILES() {

  //Inicia a partição SPIFFS
  if (SPIFFS.begin(false)) {
    Serial.println("SPIFFS inicializado!");
    //SPIFFS.remove(FILE_CFG);

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

    Init_Configs();  //Inicia as variáveis globais
  }

  else {
    Serial.println("Falha na inicialização SPIFFS");
    while (1)
      ;
  }
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
    Angulo_BLQ_Lat = cJSON_GetNumberValue(node);
  }

  cJSON_Delete(root);
}