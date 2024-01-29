#include <stdio.h>
#include <string.h>
#include <sys/_stdint.h>
#include "ti/driverlib/m0p/dl_core.h"
#include "ti_msp_dl_config.h"

volatile bool gCheckADC;
volatile uint16_t gAdcResult;
uint32_t dacValue;

// Quantidade de amostra para criação do espectro
#define NUM_AMOSTRAS 25
// DAC8 Reference Voltage in mV. Adjust this value according to the external VREF voltage.
#define COMP_INST_REF_VOLTAGE_mV (3000)
// DAC8 static output voltage in mV. Adjust outp
#define COMP_INST_DAC8_OUTPUT_VOLTAGE_mV (1000)

// Protótipos de Funções
void inicializarComparador();
void coletarAmostrasADC(uint16_t amostras[], int numAmostras);
void salvarAmostrasCSV(const char *nomeArquivo, uint16_t amostras[], int numAmostras);

// Funções
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

void inicializarComparadorDAC() {
    SYSCFG_DL_init();

    /*
     * Set output voltage:
     *  DAC value (8-bits) = DesiredOutputVoltage x 255
     *                          -----------------------
     *                              ReferenceVoltage
     */
    dacValue = (COMP_INST_DAC8_OUTPUT_VOLTAGE_mV * 255) / COMP_INST_REF_VOLTAGE_mV;
    printf("DACValue: %lu\n", dacValue);

    DL_COMP_setDACCode0(COMP_INST, dacValue);
    DL_COMP_enable(COMP_INST);

    NVIC_EnableIRQ(COMP_INST_INT_IRQN);
    DL_SYSCTL_enableSleepOnExit();
}

// Programa
int main(void) {
    SYSCFG_DL_init();
    // Array para armazenar as amostras
    uint16_t amostras[NUM_AMOSTRAS];
    // Inicializa o comparador e configuração DAC
    inicializarComparadorDAC();

    while(1){
        
        // Verifica o status do comparador
        uint32_t interruptStatus = DL_COMP_getPendingInterrupt(COMP_INST);

        if (interruptStatus & DL_COMP_IIDX_OUTPUT_EDGE) {
            // Código para realizar amostragem ADC
            NVIC_EnableIRQ(ADC12_0_INST_INT_IRQN);  // pin 25/29

            // Variável de contagem para Y amostras
            int sampleCount = 0;

            while (sampleCount < NUM_AMOSTRAS) {
                DL_ADC12_startConversion(ADC12_0_INST);

                // Espera pela conclusão da conversão ADC
                //while (!gCheckADC) {
                //    __WFE();
                //}

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

            // Salva as amostras em um arquivo CSV
            salvarAmostrasCSV("amostras.csv", amostras, NUM_AMOSTRAS);
            break;
        }

        return 0;
    }
}
