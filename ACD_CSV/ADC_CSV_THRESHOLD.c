
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "ti_msp_dl_config.h"

volatile bool gCheckADC = false;
volatile uint16_t gAdcResult = 0;
int sampleCount = 0;

#define NUM_AMOSTRAS 100         // Valor máximo de amostras 150
#define THRESHOLD 621           // Threshold de tensão para iniciar a conversão ADC (621 = 0,5004V)

// Função para salvar amostras em CSV
static void salvarAmostrasCSV(uint16_t amostras[], int numAmostras){
    const char *caminhoAbsoluto = "C:/Users/verga/OneDrive/Desktop/ADC_resultados/amostras.csv";
    FILE *file = fopen(caminhoAbsoluto, "w");
    if (file == NULL){
        printf("Erro ao abrir o arquivo.\n");
        return;
    }

    fprintf(file, "Amostra\n");
    for(int i = 0; i < numAmostras; i++){
        fprintf(file, "%d\n", amostras[i]);
    }

    fclose(file);
}

// Função para limpar array
static void limparArray(uint16_t array[], int tamanho) {
    memset(array, 0, tamanho * sizeof(uint16_t));
}

void ADC12_0_INST_IRQHandler(void){
    switch (DL_ADC12_getPendingInterrupt(ADC12_0_INST)) {
        case DL_ADC12_IIDX_MEM0_RESULT_LOADED:
            gCheckADC = true;
            break;
        default:
            break;
    }
}

int main(void) {
    SYSCFG_DL_init();
    NVIC_EnableIRQ(ADC12_0_INST_INT_IRQN);
    
    uint16_t amostras[NUM_AMOSTRAS];
    limparArray(amostras, NUM_AMOSTRAS); // Garante que o array esteja limpo antes de começar

    while (sampleCount < NUM_AMOSTRAS) {
        DL_ADC12_startConversion(ADC12_0_INST);
        while (!gCheckADC); // Espera até a conversão estar pronta
        gAdcResult = DL_ADC12_getMemResult(ADC12_0_INST, DL_ADC12_MEM_IDX_0);
        //while (!gCheckADC); // Espera até a conversão estar pronta

        if (gAdcResult >= THRESHOLD) {
            amostras[sampleCount++] = gAdcResult; // Armazena e incrementa a contagem de amostras
            printf("%d\n", gAdcResult);
        } else {
            printf("Valor menor: %d\n", gAdcResult);
        }

        gCheckADC = false; // Prepara para a próxima leitura
        DL_ADC12_enableConversions(ADC12_0_INST);
    }    

    salvarAmostrasCSV(amostras, NUM_AMOSTRAS);
    limparArray(amostras, NUM_AMOSTRAS); // Limpa array após salvar os dados
    sampleCount = 0;
    
    while (1) {
        // Loop ou ações de encerramento aqui
    }
}

