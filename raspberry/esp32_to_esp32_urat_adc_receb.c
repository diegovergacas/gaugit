// UART0 TX GPO pino 1
// UART RX GP1 pino 2

#include <Arduino.h>
#include <stdint.h>

uint16_t gAdcResult = 0;
uint16_t dataBuffer[4096]; // Buffer para armazenar os dados numéricos
int dataIndex = 0; // Índice para o buffer de dados

//------------------------------------------------------------------
void setup() {
  Serial.begin(9600); // Inicia a comunicação serial com o PC para depuração
  Serial1.begin(9600); // Inicia a UART1
  Serial.println("Setup completo. Aguardando dados do ESP32...");
}

//---------------------------------------------------------------------
void processIncomingData() {
  String dataString;
  while (true) {
    if (Serial1.available() > 0) {
      dataString = Serial1.readStringUntil('\n');
      if (dataString == "F") { // Verifica se é o fim dos dados numéricos
        Serial1.println("S"); // Indica ao ESP32 que está pronto para mais dados
        break; // Sai do loop interno
      } else {
        Serial.print("Valor recebido: ");
        uint16_t gAdcResult = (uint16_t)dataString.toInt(); // Converte para uint16_t
        Serial.println(gAdcResult); // Mostra o valor numérico recebido
        // Converte e armazena o dado no buffer
        if (dataIndex < sizeof(dataBuffer) / sizeof(dataBuffer[0])) { // Checa se há espaço no buffer
          dataBuffer[dataIndex++] = gAdcResult;
        } else {
          // Buffer cheio, implemente a lógica necessária (e.g., enviar dados, limpar buffer)
        }
      }
    }
  }
}

//-------------------------------------------------------------------------
void loop() {
  if (Serial1.available() > 0) {
    char messageType = Serial1.read();
    
    switch (messageType) {
      case 'D': // Início dos dados numéricos
        processIncomingData(); // Chama a função para processar os dados numéricos
        break;
      
      case 'P': // Pergunta se pode enviar mais dados
        Serial1.println("S"); // Responde que sim, pode enviar mais dados
        break;
      
      // Aqui você pode adicionar mais casos, se necessário
      default:
        // Comando não reconhecido
        break;
    }
  }
  // Aqui você pode implementar qualquer lógica de processamento ou envio dos dados acumulados
}
