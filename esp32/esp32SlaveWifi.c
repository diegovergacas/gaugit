/*
 * Projeto de Monitoramento ADC com ESP32
 * Este código configura um ESP32 para coletar dados de ADC (Conversão Analógica-Digital),
 * processá-los e salvá-los em um arquivo no sistema de arquivos SPIFFS. Além disso,
 * disponibiliza os dados coletados através de uma interface web acessível pelo navegador.
 *
 * Dependências:
 * - Bibliotecas WiFi, WebServer, SPIFFS, e HardwareSerial para conexão Wi-Fi,
 *   criação de um servidor web, manipulação do sistema de arquivos e comunicação Serial, respectivamente.
 *
 * Pinos:
 * - RX_PIN e TX_PIN definidos para uso com HardwareSerial (Serial2) no ESP32 para comunicação serial.
 *
 * Funcionalidades:
 * - Conecta-se a uma rede Wi-Fi usando SSID e senha especificados.
 * - Inicializa o sistema de arquivos SPIFFS para armazenamento de dados.
 * - Cria um servidor web que responde à raiz ("/") com uma mensagem simples
 *   e à rota "/txt" para fornecer acesso ao arquivo de dados coletados.
 * - Coleta dados de ADC através do Serial2, processa esses dados e os salva em um arquivo no SPIFFS.
 * - Permite visualizar os dados coletados acessando "http://<Endereço_IP>/txt" em um navegador web.
 *
 * Instruções:
 * - Modifique as constantes 'ssid' e 'password' para corresponderem às suas credenciais de Wi-Fi.
 * - Ajuste os pinos RX_PIN e TX_PIN conforme a sua configuração de hardware.
 * - Carregue este código no seu ESP32.
 * - Acesse o servidor web através do IP fornecido no monitor serial para ver os dados coletados.
 */

#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h> // Ou use #include <FS.h> e #include <FFat.h> dependendo do seu sistema de arquivos
#include <HardwareSerial.h>

// Configuração dos pinos para comunicação Serial com um sensor ou módulo externo
#define RX_PIN 16          // Define o pino GPIO 16 (25) como RX no ESP32
#define TX_PIN 17          // Define o pino GPIO 17 (27) como TX no ESP32

// Limites e definições para processamento dos dados ADC
#define MAX_CHANNEL 4095

// Credenciais da rede Wi-Fi
const char* ssid = "*************";
const char* password = "*************";

// Instância do servidor web na porta 80
WebServer server(80);

// Estruturas para armazenamento e controle dos dados coletados
uint16_t channels[MAX_CHANNEL + 1] = {0};
uint16_t amostras = 20;
uint16_t sampleCount = 0;
bool coletaConcluida = false;

//-----------------------------------------------------------------------------

// Declaração de funções utilizadas

void enviarDadosViaSerial() {
  Serial.println("Valor ADC, Ocorrências");
  for (int i = 0; i <= MAX_CHANNEL; i++) {
    if (channels[i] > 0) {
      Serial.print(i);
      Serial.print(", ");
      Serial.println(channels[i]);
    }
  }
}

// Código para salvar dados em um arquivo no SPIFFS
void salvarTXT() {
  File file = SPIFFS.open("/dados.txt", "w");
  if (!file) {
    Serial.println("Erro ao abrir o arquivo para escrita.");
    return;
  }

  file.println("Valor ADC, Ocorrências");
  for (int i = 0; i <= MAX_CHANNEL; i++) {
    if (channels[i] > 0) {
      file.print(i);
      file.print(", ");
      file.println(channels[i]);
    }
  }
  file.close();
  Serial.println("Dados salvos em /dados.txt");
}

//-----------------------------------------------------------------------------

// Configuração inicial: Serial, Wi-Fi, SPIFFS e servidor web
void setup() {
  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN); // Configura Serial2 no ESP32 para usar como SoftwareSerial
  
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

//-----------------------------------------------------------------------------

// Lógica principal: Coleta de dados, processamento e resposta a requisições web
void loop() {
    server.handleClient();

    // Se a coleta foi concluída, não precisa processar novos dados de Serial2.
    if (coletaConcluida) {
        return;
    }

    if (Serial2.available()) {
        String data = Serial2.readStringUntil('\n'); // Lê a string até encontrar uma quebra de linha
        data.trim(); // Remove espaços em branco e a quebra de linha
        Serial.print("Dados recebidos: ");
        Serial.println(data); // Imprime os dados recebidos para depuração

        // Aqui assume-se que data é um valor ADC direto.
        int valorADC = data.toInt(); // Converte a string recebida para um inteiro

        // Verifica se o valor ADC está dentro do intervalo esperado e atualiza o array de contagem.
        if (valorADC >= 0 && valorADC <= MAX_CHANNEL) {
            channels[valorADC]++; // Incrementa a contagem para esse valor ADC.
        }

        // Incrementa o contador de amostras coletadas.
        sampleCount++;

        // Checa se coletou amostras suficientes.
        if (sampleCount >= amostras) {
            enviarDadosViaSerial(); // Ou salvarTXT(), dependendo de sua necessidade.
            salvarTXT(); // Isso deve acontecer somente após todos os dados serem coletados
            coletaConcluida = true; // Marca a coleta como concluída para não reexecutar.
            Serial.println("Coleta e salvamento concluídos.");
            delay(100);
        }
    }
}
