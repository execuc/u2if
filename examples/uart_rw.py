import time
from machine import UART

# Connect a USB to UART (CP2102, FT232, ...) to PICO UART0 (GP_0, GP_1) and open terminal on it (putty, ...)
uart = UART(0)
uart.init(115200)

fable = bytearray("""\r\nLa Cigale, ayant chanté\r
Tout l'été,\r
Se trouva fort dépourvue\r
Quand la bise fut venue :\r
Pas un seul petit morceau\r
De mouche ou de vermisseau.\r
Elle alla crier famine\r
Chez la Fourmi sa voisine,\r
La priant de lui prêter\r
Quelque grain pour subsister\r
Jusqu'à la saison nouvelle.\r
"Je vous paierai, lui dit-elle,\r
Avant l'Oût, foi d'animal,\r
Intérêt et principal. "\r
La Fourmi n'est pas prêteuse :\r
C'est là son moindre défaut.\r
Que faisiez-vous au temps chaud ?\r
Dit-elle à cette emprunteuse.\r
- Nuit et jour à tout venant\r
Je chantais, ne vous déplaise.\r
- Vous chantiez ? j'en suis fort aise.\r
Eh bien! dansez maintenant.\r\r\n
       Fable de Jean de la Fontaine\n""".encode())

uart.write(fable)
time.sleep(1)

uart.write("\r\nType character on terminal\r\n".encode())

while True:
    buf = uart.read(5, timeout=10)
    # buf = uart.read(5)
    # buf = uart.read()
    # buf = uart.readline()
    # buf = uart.readline(0)
    # buf = uart.readline(10)
    # buf2 = [0, 0, 0, 0, 0, 0]
    # uart.readinto(buf2, 4, 10)
    # uart.readinto(buf2, timeout=10)

    if len(buf) == 0:
        print("No data")
    else:
        print(buf)

    print("Remaining bytes in UART : %d" % uart.any())
