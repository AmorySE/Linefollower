# draadloze communicatie proof of concept
minimale hard- en software waarmee aangetoond wordt dat duplex kan gecommuniceerd worden tussen de microcontroller en een smartphone, gebruik makend van de HC-06 bluetoothmodule
<br />
### configuratie
De draadloze communicatie wordt gerealiseerd met een HC-06 Bluetoothmodule die verbonden is met de Arduino Nano.

De HC-06 maakt via Serial bluetooth terminal verbinding met een smartphone. De communicatie tussen de Arduino en de Bluetoothmodule gebeurt via seriële communicatie.

De aansluitingen zijn:
HC-06	Arduino Nano

VCC	5V

GND	GND

TXD D11 (RX)

RXD	D12 (TX) via spanningsdeler

Voor de communicatie in de Arduino-code kan de bibliotheek SoftwareSerial.h gebruikt worden. Hiermee wordt een extra seriële verbinding aangemaakt, zodat de USB-seriële verbinding beschikbaar blijft voor communicatie met de computer.
### opmerkingen

### gebruiksaanwijzing

