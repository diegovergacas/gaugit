/*
 * Projeto de Monitoramento ADC com Comunicação Serial
 * 
 * Este código foi desenvolvido para o ESP32 e realiza a leitura de valores analógicos 
 * através do ADC no pino A0. Os valores lidos são comparados a um limiar (threshold). 
 * Se o valor lido for maior que o limiar, ele é enviado através da UART2 para um 
 * dispositivo externo e também impresso na Serial padrão para depuração.
 * 
 * Dependências:
 * - Arduino.h para funcionalidades básicas do Arduino.
 * - stdio.h, stdint.h, e string.h para manipulação de strings e tipos de dados padrão.
 * 
 * Configurações:
 * - NUM_AMOSTRAS define o número total de amostras a serem lidas.
 * - THRESHOLD define o valor mínimo para que a leitura do ADC seja considerada válida.
 * - ADC_MAX_VALUE define o valor máximo que o ADC pode ler, baseado na resolução de 12 bits.
 * 
 * Funcionamento:
 * - Inicializa as comunicações Serial e Serial2 com configurações específicas.
 * - Realiza leituras contínuas do ADC no pino A0.
 * - Compara cada leitura com o valor de threshold definido.
 * - Se o valor lido for maior que o threshold, o valor é enviado via UART2 e impresso na Serial padrão.
 * - Inclui um delay entre leituras para limitar a frequência de amostragem.
 * 
 * Notas:
 * - Este código é uma demonstração básica e pode ser expandido ou modificado conforme necessário
 *   para atender a requisitos específicos de projetos.
 * - A UART2 é configurada para uso com pinos específicos do ESP32 (16 para RX e 17 para TX).
 */

#include <Arduino.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define NUM_AMOSTRAS 1000                                          // Valor máximo de amostras 300
#define THRESHOLD 100                                            // Threshold de tensão para iniciar a conversão ADC (25 = 0,5004V
#define ADC_MAX_VALUE 4095                                       // O ESP8266 tem um ADC de 12 bits

bool coletaConcluida = false;                                   // Flag para verificar se a coleta foi concluída

void setup() {
  Serial.begin(9600);                                           // Inicializa a comunicação Serial padrão (UART0)
  Serial2.begin(9600, SERIAL_8N1, 16, 17);                      // Inicializa a UART2 com pinos 16 (RX) e 17 (TX) para comunicação externa
  analogReadResolution(12);                                     // Configura a resolução do ADC para 12 bits
}


void loop() {
  //if (!coletaConcluida) {
    //for (uint16_t sampleCount = 0; sampleCount < NUM_AMOSTRAS; ++sampleCount) {
      int gAdcResult = analogRead(A0);                       // Lê valor do ADC do pino A0

      if (gAdcResult > THRESHOLD) {
        // Envia o resultado do ADC via UART2
        Serial2.println(gAdcResult);
        Serial.printf("ADC: %d\n", gAdcResult);
        //sampleCount++;
        
      } else {
        // Opcionalmente, informa na UART0 que o valor é menor que o threshold
        Serial.printf("Valor menor: %d\n", gAdcResult);
      }
      delay(50);                                            // Delay de 1 segundo entre leituras
    //}

  //coletaConcluida = true;                                 // Marca a coleta como concluída
  //}
}
