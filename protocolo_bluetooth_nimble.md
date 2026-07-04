# Protocolo Bluetooth BLE com NimBLE

Este documento descreve a proposta de substituição do protocolo atual via `BluetoothSerial` por um protocolo BLE usando `NimBLE-Arduino`.

O objetivo principal da mudança é reduzir o consumo de RAM do ESP32 para permitir o uso simultâneo de Bluetooth e MQTT com TLS.

## Contexto Atual

Hoje o firmware usa `BluetoothSerial`, que funciona como uma porta serial clássica SPP. O app envia mensagens JSON e o ESP32 responde com JSON usando `SerialBT.println()`.

Exemplos atuais:

```json
{"requisicaoLeitura":1}
```

```json
{"requisitaCfg":1}
```

```json
{"configuracoesBLQ":{"bloqueioLateral":4,"bloqueioFrontal":4}}
```

Esse modelo é simples, mas consome muita RAM porque usa Bluetooth Classic/SPP, `String`, JSON e buffers dinâmicos.

## Novo Modelo BLE

Com BLE, a comunicação deixa de ser uma porta serial e passa a usar GATT.

O app se conecta ao serviço BLE do sensor e interage com characteristics.

## Serviço BLE

Sugestão de UUIDs:

| Item | UUID | Uso |
|---|---|---|
| Serviço principal | `b8f8a000-2f3f-4a1b-9f70-000000000001` | Serviço IncliMax |
| Command characteristic | `b8f8a000-2f3f-4a1b-9f70-000000000002` | App escreve comandos |
| Response characteristic | `b8f8a000-2f3f-4a1b-9f70-000000000003` | ESP32 envia respostas por notify |
| Readings characteristic | `b8f8a000-2f3f-4a1b-9f70-000000000004` | ESP32 envia leituras por read/notify |
| Status characteristic | `b8f8a000-2f3f-4a1b-9f70-000000000005` | ESP32 envia status por read/notify |

## Formato Das Mensagens

Para economizar RAM, o protocolo deixa de usar JSON no Bluetooth e passa a usar texto curto.

Formato geral:

```text
TIPO:campo=valor;campo=valor
```

Comando sem parâmetros:

```text
TIPO
```

Regras:

- Mensagens devem usar ASCII.
- Campos são separados por `;`.
- Chave e valor são separados por `=`.
- Nomes de comandos e eventos devem ser estáveis.
- Valores booleanos devem usar `0` ou `1`.
- Valores numéricos devem usar ponto decimal.
- O app não deve depender da ordem dos campos.
- O ESP32 deve ignorar campos desconhecidos.

## Comandos Do App Para o ESP32

### Solicitar Leituras

Substitui o JSON atual:

```json
{"requisicaoLeitura":1}
```

Novo comando:

```text
GET_READINGS
```

Resposta esperada:

```text
READINGS:lat=1.23;frt=0.45
```

### Solicitar Configurações

Substitui o JSON atual:

```json
{"requisitaCfg":1}
```

Novo comando:

```text
GET_CONFIG
```

Resposta esperada:

```text
CONFIG:blqLat=4.00;blqFrt=4.00;calLat=0.00;calFrt=0.00;ssid=MinhaRede
```

Observação: por segurança, a senha Wi-Fi não deve ser enviada de volta para o app.

### Alterar Ângulos de Bloqueio

Substitui o JSON atual:

```json
{"configuracoesBLQ":{"bloqueioLateral":4,"bloqueioFrontal":4}}
```

Novo comando:

```text
SET_BLOCK:lat=4.00;frt=4.00
```

Ação no ESP32:

- Atualizar `blockLateralAngle`.
- Atualizar `blockFrontalAngle`.
- Persistir em SPIFFS com `setBlockConfigs()`.
- Publicar evento MQTT `EVT_BLOCK_VALUE_CHANGED` se MQTT estiver conectado.

Resposta esperada:

```text
ACK:cmd=SET_BLOCK;ok=1
```

### Calibrar Sensor

Substitui o JSON atual:

```json
{"configuraCalib":1}
```

Novo comando:

```text
CALIBRATE
```

Ação no ESP32:

- Somar os ângulos atuais em `calibrateLateralAngle` e `calibrateFrontalAngle`.
- Persistir com `setCalibrationConfigs()`.
- Publicar evento MQTT `EVT_SENSOR_CALIBRATED` se MQTT estiver conectado.

Resposta esperada:

```text
ACK:cmd=CALIBRATE;ok=1
```

### Zerar Calibração

Substitui o JSON atual:

```json
{"configuraCalib":0}
```

Novo comando:

```text
CLEAR_CALIB
```

Ação no ESP32:

- Definir `calibrateLateralAngle = 0`.
- Definir `calibrateFrontalAngle = 0`.
- Persistir com `setCalibrationConfigs()`.
- Publicar evento MQTT `EVT_SENSOR_CLEARED` se MQTT estiver conectado.

Resposta esperada:

```text
ACK:cmd=CLEAR_CALIB;ok=1
```

### Configurar Wi-Fi

Substitui o JSON atual:

```json
{"wifiConfigs":{"SSID":"MinhaRede","password":"MinhaSenha"}}
```

Novo comando:

```text
SET_WIFI:ssid=MinhaRede;pwd=MinhaSenha
```

Ação no ESP32:

- Atualizar `wifiSSID`.
- Atualizar `wifiPassword`.
- Persistir com `setWiFiConfigs()`.
- Sinalizar `isWiFiConfigChanged = true`.

Resposta esperada:

```text
ACK:cmd=SET_WIFI;ok=1
```

### Controlar Báscula

Substitui o JSON atual:

```json
{"comandoBascula":{"subir":1,"descer":0}}
```

Novo comando para subir:

```text
TIP:raise=1;lower=0
```

Novo comando para descer:

```text
TIP:raise=0;lower=1
```

Ação no ESP32:

- Atualizar `commandRaise`.
- Atualizar `commandLower`.
- A task do servo executa o movimento.

Resposta esperada:

```text
ACK:cmd=TIP;ok=1
```

## Mensagens Do ESP32 Para o App

### Leituras

```text
READINGS:lat=1.23;frt=0.45
```

Campos:

| Campo | Significado |
|---|---|
| `lat` | Ângulo lateral atual |
| `frt` | Ângulo frontal atual |

### Configurações

```text
CONFIG:blqLat=4.00;blqFrt=4.00;calLat=0.00;calFrt=0.00;ssid=MinhaRede
```

Campos:

| Campo | Significado |
|---|---|
| `blqLat` | Ângulo lateral de bloqueio |
| `blqFrt` | Ângulo frontal de bloqueio |
| `calLat` | Calibração lateral |
| `calFrt` | Calibração frontal |
| `ssid` | SSID Wi-Fi configurado |

### Eventos

```text
EVENT:type=BLOCK;lat=5.20;frt=1.10
```

```text
EVENT:type=SENSOR_CONNECTED
```

```text
EVENT:type=SENSOR_DISCONNECTED
```

```text
EVENT:type=SENSOR_CALIBRATED
```

```text
EVENT:type=SENSOR_CLEARED
```

```text
EVENT:type=BLOCK_VALUE_CHANGED
```

```text
EVENT:type=START_TIPPING
```

```text
EVENT:type=END_TIPPING
```

### Status Wi-Fi

```text
WIFI:status=connected;ip=192.168.1.17
```

```text
WIFI:status=disconnected
```

### Confirmação de Comando

```text
ACK:cmd=SET_BLOCK;ok=1
```

```text
ACK:cmd=SET_WIFI;ok=1
```

### Erros

```text
ERR:cmd=SET_BLOCK;code=missing_value
```

```text
ERR:cmd=SET_WIFI;code=invalid_payload
```

```text
ERR:cmd=UNKNOWN;code=unknown_command
```

Códigos sugeridos:

| Código | Significado |
|---|---|
| `unknown_command` | Comando não reconhecido |
| `missing_value` | Campo obrigatório ausente |
| `invalid_value` | Valor inválido |
| `invalid_payload` | Mensagem malformada |
| `internal_error` | Erro interno ao processar comando |

## Mapeamento do Protocolo Atual Para o Novo

| Função atual | JSON atual | Novo comando BLE |
|---|---|---|
| Solicitar leitura | `{"requisicaoLeitura":1}` | `GET_READINGS` |
| Solicitar configurações | `{"requisitaCfg":1}` | `GET_CONFIG` |
| Alterar bloqueio | `{"configuracoesBLQ":{"bloqueioLateral":4,"bloqueioFrontal":4}}` | `SET_BLOCK:lat=4.00;frt=4.00` |
| Calibrar | `{"configuraCalib":1}` | `CALIBRATE` |
| Zerar calibração | `{"configuraCalib":0}` | `CLEAR_CALIB` |
| Configurar Wi-Fi | `{"wifiConfigs":{"SSID":"MinhaRede","password":"MinhaSenha"}}` | `SET_WIFI:ssid=MinhaRede;pwd=MinhaSenha` |
| Subir báscula | `{"comandoBascula":{"subir":1,"descer":0}}` | `TIP:raise=1;lower=0` |
| Descer báscula | `{"comandoBascula":{"subir":0,"descer":1}}` | `TIP:raise=0;lower=1` |

## Limite de Tamanho e Fragmentação

BLE tem limite de tamanho por pacote. Com MTU padrão, o payload útil pode ser pequeno. Para simplificar a primeira versão do protocolo, cada mensagem deve ter no máximo 180 bytes.

Se futuramente for necessário enviar mensagens maiores, usar fragmentação:

```text
FRAG:id=1;idx=0;total=2;data=...
```

```text
FRAG:id=1;idx=1;total=2;data=...
```

Na primeira versão, evitar fragmentação e manter mensagens curtas.

## Recomendações Para Implementação no Firmware

- Usar `NimBLE-Arduino` no lugar de `BluetoothSerial`.
- Evitar `String` no parser BLE.
- Usar buffer fixo para mensagens recebidas, por exemplo `char command[180]`.
- Montar respostas com `snprintf`.
- Enviar respostas pela characteristic de notify.
- Não enviar senha Wi-Fi em respostas.
- Só publicar eventos MQTT se `isMqttConnected == true`.
- Manter comandos idempotentes sempre que possível.

## Exemplo de Fluxo

App solicita leitura:

```text
GET_READINGS
```

ESP32 responde:

```text
READINGS:lat=1.23;frt=0.45
```

App altera bloqueio:

```text
SET_BLOCK:lat=4.00;frt=4.00
```

ESP32 responde:

```text
ACK:cmd=SET_BLOCK;ok=1
```

App solicita configurações:

```text
GET_CONFIG
```

ESP32 responde:

```text
CONFIG:blqLat=4.00;blqFrt=4.00;calLat=0.00;calFrt=0.00;ssid=MinhaRede
```
