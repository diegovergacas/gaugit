#include <stdio.h>
#include <string.h>
#include <sys/_stdint.h>
#include "ti_msp_dl_config.h"

volatile bool gCheckADC;
volatile uint16_t gAdcResult;

#define NUM_AMOSTRAS 100

void salvarAmostrasCSV(const char *nomeArquivo, uint16_t amostras[], int numAmostras){
    // Construa o caminho absoluto
    const char *caminhoAbsoluto = "C:/Users/verga/OneDrive/Desktop/ADC_resultados/amostras.csv";
    
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

void ADC12_0_INST_IRQHandler(void){
    switch (DL_ADC12_getPendingInterrupt(ADC12_0_INST)) {
        case DL_ADC12_IIDX_MEM0_RESULT_LOADED:
            gCheckADC = true;
            break;
        default:
            break;
    }
}

int main(void){
    SYSCFG_DL_init();

    NVIC_EnableIRQ(ADC12_0_INST_INT_IRQN);  // pin 25/29
    gCheckADC = false;

    // Variável de contagem para 50 amostras
    int sampleCount = 0;

    // Array para armazenar as amostras
    uint16_t amostras[NUM_AMOSTRAS];

    while (sampleCount < NUM_AMOSTRAS) {
        DL_ADC12_startConversion(ADC12_0_INST);

        // Espera pela conclusão da conversão ADC
        while (!gCheckADC) {
            __WFE();
        }

        gAdcResult = DL_ADC12_getMemResult(ADC12_0_INST, DL_ADC12_MEM_IDX_0);
        printf(" %d\n", gAdcResult);

        // Armazena a amostra no array
        amostras[sampleCount] = gAdcResult;

        // Incrementa a contagem de amostras
        sampleCount++;

        // Reinicia a flag para próxima amostra
        gCheckADC = false;
        DL_ADC12_enableConversions(ADC12_0_INST);
    }

    salvarAmostrasCSV("amostras.csv", amostras, NUM_AMOSTRAS);

    // Adicione qualquer ação de término aqui, se necessário
    // ...

    // Loop infinito ou código de desativação, dependendo dos requisitos
    while (1) {
        // Loop ou ações de encerramento aqui
        // ...
    }
}
