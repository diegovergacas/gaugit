/**
 * Código para Demonstração de ADC com Resolução de 8 Bits
 *
 * - Utiliza um Conversor Analógico-Digital (ADC) de 8 bits para leitura de dados.
 * - Armazena o espectro dos resultados em um array, formatando a saída em CSV.
 * - Calcula a média dos valores amostrados para análise de dados.
 * - Aplica um threshold (limiar) definido previamente, operando dentro de uma lógica booleana.
 *   O valor do threshold varia de 0 a 255, compatível com a resolução de 8 bits do ADC.
 *
 * Detalhes de Hardware:
 * - Pino de entrada do ADC: PA25 (PINOUT físico: 29), responsável pela leitura dos sinais analógicos.
 * - Pino de entrada do Amplificador Operacional (OPA): PA18 (PINOUT físico: 22), onde o sinal é inicialmente recebido.
 * - Pino de saída do OPA: PA16 (PINOUT físico: 20), que fornece o sinal amplificado para outras etapas do circuito.
 *
 * Este código é projetado para a coleta e análise de dados de ADC, facilitando a interpretação
 * dos resultados e a tomada de decisões baseada nos valores amostrados acima do threshold definido.
 * A especificação dos pinos de hardware ajuda na correta configuração e entendimento da interconexão dos componentes.
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "ti_msp_dl_config.h"

#define NUM_AMOSTRAS 10                                                                         // Valor máximo de amostras 300
#define THRESHOLD 0                                                                             // Threshold de tensão para iniciar a conversão ADC (25 = 0,5004V

volatile bool gCheckADC = false;
volatile uint16_t gAdcResult = 0;                                                               // Número de contagens que pode ter em cada canal
uint16_t adcArray[256] = {0};                                                                   // Array para contar as ocorrências de cada valor ADC


// Função para salvar amostras em CSV
static void salvarCSV(){
    const char *caminhoAbsoluto = "C:/Users/verga/OneDrive/Desktop/ADC_resultados/histograma.csv";
    FILE *file = fopen(caminhoAbsoluto, "w");
    if (file == NULL){
        printf("Erro ao abrir o arquivo.\n");
        return;
    }

    fprintf(file, "Valor ADC,Ocorrencias\n");
    for(int i = 0; i < 255; i++){
        fprintf(file, "%d,%d\n", i, adcArray[i]);
    }

    fclose(file);
}

// Função para limpar array
void limparArray(uint16_t array[], int tamanho) {
    memset(array, 0, tamanho * sizeof(uint16_t));
}

void ADC12_0_INST_IRQHandler(void) {
    switch (DL_ADC12_getPendingInterrupt(ADC12_0_INST)) {
        case DL_ADC12_IIDX_MEM0_RESULT_LOADED:
            gCheckADC = true;
            break;
        default:
            break;
    }
}

//--------------------------------------------------------------------

int main(void) {

    uint8_t sampleCount = 0;
    uint16_t amostras[NUM_AMOSTRAS];
    int soma = 0;
    int media;
    limparArray(amostras, NUM_AMOSTRAS);                                                        // Garante que o array esteja limpo antes de começar

    SYSCFG_DL_init();
    NVIC_EnableIRQ(ADC12_0_INST_INT_IRQN);                                                      // Inicialização ADC

    while (sampleCount < NUM_AMOSTRAS) {
        DL_ADC12_startConversion(ADC12_0_INST);                                                 // Inicialização da contagem
        while (!gCheckADC);                                                                     // Espera até a conversão estar pronta

        gAdcResult = DL_ADC12_getMemResult(ADC12_0_INST, DL_ADC12_MEM_IDX_0) & 0xFF;            // 8bits     256
        //gAdcResult = DL_ADC12_getMemResult(ADC12_0_INST, DL_ADC12_MEM_IDX_0) & 0x200;         // 10bits    512
        //gAdcResult = DL_ADC12_getMemResult(ADC12_0_INST, DL_ADC12_MEM_IDX_0) & 0x3FF;         // 10bits    1024
        //gAdcResult = DL_ADC12_getMemResult(ADC12_0_INST, DL_ADC12_MEM_IDX_0) & 0xFFF;         // 12bits    4096

        gCheckADC = false;
        DL_ADC12_enableConversions(ADC12_0_INST);                                               // Prepara para a próxima leitura

        if (gAdcResult > THRESHOLD) {
            adcArray[gAdcResult]++;                                                             // Incrementa a contagem para o valor do ADC
            soma += gAdcResult;
            sampleCount++;
            printf("ADC: %d\n", gAdcResult);
            delay_cycles(1000000);

        } else {
            printf("Valor menor: %d\n", gAdcResult);                                            // apenas para visualização, tirar para o codigo final
        }

    }

    media = (soma / NUM_AMOSTRAS);
    printf("Média das amostras: %d\n", media);

    salvarCSV();
    limparArray(amostras, NUM_AMOSTRAS);                                                        // Limpa array após salvar os dados

    while (1) {
       // ---
    }
}
