/*

NÃO ESTA FUNCIONANDO

*/




#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include "ti_msp_dl_config.h"



volatile bool gCheckADC;
volatile uint16_t gADCResult;
volatile bool raspReady = false;
char buffer[12] = {0};                // Buffer suficiente para cabeçalho, dados e rodapé

// --------------------------------------------------------
// Função fictícia para exemplificar a leitura de dados via UART e definir a flag raspReady
void verificarProntidaoRasp() {
    volatile char sinal;
    if (DL_UART_Main_dataAvailable(UART_0_INST) > 0) {
        sinal = DL_UART_Main_receiveData(UART_0_INST);
        raspReady = (sinal == '1');  // Supondo que o Raspberry Pi Pico envie '1' como sinal de prontidão
    }
}

// ---------------------------------------------------------
void enviarDadosADC(uint16_t adcValor) {


    verificarProntidaoRasp();       // Verifica se o Raspberry Pi Pico está pronto para receber dados

    if (raspReady) {
        sprintf(buffer, "%d", gADCResult);       // Converte gADCResult para string

        // Envia cada caractere da string via UART
        for (int i = 0; i < strlen(buffer); i++) {
            DL_UART_Main_transmitData(UART_0_INST, buffert[i]);
        }

        // Envie um caractere de nova linha e retorno a posição inicial
        DL_UART_Main_transmitData(UART_0_INST, '\r');
        DL_UART_Main_transmitData(UART_0_INST, '\n');

    raspReady = false;                   // Reset da flag
    }
}

// ---------------------------------------------------------
void ADC12_0_INST_IRQHandler(void)
{
    switch (DL_ADC12_getPendingInterrupt(ADC12_0_INST)) {
        case DL_ADC12_IIDX_MEM0_RESULT_LOADED:
            gCheckADC = true;
            break;
        default:
            break;
    }
}

// ----------------------------------------------------------
int main(void) {
    SYSCFG_DL_init();
    NVIC_EnableIRQ(ADC12_0_INST_INT_IRQN);
    gCheckADC = false;

    while (1) {
        if (!gCheckADC) {
            DL_ADC12_startConversion(ADC12_0_INST);
        }

        while (false == gCheckADC) {
            __WFE();
        }

        gADCResult = DL_ADC12_getMemResult(ADC12_0_INST, DL_ADC12_MEM_IDX_0);

        // Envia o resultado do ADC empacotado
        enviarDadosADC(gADCResult);

        printf("ADC: %d\n", gADCResult);

        gCheckADC = false;
        DL_ADC12_enableConversions(ADC12_0_INST);
        delay_cycles(100);
    }
}

// ------------------------------------------------------------
