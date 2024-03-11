#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h> // Ou use #include <FS.h> e #include <FFat.h> dependendo do seu sistema de arquivos
#include <HardwareSerial.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define RX_PIN 16          // Define o pino GPIO 16 (RX2) como RX no ESP32
#define TX_PIN 17          // Define o pino GPIO 17 (TX2) como TX no ESP32
#define CONTROL_PIN 33    // Pino GPIO 33 (D33) de controle para acionamento do ADC
#define MAX_CHANNEL 4095

const char* ssid = "PC_Diego";
const char* password = "12345678";
WebServer server(80);

uint16_t channels[MAX_CHANNEL + 1] = {0};
bool coletaConcluida = false;
uint16_t inicioAcionamento = 0; // Tempo de início do acionamento
uint16_t tempoAcionamento = 0; // Tempo de acionamento em segundos

//----------------------------------------------------------

void enviarDadosViaSerial() {
  Serial.println("Valor ADC; Ocorrências");
  for (int i = 0; i <= MAX_CHANNEL; i++) {
    if (channels[i] > 0) {
      Serial.print(i);
      Serial.print("; ");
      Serial.println(channels[i]);
    }
  }
}

void salvarTXT() {
  File file = SPIFFS.open("/dados.txt", "w");
  if (!file) {
    Serial.println("Erro ao abrir o arquivo para escrita.");
    return;
  }

  file.println("Valor ADC; Ocorrências");
  for (int i = 0; i <= MAX_CHANNEL; i++) {
    if (channels[i] > 0) {
      file.print(i);
      file.print("; ");
      file.println(channels[i]);
    }
  }
  file.close();
  Serial.println("Dados salvos em /dados.txt");
}


//---------------------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN); // Configura Serial2 no ESP32 para usar como SoftwareSerial
  digitalWrite(CONTROL_PIN, HIGH); // Inicializa o pino de controle em HIGH
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi conectado.");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());

  if (!SPIFFS.begin(true)) {
    Serial.println("Erro ao inicializar o SPIFFS.");
  } else {
    Serial.println("SPIFFS inicializado com sucesso.");
  }

  server.on("/", HTTP_GET, []() {
    server.send(200, "text/plain", "Hello, world!");
  });

// Rota '/txt'
server.on("/txt", HTTP_GET, []() {
  if (SPIFFS.exists("/dados.txt")) {
    File file = SPIFFS.open("/dados.txt", "r");
    String content;
    while (file.available()) {
      content += char(file.read());
    }
    file.close();
    server.send(200, "text/plain; charset=UTF-8", content);
  } else {
    server.send(404, "text/plain", "Arquivo TXT não encontrado.");
  }
});

  server.begin();
}

//--------------------------------------------------------------------

void loop() {
    server.handleClient();

    if (Serial.available() > 0) {
        char receivedChar = Serial.read();

        // Reseta o sistema e solicita novo tempo se 'R' for recebido
        if (receivedChar == 'R' || receivedChar == 'r') {
            Serial.println("Sistema RESETADO:");
            Serial.println("Digite o tempo de acionamento em segundos:");
            while (!Serial.available()) {
                delay(100); // Espera a entrada do usuário
            }
            String input = Serial.readStringUntil('\n');
            tempoAcionamento = input.toInt() * 1000; // Converte o tempo para milissegundos
            Serial.println("Tempo atualizado. Digite 'C' para iniciar.");
            coletaConcluida = false;
        }

        // Inicia a coleta se 'C' for recebido
        else if (receivedChar == 'C' || receivedChar == 'c') {
            if (!coletaConcluida && tempoAcionamento > 0) {
                inicioAcionamento = millis();
                digitalWrite(CONTROL_PIN, LOW); // Inicia o acionamento
                Serial.println("Acionamento iniciado.");
            }
            else {
                Serial.println("Defina o tempo de acionamento primeiro.");
            }
        }
    }

    // Verifica se o período de acionamento acabou
    if (!coletaConcluida && millis() - inicioAcionamento >= tempoAcionamento) {
        digitalWrite(CONTROL_PIN, HIGH); // Desliga o pino de controle
        coletaConcluida = true;
        Serial.println("Acionamento concluído. Pino desativado.");
        enviarDadosViaSerial();
        salvarTXT();
        Serial.println("Coleta e salvamento concluídos.");
    }

    // Processa os dados recebidos de Serial2 se o acionamento estiver ativo
    if (!coletaConcluida && Serial2.available()) {
        String data = Serial2.readStringUntil('\n');
        data.trim();
        Serial.print("Dados recebidos: ");
        Serial.println(data);
        int valorADC = data.toInt();
        if (valorADC >= 0 && valorADC <= MAX_CHANNEL) {
            channels[valorADC]++;
        }
    }
}
