
'''
Nome do Projeto: UART Communication with MSP430G2553
Descrição: Este script implementa a comunicação UART entre o Raspberry Pi Pico (RP2040) e um microcontrolador MSP430G2553, lendo valores ADC enviados pelo MSP430G2553 e convertendo-os de volta para inteiros no Raspberry Pi Pico.
Microcontrolador utilizado: Raspberry Pi Pico (RP2040)
IDE utilizada: Thonny Python IDE 4.1.4
Autor: Diego Vergaças
Data de criação: [2024-03-25]
Última modificação: [Data da última modificação, se aplicável]
Observações: Certifique-se de que a taxa de baudrate e os pinos TX/RX estão corretamente configurados em ambos os microcontroladores.
Pinos UART 1: 	TX GPIO 4
                RX GPIO 5
'''

from machine import UART, Pin
import utime

# Configuração inicial da UART no Raspberry Pi Pico
uart = UART(1, baudrate=9600, tx=Pin(4), rx=Pin(5))

print("Iniciando recepção de dados...")

while True:
    if uart.any():
        # Lê os dados disponíveis na UART
        rxData = uart.read(uart.any())
        #print("Dados brutos recebidos:", rxData)
        
        try:
            # Decodifica os dados recebidos para string
            dados_str = rxData.decode('utf-8')
            # Converte a string decodificada para int
            dados_int = int(dados_str)
            print("Valor ADC convertido:", dados_int)
        except ValueError:
            print("Erro ao converter os dados para inteiro.")
        except UnicodeDecodeError:
            print("Erro de decodificação Unicode.")
        
        utime.sleep(0.1)  # Pequena pausa para evitar sobrecarga
