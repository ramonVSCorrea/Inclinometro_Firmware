#include <SPIFFS.h>
#include <FS.h>
#include <cJSON.h>
#include <ArduinoJson.h>

#define DBG_MSG_FILE

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
  // Abre o arquivo para escrita
  File fp = SPIFFS.open(FILE_CFG, FILE_WRITE);

  if (!fp) {
    Serial.println("Falha ao abrir arquivo");
    return false;
  }

  // Crie um objeto JSON para armazenar as configurações
  DynamicJsonDocument doc(256);  // Tamanho do buffer JSON

  // Crie um objeto JSON para as configurações
  JsonObject configs = doc.createNestedObject("configuracoes");
  configs["bloqueioLateral"] = 3.5;
  configs["bloqueioFrontal"] = 5;
  configs["calibracaoLateral"] = 0;
  configs["calibracaoFrontal"] = 0;

  // Serialize o JSON no arquivo
  if (serializeJson(doc, fp) == 0) {
    Serial.println("Falha ao serializar o JSON no arquivo");
    fp.close();
    return false;
  }

  Serial.println("Arquivo Criado!");

  // Feche o arquivo
  fp.close();

#ifdef DBG_MSG_FILE
  // Se você quiser exibir o conteúdo do arquivo no monitor serial
  fp = SPIFFS.open(FILE_CFG, FILE_READ);

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
  // Abra o arquivo para leitura
  File fp = SPIFFS.open(FILE_CFG, "r");

  if (!fp) {
    Serial.println("Falha ao abrir o arquivo");
    return;
  }

  // Obtenha o tamanho do arquivo
  size_t fileSize = fp.size();

  // Leia o conteúdo do arquivo em uma String
  String fileContent;
  fileContent.reserve(fileSize);
  while (fp.available()) {
    char c = fp.read();
    fileContent += c;
  }

  // Feche o arquivo
  fp.close();

  // Crie um objeto JSON para analisar o conteúdo
  DynamicJsonDocument doc(256);  // Tamanho do buffer JSON

  DeserializationError error = deserializeJson(doc, fileContent);

  if (error) {
    Serial.println("Falha ao analisar o JSON");
    return;
  }

  JsonObject configs = doc["configuracoes"];

  Angulo_BLQ_Lat = configs["bloqueioLateral"];
  Angulo_BLQ_Front = configs["bloqueioFrontal"];
}



/**
* Função que altera os valores de bloqueio do arquivo de configurações
*/
void Set_BLQ_Configs() {
  // Abra o arquivo para leitura e escrita
  File fp = SPIFFS.open(FILE_CFG, "r+");

  if (!fp) {
    Serial.println("Falha ao abrir o arquivo");
    return;
  }

  // Obtenha o tamanho do arquivo
  size_t fileSize = fp.size();

  // Leia o conteúdo do arquivo em uma String
  String fileContent;
  fileContent.reserve(fileSize);
  while (fp.available()) {
    char c = fp.read();
    fileContent += c;
  }

  // Crie um objeto JSON para analisar o conteúdo
  DynamicJsonDocument doc(256);  // Tamanho do buffer JSON

  DeserializationError error = deserializeJson(doc, fileContent);

  if (error) {
    Serial.println("Falha ao analisar o JSON");
    fp.close();
    return;
  }

  JsonObject configs = doc["configuracoes"];

  // Modifique os valores no JSON
  configs["bloqueioLateral"] = Angulo_BLQ_Lat;
  configs["bloqueioFrontal"] = Angulo_BLQ_Front;

  // Volte ao início do arquivo para escrita
  fp.seek(0);

  // Serialize o JSON no arquivo
  if (serializeJson(doc, fp) == 0) {
    Serial.println("Falha ao serializar o JSON no arquivo");
  } else {
    Serial.println("JSON modificado e salvo no arquivo.");
  }

  // Feche o arquivo
  fp.close();
}
