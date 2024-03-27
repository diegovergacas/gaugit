"""
Projeto: 
Monitoramento de ADC com Acionamento Externo e Comunicação Bluetooth

Descrição:
Este script é destinado ao uso com o Raspberry Pi Pico para monitorar um sinal analógico,
realizar a conversão Analógico-Digital (ADC) e enviar os valores convertidos via Bluetooth.
A leitura do ADC é acionada por um sinal externo através de um comparador conectado a um pino GPIO configurado para interrupções.
Além disso, este projeto utiliza um display OLED para exibir um histograma em tempo real dos valores ADC coletados, proporcionando uma visualização imediata e intuitiva das leituras.

Funcionalidades:
- Monitoramento de sinal analógico com ajuste de zero.
- Acionamento da leitura ADC por sinal externo via interrupção.
- Envio dos valores ADC ajustados via comunicação UART (Bluetooth).
- Visualização dos dados do ADC em forma de histograma no display OLED.

Configurações Principais:
- Tensão de referência do ADC: 3.3V
- Resolução do ADC: 12 bits
- Pino ADC: GP26 para leitura do sinal analógico.
- Pino de Interrupção: GP22 utilizado para receber sinal do comparador externo.
- Comunicação Bluetooth: UART configurada nos pinos GP4 (TX) e GP5 (RX).
- Display OLED: Utilizado para a visualização dos dados do ADC em forma de histograma, facilitando a interpretação dos dados coletados.
- Pino oled: SCL GPIO(17), SDA GPIO(16)

Microcontrolador: Raspberry Pi Pico
Autor: Diego Vergaças
Data de Criação: 27/03/2024
Última Modificação: [Data da última modificação, se aplicável]

Observações:
Ajuste 'v_zero' conforme necessário para definir a tensão de 'zero' desejada.
Certifique-se de que os dispositivos externos estejam corretamente conectados e configurados.
Biblioteca para utilização do oled é própria ssd1306.py, tem que savar ela dentro do chip do Rasp.
"""

from machine import ADC, Pin, UART, I2C, SoftI2C
import utime
import ssd1306

# Configurações iniciais do ADC e do UART
V_ZERO = 0.0
V_REF = 3.3
ADC_RESOLUCAO = 4095
adc_pin = ADC(26)
pino_comparador = Pin(22, Pin.IN, Pin.PULL_DOWN)
uart = UART(1, baudrate=9600, tx=Pin(4), rx=Pin(5))
valor_zero = int((V_ZERO / V_REF) * ADC_RESOLUCAO)

# Configuração do I2C para o display OLED
i2c = SoftI2C(scl=Pin(17), sda=Pin(16))
oled_width = 128
oled_height = 64
oled = ssd1306.SSD1306_I2C(oled_width, oled_height, i2c)

# Histograma com 4096 posições
histograma = [0] * (ADC_RESOLUCAO + 1)
contador_atualizacao = 0
intervalo_atualizacao = 10  # Atualiza o display a cada 10 leituras

def atualizar_display():
    oled.fill(0)  # Limpa o display, incluindo as 10 primeiras linhas para futuros usos
    max_contagem = max(histograma) if max(histograma) > 0 else 1
    escala = (oled_height - 20) / max_contagem if max_contagem > oled_height - 20 else 1

    for i in range(len(histograma)):
        valor = histograma[i]
        altura_barra = int(valor * escala)
        altura_barra = min(altura_barra, oled_height - 20)  # Garante que não exceda a área destinada

        for y in range(altura_barra):
            oled.pixel(i // (len(histograma) // oled_width), oled_height - 11 - y, 1)

    # Adiciona os números 0 e 4095 na base do histograma
    oled.text('0', 0, oled_height - 9)
    oled.text('4095', oled_width - 36, oled_height - 9)
    oled.show()

def ler_adc_corrigido():
    valor_bruto = adc_pin.read_u16() >> 4
    return valor_bruto - valor_zero

def coletar_dados():
    global contador_atualizacao
    valor_ajustado = ler_adc_corrigido()
    histograma[valor_ajustado] += 1
    mensagem = f"Valor ADC: {valor_ajustado}"
    uart.write(mensagem + '\r\n')
    print(mensagem)

    contador_atualizacao += 1
    if contador_atualizacao >= intervalo_atualizacao:
        atualizar_display()
        contador_atualizacao = 0

def handler_interrupcao(pin):
    coletar_dados()

pino_comparador.irq(trigger=Pin.IRQ_RISING, handler=handler_interrupcao)

print("Aguardando sinal do comparador...")

while True:
    utime.sleep(0.25)
