/**
 * @file main.c
 * @brief Programa para leitura ADC e envio via UART no MSPM0G3507.
 *
 * Este programa configura o MSPM0G3507 para realizar leituras do ADC e enviar
 * os resultados através da UART para um Raspberry Pi Pico. Os dados do ADC são
 * formatados como strings ASCII e enviados sem encapsulamento adicional.
 *
 * Pinagem:
 * - UART 0:
 *   - RX -> PB1/48
 *   - TX -> PB0/47
 * - ADC0:
 *   - Input -> PA25/26
 *
 * @microcontrolador MSPM0G3507
 * @ide Code Composer Studio Versão 12.6.0.00008
 * @autor Diego Vergaças
 * @data 10/03/2024
 * @observacoes:
 *
 * Revisões:
 * - [Data] [Nome] [Descrição da mudança]
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
char buffer[12];                // Buffer suficiente para cabeçalho, dados e rodapé

//---------------------------------------------------------------------------------
void enviarDadosADC(uint16_t adcValor) {
    sprintf(buffer, "%d", adcValor);
    
    // Envia cada caractere da string via UART
    for (int i = 0; i < strlen(buffer); i++) {
        DL_UART_Main_transmitData(UART_0_INST, buffer[i]);
    }
}

// -----------------------------------------------------------------------------
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

// -----------------------------------------------------------------------------
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

        DL_ADC12_startConversion(ADC12_0_INST);
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
