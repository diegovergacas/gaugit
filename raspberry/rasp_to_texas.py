'''

NÃO ESTA FUNCIONANDO A PARTE DA TEXAS

'''


from machine import UART, Pin, ADC
from utime import sleep
import utime

"""-----------------------------------------------------------------------------"""
led = Pin(25, Pin.OUT)  # Pino do LED como saída

# Inicializa a UART para o Bluetooth
blue = UART(1, baudrate=9600, tx=Pin(4), rx=Pin(5))

# Inicializa a UART para a leitura do ADC externo
adc = UART(0, baudrate=9600, tx=Pin(0), rx=Pin(1))


# Configura o sensor de temperatura interno
sensor_temp = ADC(4)
conversion_factor = 3.3 / (65535)

"""-----------------------------------------------------------------------------"""
# Função para enviar e imprimir a temperatura
def enviar_temperatura():
    while True:
        reading = sensor_temp.read_u16() * conversion_factor
        temperature = 27 - (reading - 0.706)/0.001721
        #blue.write(str(temperature).encode() + b'\n\r')
        print("Temperatura:", temperature)
        utime.sleep(2)

"""-----------------------------------------------------------------------------"""
# Função para ler e imprimir dados do ADC externo
def ler_adc():
    # Sinal para solicitar dados do ADC
    solicitar_dados_adc = b'R'  # Pronto para receber
    nao_solicitar_dados_adc = b'N'  # Não pronto para receber

    while True:
        # Suponha que há uma condição que determina se estamos prontos para receber dados
        if estamos_prontos_para_receber():  # Você precisa definir essa função ou condição
            adc.write(solicitar_dados_adc)
        else:
            adc.write(nao_solicitar_dados_adc)
            utime.sleep(1)  # Pausa antes de verificar novamente
            continue  # Pula para a próxima iteração do loop
        
        # Dá um breve tempo para o outro microcontrolador responder
        utime.sleep(0.5)
        
        # Verifica se há dados disponíveis para leitura
        if adc.any() > 0:
            rxData = adc.read(adc.any())
            try:
                print("Dados ADC:", rxData.decode('utf-8'))
                # Código adicional para processar/enviar os dados recebidos
            except UnicodeDecodeError:
                print("Erro ao decodificar os dados recebidos.")

        # Espera antes de solicitar os dados novamente
        utime.sleep(1)
        
"""-----------------------------------------------------------------------------"""
while True:
    # Enviar e imprimir a temperatura
    enviar_temperatura()
    ler_adc()
    
    # Piscar o LED
    led.value(1)
    sleep(1)
    led.value(0)
    sleep(1)
