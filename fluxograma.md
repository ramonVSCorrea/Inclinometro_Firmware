# Fluxograma de Funcionamento - Inclinômetro

```mermaid
flowchart TD
    %% Inicialização
    A((Power On / Reset)) --> B[setup()]
    B --> C[Inicializa Serial]
    B --> D[Cria Mutex para HTTP]
    B --> E[Inicializa SPIFFS e Arquivos]
    B --> F[Inicializa MPU6050]
    B --> G{Criação das Tarefas\n(FreeRTOS)}

    E -.-> E1[(Memória Interna\nSPIFFS)]
    E1 -.->|Carrega Configurações| E

    %% Tarefas
    G --> H[Task: Wi-Fi]
    G --> I[Task: Acelerômetro]
    G --> J[Task: Sinalização]
    G --> K[Task: Servo Motor]
    G --> L[Task: Bluetooth]
    G --> M[Task: GPS]

    %% Wi-fi e MQTT
    H --> H1{Conectou?}
    H1 -->|Sim| H2[Configura/Conecta MQTT]
    H1 -->|Não| H3[Tenta Reconectar]
    H2 --> N((MQTT\nConectado))
    N -.->|Recebe Configs| H2
    
    %% Acelerômetro
    I --> I1[Lê dados do MPU6050]
    I1 --> I2[Atualiza Ângulos:\nLateral e Frontal]
    I2 --> I3[Publica dados via MQTT\n a cada 10s]

    %% GPS
    M --> M1[Lê NMEA via SoftwareSerial]
    M1 --> M2[Atualiza Latitude/Longitude\nglobais]

    %% Bluetooth
    L --> L1{App Conectado?}
    L1 -->|Sim| L2[Parse de comandos JSON]
    L2 --> L3[Realiza comandos:\nbascula, leitura, calibração,\ntroca de regras, rede Wi-Fi]
    L3 -.->|Salva/Atualiza| E1
    L3 -.->|Manda Comandos| K

    %% Sinalização / Alertas
    J --> J1[Compara ângulos atuais\ncom limites salvos]
    J1 --> J2{Estado Atual}
    J2 -->|Bloqueio| J3[LED Vermelho + Buzzer Rápido]
    J2 -->|Alerta| J4[LED Vermelho + Buzzer Lento]
    J2 -->|Seguro| J5[LED Verde Ligado]
    J1 -.->|Se houver Bloqueio| N

    %% Servo Motor (Bascula)
    K --> K1{Comando de Subir?}
    K1 -->|Sim| K2[Move Servo para Acima\nPublica EVT_START_TIPPING]
    K1 -->|Não| K3{Comando de Descer?}
    K3 -->|Sim| K4[Move Servo para Abaixo\nPublica EVT_END_TIPPING]
    K2 -.-> N
    K4 -.-> N
    
    %% Output Centralizado MQTT
    I3 -.-> N
```

## Explicação do Fluxo

1. **Inicialização (`setup`)**: O microcontrolador (ESP) liga e carrega as configurações da memória persistente (SPIFFS). Isso envolve buscar dados de rede Wi-Fi salvas, limites de ângulo de bloqueio/alerta e calibração. O acelerômetro (MPU6050) inicia e preparam-se as *tasks* (tarefas).
2. **Tarefas em Paralelo**: Usa-se o **FreeRTOS** para rodar funções concorrentes:
   - **Acelerômetro**: Lê continuamente os eixos e calcula os ângulos reais do caminhão/veículo.
   - **GPS**: Coleta posicionamento real para anexar aos pacotes de dados.
   - **Sinalização**: Verifica a todo momento as variáveis globais de ângulo que vêm do acelerômetro, as compara com as "regras de alerta" em memória e aciona outputs visuais/sonoros (buzzer/LEDs) sem atrasar a leitura do sensor.
   - **Servo Motor**: Escuta eventos (geralmente do app via Bluetooth) mandando a báscula "subir" ou "descer", controla os pinos PWM do servo e relata o evento inicial/final ao sistema de mensagens.
   - **Bluetooth**: Mantém um servidor BLE aguardando o App do motorista se conectar para ler a inclinação num painel local, mandar comandos para o servo motor ou efetuar novas configurações.
   - **Conectividade Wi-Fi / MQTT**: Age de fundo para manter conexão com a internet e com o Broker. Quando o Wi-Fi está rodando, transmite dados telemetria a cada 10s e eventos críticos na hora que ocorrem (como risco de tombamento ou operações do servo).