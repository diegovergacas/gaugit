"""
Projeto: Histograma de Leitura ADC com Raspberry Pi Pico
Descrição: Este script lê valores de um sinal analógico usando ADC, 
- constrói um histograma baseado nesses valores durante um período específico e salva o histograma em um arquivo CSV. 
- A leitura ADC é acionada por um sinal externo através de um comparador conectado a um pino GPIO configurado para interrupções.
- Microcontrolador: Raspberry Pi Pico
Funcionalidades:
- Leitura de sinal analógico através do ADC.
- Construção de histograma dos valores lidos.
- Salvamento do histograma em arquivo CSV após período de registro.
Comunicação: Bluetooth via UART (GPIOs 4 e 5 para TX e RX, respectivamente).
ADC: Pino 26 para leitura do sinal analógico.
Sinal do Comparador: Pino 22 configurado para detectar borda de subida e acionar leitura ADC.
Período de Registro do Histograma: 60 segundos.
Autor: Diego Vergaças
Data de Criação: 26/03/2024
Última Modificação: 
Observações:
"""

from machine import ADC, Pin, UART
import utime

# Configurações iniciais
uart = UART(1, baudrate=9600, tx=Pin(4), rx=Pin(5))  # Para comunicação Bluetooth
adc_pin = ADC(26)  # Supondo que o sinal analógico está conectado ao GP26
pino_comparador = Pin(22, Pin.IN, Pin.PULL_DOWN)  # Pino do sinal do comparador
histograma = [0] * 4096  # Inicializa o histograma para um ADC de 12 bits

# Variáveis para o controle do tempo
inicio_tempo = None
periodo_registro = 60  # Período de registro do histograma em segundos

def handler_interrupcao(pin):
    global inicio_tempo
    if inicio_tempo is None:
        print("Iniciando a coleta do histograma...")
        inicio_tempo = utime.time()
    else:
        valor_adc = adc_pin.read_u16() >> 4  # Ajusta para 12 bits
        histograma[valor_adc] += 1  # Incrementa a contagem para o valor lido

def salvar_histograma_em_csv(nome_arquivo="histograma_adc.csv"):
    with open(nome_arquivo, "w") as arquivo:
        for indice, contagem in enumerate(histograma):
            arquivo.write("{},{}\n".format(indice, contagem))
    print(f"Histograma salvo em {nome_arquivo}")

# Configura a interrupção para acionar na borda de subida
pino_comparador.irq(trigger=Pin.IRQ_RISING, handler=handler_interrupcao)

print("Aguardando sinal do comparador...")

while True:
    if inicio_tempo and utime.time() - inicio_tempo >= periodo_registro:
        salvar_histograma_em_csv()
        inicio_tempo = None  # Reseta o contador para possibilitar uma nova coleta
    utime.sleep(0.25)

