/**
 * @file main.cpp
 * 
 * Descrição:
 * Este código é destinado ao ESP32 e é utilizado para receber strings via UART,
 * enviadas por um microcontrolador externo. Após a recepção, a string é convertida
 * para um valor numérico inteiro, interpretado como um valor ADC.
 * 
 * Ambiente de Desenvolvimento: Arduino IDE
 * 
 * Dispositivo Alvo: ESP32
 * 
 * Autor: Diego Vergaças
 * 
 * Notas:
 * - Utiliza a Serial2 para comunicação UART com pinos RX e TX definidos respectivamente
 *   em RX_PIN e TX_PIN.
 * - A taxa de baud para a comunicação UART está configurada para 9600 bps.
 * - Este código espera por strings terminadas com um caractere de nova linha ('\n')
 *   e as converte para um valor numérico inteiro que é exibido no monitor serial.
 * - Adicionado um delay para facilitar a leitura dos dados no monitor serial.
 * 
 * Instruções:
 * - Certifique-se de conectar corretamente os pinos RX e TX do ESP32 aos pinos TX e RX
 *   do microcontrolador que está enviando os dados.
 * - Substitua RX_PIN e TX_PIN pelos números dos pinos que você está usando para a comunicação UART.
 * 
 * Revisões:
 * - [Data] [Autor] [Mudanças]
 * - 12/03/2024 [Seu Nome Aqui] - Criação do arquivo e implementação inicial.
 */

#include <HardwareSerial.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define RX_PIN 16          // Substitua pelo seu pino RX
#define TX_PIN 17          // Substitua pelo seu pino TX

void setup() {
  // Inicia a Serial padrão para comunicação com o Serial Monitor
  Serial.begin(9600);

  // Configura e inicia a Serial2 com os pinos RX e TX definidos
  Serial2.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);

  Serial.println("Pronto para receber dados na Serial2...");
}

void loop() {
  String readString; // Cria uma string para armazenar os caracteres recebidos

  // Verifica se há dados disponíveis para leitura na Serial2
  while (Serial2.available()) {
    char c = Serial2.read();  // Lê um caractere
    if (c == '\n') { // Checa se é o fim da linha (ou fim da transmissão da string)
      break; // Sai do loop se for o fim da linha
    }
    readString += c; // Adiciona o caractere lido à string
    delay(1);  // Pequeno delay para permitir a chegada de mais dados
  }

  if (readString.length() > 0) {
    Serial.print("String recebida: ");
    Serial.println(readString); // Exibe a string recebida

    // Converte a string para um número inteiro
    int valorADC = readString.toInt();
    Serial.print("Valor ADC: ");
    Serial.println(valorADC); // Exibe o valor convertido
  }

  // Delay opcional para reduzir a velocidade do loop e facilitar a visualização
  delay(100);
}
