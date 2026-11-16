# draadloze communicatie proof of concept
minimale hard- en software waarmee aangetoond wordt dat duplex kan gecommuniceerd worden tussen de microcontroller en een smartphone, gebruik makend van de HC-06 bluetoothmodule
<br />
### configuratie
De draadloze communicatie wordt gerealiseerd met een HC-06 Bluetoothmodule die verbonden is met de Arduino Nano.

De HC-06 maakt via Serial bluetooth terminal verbinding met een smartphone. De communicatie tussen de Arduino en de Bluetoothmodule gebeurt via seriële communicatie.

De aansluitingen zijn:

|HC-06	|Arduino Nano|

VCC	5V

GND	GND

TXD D11 (RX)

RXD	D12 (TX) via spanningsdeler

Voor de communicatie in de Arduino-code kan de bibliotheek SoftwareSerial.h gebruikt worden. Hiermee wordt een extra seriële verbinding aangemaakt, zodat de USB-seriële verbinding beschikbaar blijft voor communicatie met de computer.
### opmerkingen
De HC-06 werkt als Bluetooth-slavemodule. De smartphone maakt verbinding met de module.

De baudrate is 9600 baud.

De baudrate in de Arduino-code en de Bluetoothverbinding moeten overeenkomen.

Merk op dat de RX van de bluetoothmodule niet rechtsreeks verbonden is met de TX van de microcontroller! Dit is belangrijk aangezien de HC-06 Bluetoothmodule met 3.3V signalen verwacht. De arduino stuurt signalen van 5V. Daarom maken we een spanningsdeler met behulp van 2 weerstanden (1kOhm en 2Kohm) zodat we de 5V een stuk kunnen reduceren.

De communicatie is duplex: zowel de Arduino als de smartphone kunnen gegevens versturen en ontvangen.
### gebruiksaanwijzing
1. Upload de Arduino-code naar de microcontroller.
2. Sluit de HC-06 Bluetoothmodule aan op de Arduino Nano.
3. Schakel Bluetooth in op de smartphone.
4. Zoek naar de HC-06-module en maak verbinding. De standaardpincode 0000.
5. Open een Bluetooth Serial Terminal-app op de smartphone.
6. Verstuur een tekstbericht vanaf de smartphone naar de Arduino.
7. Controleer of het ontvangen bericht verschijnt in de Serial Monitor van de computer.
8. Verstuur vervolgens een bericht vanuit de Serial Monitor naar de smartphone.
9. Controleer of het bericht correct verschijnt in de Bluetooth Serial Terminal.
Op deze manier wordt aangetoond dat er communicatie mogelijk is in beide richtingen tussen de Arduino Nano en de smartphone.

