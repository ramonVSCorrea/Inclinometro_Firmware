# 📐 Firmware Inclinômetro IncliMax

Firmware embarcado para um **inclinômetro de caminhão basculante**, desenvolvido para ESP32. O sistema monitora a inclinação da báscula em tempo real, sinaliza situações de risco, bloqueia o acionamento quando os limites configurados são ultrapassados e envia dados para a nuvem via MQTT.

![Circuito do Inclinômetro](assets/circuito.png)

## 🚀 O Que Este Projeto Faz

O firmware transforma um ESP32 em um dispositivo IoT capaz de:

- 📏 Ler inclinação lateral e frontal usando o sensor **MPU6050**.
- 🚨 Identificar estados de alerta e bloqueio por ângulo configurável.
- 🔊 Acionar LEDs e buzzer para sinalização local.
- ⚙️ Controlar um servo motor para simular ou acionar a operação da báscula.
- 📍 Capturar localização via módulo GPS.
- 📡 Conectar ao Wi-Fi e publicar dados/eventos via MQTT.
- 🔵 Permitir configuração local via Bluetooth Low Energy com **NimBLE**.
- 💾 Persistir configurações em SPIFFS, mantendo dados após reinicialização.

## 🧠 Como Funciona

Na inicialização, o ESP32 monta o sistema de arquivos SPIFFS, carrega ou cria as configurações padrão e inicia tarefas FreeRTOS independentes para cada responsabilidade do sistema.

```text
ESP32
├── MPU6050              -> leitura dos ângulos lateral/frontal
├── GPS                  -> latitude e longitude
├── BLE NimBLE           -> configuração local pelo app
├── Wi-Fi + MQTT         -> envio de dados/eventos e recebimento de configs
├── SPIFFS               -> persistência das configurações
├── LEDs + buzzer        -> sinalização visual e sonora
└── Servo motor          -> controle da báscula
```

### 🔄 Fluxo Principal

1. O ESP32 inicializa SPIFFS e carrega configurações salvas.
2. O MPU6050 começa a medir inclinação lateral e frontal.
3. A task de sinalização verifica se o ângulo está normal, em alerta ou bloqueado.
4. A task Wi-Fi tenta conectar à rede configurada.
5. Ao conectar, o MQTT é iniciado e passa a publicar leituras e eventos.
6. O BLE fica disponível para leitura de dados, calibração, configuração de Wi-Fi e alteração dos limites de bloqueio.
7. O servo executa comandos de subida/descida, respeitando o estado de bloqueio.

## ✨ Funcionalidades

| Funcionalidade | Descrição |
|---|---|
| 📐 Medição de inclinação | Calcula os ângulos lateral e frontal a partir do MPU6050. |
| 🚨 Bloqueio automático | Bloqueia a operação quando o ângulo ultrapassa o limite configurado. |
| ⚠️ Alerta preventivo | Entra em alerta quando atinge 70% do limite de bloqueio. |
| 🔊 Sinalização local | Usa LED verde, LED vermelho, LED amarelo e buzzer. |
| 🔵 Configuração BLE | Permite configurar Wi-Fi, calibração, limites e comandos da báscula. |
| 📡 MQTT | Publica dados de inclinação, eventos e configurações do sensor. |
| 📍 GPS | Inclui latitude e longitude nos payloads publicados. |
| 💾 Persistência | Salva Wi-Fi, calibração e limites de bloqueio em SPIFFS. |
| 🧵 Multitarefa | Usa tasks FreeRTOS para executar módulos em paralelo. |

## 🛠️ Tecnologias Utilizadas

- **ESP32 Dev Module**
- **Arduino Framework**
- **PlatformIO**
- **FreeRTOS**
- **SPIFFS**
- **MQTT com TLS** usando `espMqttClient`
- **Bluetooth Low Energy** usando `NimBLE-Arduino`
- **MPU6050** usando `MPU6050_light`
- **GPS** usando `TinyGPSPlus`
- **Servo motor** usando `ESP32Servo`
- **cJSON** para manipulação de configurações JSON

## 📦 Bibliotecas Do PlatformIO

As dependências são instaladas automaticamente pelo PlatformIO a partir do arquivo `platformio.ini`:

```ini
lib_deps =
  https://github.com/rfetick/MPU6050_light.git
  madhephaestus/ESP32Servo@^3.0.6
  https://github.com/mikalhart/TinyGPSPlus.git
  https://github.com/bertmelis/espMqttClient.git
  h2zero/NimBLE-Arduino@^2.3.6
```

## 🔌 Hardware Necessário

- ESP32 Dev Module
- Sensor MPU6050
- Módulo GPS serial
- Servo motor
- LED verde
- LED vermelho
- LED amarelo
- Buzzer
- Jumpers, resistores e protoboard ou placa dedicada

## 🧷 Pinagem

| Componente | Pino ESP32 |
|---|---:|
| LED verde | GPIO 25 |
| LED vermelho | GPIO 27 |
| LED amarelo/BLE | GPIO 26 |
| Buzzer | GPIO 19 |
| Servo motor | GPIO 13 |
| GPS RX | GPIO 2 |
| GPS TX | GPIO 4 |
| MPU6050 SDA/SCL | Barramento I2C padrão do ESP32 |

## 📁 Estrutura Do Projeto

```text
.
├── src/
│   └── main.cpp                    # Inicialização das tasks principais
├── include/
│   ├── define_pins.h               # Definição dos pinos
│   ├── events_constants.h          # Eventos do sistema
│   └── debug_log.h                 # Macros de debug
├── lib/
│   ├── accelerometer/              # Leitura do MPU6050
│   ├── bluetooth/                  # Protocolo BLE NimBLE
│   ├── configs_manager/            # Persistência em SPIFFS
│   ├── gps/                        # Leitura do GPS
│   ├── messages/                   # Montagem de payloads MQTT
│   ├── mqtt_communication/         # Comunicação MQTT
│   ├── servo/                      # Controle da báscula/servo
│   ├── signaling/                  # LEDs, buzzer e estados de alerta
│   └── wifi/                       # Conexão e reconexão Wi-Fi
├── assets/
│   └── circuito.png                # Diagrama do circuito
├── partitions.csv                  # Partições do ESP32
├── platformio.ini                  # Configuração do PlatformIO
└── protocolo_bluetooth_nimble.md   # Documentação detalhada do BLE
```

## ▶️ Como Rodar

### 1. Pré-requisitos

- Instale o **Visual Studio Code**.
- Instale a extensão **PlatformIO IDE**.
- Instale o **Git**.
- Conecte o ESP32 ao computador por USB.

### 2. Clone O Repositório

```bash
git clone https://github.com/ICEI-PUC-Minas-EC-TI/pmg-ec-2025-1-p7-iotii-t1-inclinometro.git
cd pmg-ec-2025-1-p7-iotii-t1-inclinometro
```

### 3. Compile

```bash
pio run
```

### 4. Grave No ESP32

```bash
pio run --target upload
```

### 5. Abra O Monitor Serial

```bash
pio device monitor
```

O monitor serial usa `115200` baud, conforme configurado em `platformio.ini`.

## ⚙️ Primeira Inicialização

Na primeira execução, o firmware:

- Inicializa o SPIFFS.
- Cria o arquivo de configurações, caso ele ainda não exista.
- Define limites padrão de bloqueio.
- Inicializa o MPU6050.
- Inicia as tasks de Wi-Fi, acelerômetro, sinalização, servo, BLE e GPS.

Configurações padrão gravadas no SPIFFS:

| Configuração | Valor inicial |
|---|---:|
| Bloqueio lateral | `3.5` graus |
| Bloqueio frontal | `5.0` graus |
| Calibração lateral | `0.0` |
| Calibração frontal | `0.0` |

## 🔵 Comunicação Bluetooth BLE

O dispositivo anuncia como:

```text
IncliMax - AAAA
```

Serviço principal BLE:

```text
b8f8a000-2f3f-4a1b-9f70-000000000001
```

Characteristics:

| Characteristic | UUID | Uso |
|---|---|---|
| Command | `b8f8a000-2f3f-4a1b-9f70-000000000002` | App escreve comandos. |
| Response | `b8f8a000-2f3f-4a1b-9f70-000000000003` | ESP32 responde por read/notify. |
| Readings | `b8f8a000-2f3f-4a1b-9f70-000000000004` | Leituras por read/notify. |
| Status | `b8f8a000-2f3f-4a1b-9f70-000000000005` | Status do dispositivo. |

### 📲 Comandos BLE

| Comando | Função |
|---|---|
| `GET_READINGS` | Solicita ângulos atuais. |
| `GET_CONFIG` | Solicita configurações atuais. |
| `CALIBRATE` | Calibra usando os ângulos atuais como referência. |
| `CLEAR_CALIB` | Zera a calibração. |
| `SET_BLOCK:lat=4.00;frt=5.00` | Atualiza limites de bloqueio. |
| `SET_WIFI:ssid=MinhaRede;pwd=MinhaSenha` | Atualiza credenciais Wi-Fi. |
| `TIP:raise=1;lower=0` | Comanda subida da báscula. |
| `TIP:raise=0;lower=1` | Comanda descida da báscula. |

Exemplo de resposta:

```text
READINGS:lat=1.23;frt=0.45
```

Mais detalhes estão em [`protocolo_bluetooth_nimble.md`](protocolo_bluetooth_nimble.md).

## 📡 Comunicação MQTT

Quando o Wi-Fi está conectado, o firmware mantém conexão MQTT e publica mensagens JSON.

### Tópicos

| Tópico | Direção | Descrição |
|---|---|---|
| `inclimax/inclination-data` | ESP32 -> broker | Dados periódicos de inclinação e GPS. |
| `inclimax/event` | ESP32 -> broker | Eventos de bloqueio, Wi-Fi, calibração e báscula. |
| `inclimax/sensor-configs` | ESP32 -> broker | Configurações atuais do sensor. |
| `/inclimax/sensors/{sensorId}/update` | broker -> ESP32 | Atualização remota de configurações. |

### Exemplo De Payload De Inclinação

```json
{
  "sensorId": "AAAA",
  "lateralAngle": "1.23",
  "frontalAngle": "0.45",
  "latitude": "-19.922731",
  "longitude": "-43.945095"
}
```

### Eventos Publicados

- `BLOCK`
- `START_TIPPING`
- `END_TIPPING`
- `SENSOR_CONNECTED`
- `SENSOR_DISCONNECTED`
- `SENSOR_CALIBRATED`
- `SENSOR_CLEARED`
- `BLOCK_VALUE_CHANGED`
- `WIFI_CONNECTED`
- `WIFI_DISCONNECTED`

## 🚦 Estados De Sinalização

| Estado | Condição | Sinalização |
|---|---|---|
| Normal | Abaixo de 70% do limite | LED verde ligado. |
| Alerta | A partir de 70% do limite | LED vermelho e buzzer piscam lentamente. |
| Bloqueio | Limite lateral ou frontal ultrapassado | LED vermelho e buzzer piscam rapidamente. |
| BLE conectado | App conectado ao dispositivo | LED amarelo ligado. |

## 🧪 Verificação Rápida

Após gravar o firmware:

1. Abra o monitor serial com `pio device monitor`.
2. Confirme se o MPU6050 foi encontrado em `0x68` ou `0x69`.
3. Verifique se o BLE aparece como `IncliMax - AAAA` em um app BLE.
4. Envie `GET_READINGS` pela characteristic de comando.
5. Configure Wi-Fi com `SET_WIFI:ssid=...;pwd=...`.
6. Confira no serial se o Wi-Fi e o MQTT conectaram.

## 🧯 Troubleshooting

| Problema | Possível causa | Solução |
|---|---|---|
| MPU6050 não encontrado | Ligação I2C incorreta ou endereço diferente | Verifique VCC, GND, SDA, SCL e endereço `0x68/0x69`. |
| Wi-Fi não conecta | SSID/senha incorretos ou sinal fraco | Reenvie `SET_WIFI` via BLE e aproxime o ESP32 do roteador. |
| MQTT não publica | Wi-Fi desconectado ou broker indisponível | Verifique rede, credenciais e broker configurado no código. |
| BLE não aparece | Firmware não iniciou ou ESP32 travou | Reinicie o ESP32 e acompanhe o monitor serial. |
| GPS sem coordenadas | Sem fix de satélite | Teste em área aberta e aguarde alguns minutos. |

## 📚 Documentos Complementares

- [`protocolo_bluetooth_nimble.md`](protocolo_bluetooth_nimble.md) - protocolo BLE detalhado.
- [`diagrama_fluxo.md`](diagrama_fluxo.md) - diagrama/fluxo do funcionamento.
- [`fluxograma.md`](fluxograma.md) - visão de fluxo do projeto.

## 👥 Projeto

Projeto acadêmico desenvolvido no contexto do repositório **Inclinômetro**, com foco em IoT embarcado, telemetria, segurança operacional e comunicação entre dispositivo, aplicativo e plataforma remota.
