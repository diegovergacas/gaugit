/**
 * @file main.c
 * 
 * Descrição:
 * Este código é destinado ao microcontrolador LP-MSPM0G3507 e realiza a leitura
 * de um valor ADC simulado, converte-o para uma string e transmite essa string via UART.
 * 
 * IDE: Code Composer Studio
 * Versão: 12.6.0.00008
 * 
 * Microcontrolador: LP-MSPM0G3507
 * 
 * Autor: Diego Vergças
 * 
 * Notas:
 * - Pinos de comunicação UART utilizados são TX PA28 e RX PA31.
 * - O valor ADC é inicialmente simulado como o máximo de 16 bits (65535) para teste.
 * - A taxa de transmissão e a configuração da UART devem ser definidas adequadamente
 *   no receptor para garantir a correta recepção dos dados.
 * - Este código inclui um atraso deliberado entre transmissões para facilitar a leitura
 *   e o processamento pelo dispositivo receptor.
 *
 * Revisões:
 * - [Data] [Autor] [Mudanças]
 * - 12/03/2024 Diego Vergças - Criação do arquivo e implementação inicial.
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "ti_msp_dl_config.h"

volatile bool gCheckADC;
volatile uint16_t gADCResult = 65535; // valor de 0 a 65635


int main(void) {
    SYSCFG_DL_init();

    while (1) {

        // Buffer para armazenar o valor convertido em string
        char strResult[10];                                     // Converter a string de 10 caracteres

        // Converte gADCResult para string
        sprintf(strResult, "%d", gADCResult);

        // Envia cada caractere da string via UART
        for (int i = 0; i < strlen(strResult); i++) {
            DL_UART_Main_transmitData(UART_0_INST, strResult[i]);
        }

        // Envie um caractere de nova linha e retorno a posição inicial
        DL_UART_Main_transmitData(UART_0_INST, '\r');
        DL_UART_Main_transmitData(UART_0_INST, '\n');

        delay_cycles(100000);
        printf("ADC: %s\n", strResult);                         // Use %s para formatar strings
        delay_cycles(100);
    }
}
