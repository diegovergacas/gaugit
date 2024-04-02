#include <stdio.h>
#include <string.h>
#include <sys/_stdint.h>
#include "ti_msp_dl_config.h"

volatile bool gCheckADC;
volatile uint16_t gAdcResult = 0;   
uint32_t sampleCount = 0;                       // Variável de contagem
uint16_t amostras[NUM_AMOSTRAS];                // Array para armazenar as amostras

#define NUM_AMOSTRAS 10000                      // Números de amostras

//-----------------------------------------------------------------------------------------------------------
void salvarAmostrasCSV(const char *nomeArquivo, uint16_t amostras[], int numAmostras) {
    // Construa o caminho absoluto
    const char *caminhoAbsoluto = "C:/Users/verga/OneDrive/Desktop/ADC_resultados/50kHz_16M.csv";
    
    FILE *file = fopen(caminhoAbsoluto, "w");

    if (file == NULL){
        printf("Erro ao abrir o arquivo.\n");
        return;
    }

    fprintf(file, "Amostra\n");
    for(int i = 0; i < numAmostras; i++){
        fprintf(file, " %d\n", amostras[i]);
    }

    fclose(file);
}

//-----------------------------------------------------------------------------------------------------------
void ADC12_0_INST_IRQHandler(void) {
    switch (DL_ADC12_getPendingInterrupt(ADC12_0_INST)) {
        case DL_ADC12_IIDX_MEM0_RESULT_LOADED:
            gCheckADC = true;           
            break;
        default:
            break;
    }
}

//-----------------------------------------------------------------------------------------------------------
int main(void) {
    SYSCFG_DL_init();

    NVIC_EnableIRQ(ADC12_0_INST_INT_IRQN);                                          // pin 25/29
    gCheckADC = false;

    while (sampleCount < NUM_AMOSTRAS) {
        DL_ADC12_startConversion(ADC12_0_INST);  
        gAdcResult = DL_ADC12_getMemResult(ADC12_0_INST, DL_ADC12_MEM_IDX_0);
        amostras[sampleCount] = gAdcResult;                                         // Armazena a amostra no array 
        sampleCount++;                                                              // Incrementa a contagem de amostras
        gCheckADC = false;                                                          // Reinicia a flag para próxima amostra
        DL_ADC12_enableConversions(ADC12_0_INST);
    }

    salvarAmostrasCSV("amostras.csv", amostras, NUM_AMOSTRAS);
    printf("FIM");

    while (1) {
        // Loop infinito ou código de desativação, dependendo dos requisitos
    }
}
