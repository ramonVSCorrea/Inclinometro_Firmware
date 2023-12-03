# Firmware Inclinômetro

Projeto que monitora inclinação de um caminhão basculante realizado no ESP32.

## Como configurar o Arduino IDE para compilar o programa:

### Instalação do ESP32 na IDE:

Abra o Arduino IDE, clique em "Arquivo" e depois em "Preferências".

Na opção "URLs adicionais para gerenciadores de Placas" entre com o seguinte link e clique em "OK": 
https://dl.espressif.com/dl/package_esp32_index.json

Clique em "Ferramentas / Placas / Gerenciador de Placas". Na janela do gerenciador de placas busque por "esp32".
Irá aparecer a opção "esp32 by Espressif Systems", instale ela.

Após instalado selecione a placa que você irá trabalhar clicando em "Ferramentas / Placa" e selecionando o modelo "ESP32 Dev Module"

Clique em "Ferramentas / Partition Scheme" e selecione a opção "No OTA (2MB APP / 2MB SPIFFS)".

### Inclusão de Bibliotecas:

Algumas bibliotecas tiveram que ser instaladas para utilizar os componentes necessários. Para instalar, clique em "Rascunho / Incluir Biblioteca / Gerenciar bibliotecas".
Abrindo uma janela do gerenciador de bibliotecas, instale as seguintes bibliotecas:

- ESP32Servo by Kevin Harrington;
- MPU6050_light by rfetick;
- RTClib by Adafruit;
- ArduinoJson by Benoit Blanchon;

## Circuito

A imagem a seguir mostra o esquema de ligação dos componentes.

<img src="/assets/Cicuito_inclinometro.png">
