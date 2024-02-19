
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "ti_msp_dl_config.h"

volatile bool gCheckADC = false;
volatile uint8_t gAdcResult = 0;
int sampleCount = 0;

#define NUM_AMOSTRAS 300         // Valor máximo de amostras 150
#define THRESHOLD 5           // Threshold de tensão para iniciar a conversão ADC (25 = 0,5004V)

// Array para contar as ocorrências de cada valor ADC
uint8_t adcHistogram[256] = {0};

static void salvarHistogramaCSV(){
    const char *caminhoAbsoluto = "C:/Users/verga/OneDrive/Desktop/ADC_resultados/histograma.csv";
    FILE *file = fopen(caminhoAbsoluto, "w");
    if (file == NULL){
        printf("Erro ao abrir o arquivo.\n");
        return;
    }

    fprintf(file, "Valor ADC,Ocorrencias\n");
    for(int i = 0; i < 256; i++){
        //if(adcHistogram[i] > 0){
            fprintf(file, "%d,%d\n", i, adcHistogram[i]);
        //}
    }

    fclose(file);
}

// Função para limpar o histograma
void limparHistograma() {
    for(int i = 0; i < 256; i++) {
        adcHistogram[i] = 0;
    }
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

//---------------------------------------------------------------------

int main(void) {
    SYSCFG_DL_init();
    NVIC_EnableIRQ(ADC12_0_INST_INT_IRQN);
    
    uint8_t amostras[NUM_AMOSTRAS];
    limparHistograma();
 
    while (sampleCount < NUM_AMOSTRAS) {
        DL_ADC12_startConversion(ADC12_0_INST);
        while (!gCheckADC); // Espera até a conversão estar pronta
        gAdcResult = DL_ADC12_getMemResult(ADC12_0_INST, DL_ADC12_MEM_IDX_0) & 0xFF;

        if (gAdcResult >= THRESHOLD) {
            adcHistogram[gAdcResult]++; // Incrementa a contagem para o valor do ADC
            sampleCount++;
            printf("%d\n", gAdcResult);
            
        } else {
            printf("Valor menor: %d\n", gAdcResult);
        }

        gCheckADC = false; // Prepara para a próxima leitura
        DL_ADC12_enableConversions(ADC12_0_INST);
    }    

    salvarHistogramaCSV(); // Salva o histograma após coletar todas as amostras

    while (1) {
        // Loop ou ações de encerramento aqui
    }
}

