# Diagrama em blocos do firmware

```mermaid
flowchart TD
    A[Power on / Reset] --> B[setup()]
    B --> C[Serial.begin]
    B --> D[Cria mutex HTTP]
    B --> E[initializeFiles()]
    B --> F[initializeAccelerometerMPU6050()]
    B --> G[Cria tarefas FreeRTOS]

    E --> E1[SPIFFS inicia / cria / carrega configs]
    E1 --> E2[block angles, calibração, Wi-Fi]

    G --> H[taskWiFiConnection]
    G --> I[taskAccelerometerMPU6050]
    G --> J[taskSignals]
    G --> K[taskServoMotor]
    G --> L[taskBluetooth]
    G --> M[taskGPS]

    H --> H1[Conecta ao Wi-Fi]
    H1 --> H2[Se conectar: setupMqtt()]
    H1 --> H3[Se cair ou mudar config: reconecta]
    H2 --> N[MQTT conectado]

    I --> I1[Lê MPU6050]
    I1 --> I2[Atualiza lateralAngle / frontalAngle]
    I2 --> I3[A cada 10 s publica inclinação via MQTT]

    M --> M1[Lê NMEA do GPS]
    M1 --> M2[Atualiza latitude / longitude]

    L --> L1[Bluetooth conectado?]
    L1 -->|sim| L2[Lê comandos JSON do app]
    L2 --> L3[Leitura de ângulos]
    L2 --> L4[Requisita config]
    L2 --> L5[Atualiza bloqueio]
    L2 --> L6[Comando bascula subir/descer]
    L2 --> L7[Calibração do sensor]
    L2 --> L8[Atualiza Wi-Fi]
    L3 --> P[Variáveis globais / SPIFFS / MQTT]
    L4 --> P
    L5 --> P
    L6 --> K
    L7 --> P
    L8 --> H

    J --> J1[checkCurrentCondition()]
    J1 --> J2{Está em bloqueio?}
    J2 -->|sim| J3[LED vermelho + buzzer rápido]
    J2 -->|não| J4{Está em alerta?}
    J4 -->|sim| J5[LED vermelho + buzzer lento]
    J4 -->|não| J6[LED verde ligado]
    J1 --> J7[Se bloqueio novo: publica evento MQTT]

    K --> K1{commandRaise?}
    K1 -->|sim| K2[Move servo até subir]
    K1 -->|não| K3{commandLower?}
    K3 -->|sim| K4[Move servo até descer]
    K2 --> K5[Publica EVT_START_TIPPING]
    K4 --> K6[Publica EVT_END_TIPPING]

    N --> N1[Assina tópico de atualização de configs]
    N1 --> N2[onMqttMessage()]
    N2 --> N3[Valida sensorId]
    N3 --> N4[applyConfigUpdateJson()]
    N4 --> E1
    N4 --> H

    I3 --> O[buildInclinationDataPayload() / publishMessageToMqtt()]
    J7 --> O
    K5 --> O
    K6 --> O
    L2 --> O
```

## Leitura rápida do fluxo

1. O firmware inicializa SPIFFS, carrega as configurações e configura o MPU6050.
2. Depois cria tarefas paralelas para Wi-Fi, acelerômetro, sinalização, servo, Bluetooth e GPS.
3. O acelerômetro alimenta os ângulos globais; o GPS alimenta latitude e longitude.
4. A tarefa de sinalização avalia esses ângulos e aciona LED/buzzer e evento MQTT.
5. A tarefa do servo responde aos comandos de basculamento recebidos via Bluetooth.
6. O Bluetooth também altera bloqueio, calibração e Wi-Fi gravando em SPIFFS.
7. A conexão Wi-Fi habilita MQTT, que recebe atualizações remotas de configuração e também publica leituras e eventos.
