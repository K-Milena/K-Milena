# **************************************************************************** #
#                                                                              #
#  Projekt: SYIR - Systemy Internetu Rzeczy                                    #
#                                                                              #
#  Plik:    get_uart_settings.py                                               #
#                                                                              #
#  Autor:   Arkadiusz Luczyk; Jakub Jasinski                                   #
#  Email:   arkadiusz.luczyk@pw.edu.pl; jakub.jasinski@pw.edu.pl               #
#                                                                              #
#  Utworzony:     2023/03/24 11:41:12 przez Arkadiusz Luczyk; Jakub Jasinski   #
#  Zmodyfikowany: 2023/03/24 13:15:20 przez Arkadiusz Luczyk; Jakub Jasinski   #
#                                                                              #
# **************************************************************************** #

import argparse
import random as rnd

parser = argparse.ArgumentParser(description='Generacja stalych konfiguracji UART')

parser.add_argument('-freq', type=int, default=32000000, help='Czestotliwosc pracy zegara ukladu [Hz]  - default = 32000000')
parser.add_argument('-div',  type=int, default=1,        help='Krotnosc podzialu czestotliwosci zegara - default = 1')
parser.add_argument('baud',  type=int, default=9600,     help='Baudrate dla ktorego liczone sa stale')

args = parser.parse_args()

assert args.freq > 0, f"ERROR: Czestotliwosc nie moze byc mniejsza od zera: {args.freq}"
assert args.div > 0, f"ERROR: Wspolczynnik podzialu czestotliwosci nie moze byc mniejszy od 1: {args.div}"

clk2x = -1
if args.freq / 16 > args.baud:
    clk2x = 0
elif args.freq / 8 > args.baud:
    clk2x = 1
assert not clk2x < 0, f"Zadany baudrate {args.baud} nie jest możliwy dla podanej czestotliwoci {args.freq} i podzialu czestotliwosci {args.div}"

# Czestotliwosc zegara po podziale
freq = args.freq / args.div

const = 16
if clk2x == 1: 
    const = 8
wasBsel0 = False

print(f"\nUstawienia parametrow UART dla zadanych: \n\tbaud rate = {args.baud} \n\tfreq = {args.freq} \n\tclk div = {args.div}:\n")
for bscale in range (-7,7):
    if bscale < 0:
        bsel = int((1/(2**bscale)) * (freq / (const * args.baud) -1 ))
        if bsel == 0: 
            if wasBsel0: continue
            bscale = 0
            wasBsel0 = True
        baud = freq / (const * ((2**bscale * bsel)+1))
        err  = abs((args.baud - baud)/args.baud * 100)
        if bsel >= 0 and bsel <= 4096 and err < 10:
            print(f"Mozliwe ustawienie:   BSCALE={bscale:{2}},   BSEL={int(bsel):{6}},   CLK2X={clk2x},   dokladnosc={err:{4}.{2}}%") 
    else:
        bsel = int(freq/((2**bscale)* const * args.baud - 1))
        if bsel == 0: 
            if wasBsel0: continue
            bscale = 0
            wasBsel0 = True
        baud = freq/((2**bscale) * const * bsel+1)
        err  = abs(int(1000*(args.baud - baud)/args.baud * 100)/1000)
        if bsel >= 0 and bsel <= 4096 and err < 10:
            print(f"Mozliwe ustawienie:   BSCALE={bscale:{2}},   BSEL={int(bsel):{6}},   CLK2X={clk2x},   dokladnosc={err:{4}.{2}}%") 
print()