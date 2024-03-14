

#include <Arduino.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>


void setup() {
  Serial.begin(9600);                                           // Inicializa a comunicação Serial padrão (UART0)
  Serial2.begin(9600, SERIAL_8N1, 16, 17);                      // Inicializa a UART2 com pinos GPIO16 (RX) e GPIO17 (TX) para comunicação externa
  analogReadResolution(12);                                     // Configura a resolução do ADC para 12 bits (A0)
}

//----------------------------------------------------------------------------
void limpeza() {
  while(Serial2.available() > 0) {
    char t = Serial2.read(); // Limpa o buffer
}
}

//---------------------------------------------------------------------------------
void sendData() {
  limpeza();
  Serial2.println("D"); // Indica o envio de dados numéricos
  int gAdcResult = analogRead(A0);
  Serial2.println(gAdcResult); // Envia o valor lido
  delay(10);
  Serial.printf("ADC: %d\n", gAdcResult);
  
  Serial2.println("F"); // Indica o fim do envio de dados numéricos
  Serial2.println("P"); // Pergunta se pode enviar mais dados
}

//-------------------------------------------------------------------------------

void loop() {
  char response = 'S'; // Assume que inicialmente pode enviar dados
  
  do {
    sendData(); // Chama a função que contém a lógica de envio dos dados
    
    unsigned long startTime = millis(); // Inicia a contagem para o timeout
    bool respostaRecebida = false; // Flag para verificar se a resposta foi recebida
    
    while (!respostaRecebida) {
      if (Serial2.available() > 0) {
        response = Serial2.read(); // Lê a resposta
        respostaRecebida = true;
        //Serial.print("Resposta recebida: "); // Depuração
        //Serial.println(response);
      } else if (millis() - startTime > 20) { // Timeout de 20 micro-segundos
        //Serial.println("Timeout esperando resposta.");
        respostaRecebida = true; // Sai do loop mesmo sem resposta
        response = 'N'; // Assume uma resposta que causa saída do loop do-while
      }
    }
  } while (response == 'S'); // Continua se a resposta for 'S'
  
  //delay(10); // Delay para controlar a velocidade de envio
}
