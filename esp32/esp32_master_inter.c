// Calibração de Ganho:
//  -Fator de Correção de Ganho.

#include <Arduino.h>
//#include <stdio.h>
//#include <stdint.h>
//#include <string.h>

//#define THRESHOLD 100                                             // Threshold de tensão para iniciar a conversão ADC (25 = 0,5004V
#define ADC_MAX_VALUE 4095                                        // O ESP8266 tem um ADC de 12 bits
#define SIGNAL_PIN 4                                              // Pino (GPIO4) de sinal para acionar leitura do ADC
#define DAC_PIN 25 // DAC1 está no GPIO 25
#define ACIONA_PIN 34                                              // Pino para ler o valor de ACIONA (pino ADC exemplo)

// Calcula o valor para o DAC com base em uma tensão desejada de 3000 mV
// A tensão máxima do DAC é considerada como 3300 mV para este cálculo
#define TENSION_DESEJADA 2000
#define TENSION_MAXIMA 3300
#define THRESHOLD (TENSION_DESEJADA * 255) / TENSION_MAXIMA

uint16_t ACIONA = 2000;                                                 // Valor de ACIONA para acionamento (em mV)

bool coletaConcluida = false;                                     // Flag para verificar se a coleta foi concluída
volatile bool adcAcionado = false;                                // Flag para controlar o acionamento do ADC

// Interrupção que verifica se o valor de ACIONA foi atingido
void IRAM_ATTR verificaACIONA() {
  uint16_t valorACIONA = analogRead(ACIONA_PIN);                        // Lê o valor do pino ACIONA
  if (valorACIONA >= ACIONA) {                                     // Compara com o valor de ACIONA
    adcAcionado = true;                                            // Seta a flag para verdadeiro se atingido
  }
}
/*
Nota: IRAM_ATTR é usado para garantir que a função de interrupção seja colocada na RAM interna, 
o que é recomendado para funções que precisam ser executadas rapidamente.
*/

// Fator de Correção de Ganho
uint16_t lerADC_Calibrado(uint16_t pin) {
  uint16_t valorADC = analogRead(pin);                               // Lê o valor do ADC
  valorADC -= 5;                                                // Corrige o erro de offset
  float valorCalibrado = valorADC * 0.98;                       // Aplica o fator de correção de ganho
  return round(valorCalibrado);                                 // Retorna o valor calibrado e arredondado
}

void setup() {
  Serial.begin(9600);                                           // Inicializa a comunicação Serial padrão (UART0)
  Serial2.begin(9600, SERIAL_8N1, 16, 17);                      // Inicializa a UART2 com pinos 16 (RX) e 17 (TX) para comunicação externa

  pinMode(SIGNAL_PIN, INPUT);
  pinMode(ACIONA_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(SIGNAL_PIN), verificaACIONA, RISING); // Aciona a interrupção na borda de subida

  analogReadResolution(12);                                        // Configura a resolução do ADC para 12 bits
}

void loop() {

  //Serial.print("Valor DAC para 3000 mV: ");
  //Serial.println(Serial.print("Valor DAC para 3000 mV: ");
  //Serial.println(THRESHOLD););

  if (adcAcionado) {

    uint16_t gAdcResult = analogRead(A0);                       // Assume-se que a função lerADC_Calibrado() já aplica correções
    //if (gAdcResult > THRESHOLD) {
      // Envia o resultado do ADC via UART2
      Serial2.println(gAdcResult);
      Serial.printf("ADC: %d\n", gAdcResult);     
    //} else {
      // Opcionalmente, informa na UART0 que o valor é menor que o threshold
     // Serial.printf("Valor menor: %d\n", gAdcResult);
    }
    adcAcionado = false; // Reseta a flag
  }
//}
