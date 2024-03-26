"""
Projeto: 
Monitoramento de ADC com Acionamento Externo e Comunicação Bluetooth

Descrição:
Este script é destinado ao uso com o Raspberry Pi Pico para monitorar um sinal analógico,
realizar a conversão Analógico-Digital (ADC) e enviar os valores convertidos via Bluetooth.
A leitura do ADC é acionada por um sinal externo através de um comparador conectado a um pino GPIO configurado para interrupções.

Funcionalidades:
- Monitoramento de sinal analógico com ajuste de zero.
- Acionamento da leitura ADC por sinal externo via interrupção.
- Envio dos valores ADC ajustados via comunicação UART (Bluetooth).

Configurações Principais:
- Tensão de referência do ADC: 3.3V
- Resolução do ADC: 12 bits
- Pino ADC: GP26 para leitura do sinal analógico.
- Pino de Interrupção: GP22 utilizado para receber sinal do comparador externo.
- Comunicação Bluetooth: UART configurada nos pinos GP4 (TX) e GP5 (RX).

Microcontrolador: Raspberry Pi Pico
Autor: Diego Vergaças
Data de Criação: 26/03/2024
Última Modificação: [Data da última modificação, se aplicável]

Observações:
Ajuste 'v_zero' conforme necessário para definir a tensão de 'zero' desejada.
Certifique-se de que os dispositivos externos estejam corretamente conectados e configurados.
"""

from machine import ADC, Pin, UART
import utime

'''-----------------------------------------------------------------------------'''

# Configurações do ADC e do offset
v_zero = 0.0 											# Tensão de "zero" volts
v_ref = 3.3												# Tensão de referência do ADC em volts
adc_resolucao = 4095									# Resolução do ADC de 12 bits
valor_zero = int((v_zero / v_ref) * adc_resolucao)  	# Conversão da tensão de "zero" para valor digital do ADC

adc_pin = ADC(26)  										# Assume que o sinal analógico está conectado ao GP26
pino_comparador = Pin(22, Pin.IN, Pin.PULL_DOWN)  		# Exemplo: pino GP22 como entrada do sinal do comparador

uart = UART(1, baudrate=9600, tx=Pin(4), rx=Pin(5))		# Configuração do UART para comunicação Bluetooth

'''-----------------------------------------------------------------------------'''
def handler_interrupcao(pin):
    valor_adc = adc_pin.read_u16()
    print("Valor ADC lido:", valor_adc)
    uart.write(str(valor_adc) + '\n')  					# Envia o valor lido via UART

'''-----------------------------------------------------------------------------'''
def ler_adc_corrigido():
    valor_bruto = adc_pin.read_u16() >> 4  				# Converte de 16 para 12 bits para combinar com a resolução do ADC
    valor_corrigido = valor_bruto - valor_zero  		# Aplica o offset para ajustar o "zero"
    return valor_corrigido

print("Iniciando leitura do ADC e transmissão via Bluetooth...")

'''-----------------------------------------------------------------------------'''
def enviar():
    valor_ajustado = ler_adc_corrigido()
    valor_str = str(valor_ajustado)						# Converte o valor ajustado para string para transmissão

    uart.write(valor_str + '\r\n')						# Envia o valor ajustado via Bluetooth
    
    print("Valor ADC ajustado enviado:", valor_str)
    
'''-----------------------------------------------------------------------------'''
def handler_interrupcao(pin):
    enviar()

# Configura a interrupção para acionar na borda de subida (quando o comparador indica que está pronto)
pino_comparador.irq(trigger=Pin.IRQ_RISING, handler=handler_interrupcao)

print("Aguardando sinal do comparador...")

'''-----------------------------------------------------------------------------'''

while True:
    utime.sleep(0.25)

