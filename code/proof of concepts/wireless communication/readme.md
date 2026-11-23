# draadloze communicatie proof of concept
minimale hard- en software waarmee aangetoond wordt dat duplex kan gecommuniceerd worden tussen de microcontroller en een smartphone, gebruik makend van de HC-06 bluetoothmodule
<br />

### configuratie
De draadloze communicatie wordt gerealiseerd met een HC-06 Bluetoothmodule die verbonden is met de hardware-seriële poort (D0/RX en D1/TX) van de Arduino Nano.

De HC-06 maakt via Serial Bluetooth Terminal verbinding met een smartphone. De communicatie tussen de Arduino en de Bluetoothmodule gebeurt via seriële communicatie met `Serial` op **9600 baud** (standaardinstelling van de HC-06).

**Arduino code:** zie bestand [hc-06_test/hc-06_test.ino](hc-06_test/hc-06_test.ino)

### aansluitingen

| HC-06 | Arduino Nano |
|---|---|
| VCC | +5V |
| GND | GND |
| TXD | D0/RX (rechtstreeks) |
| RXD | D1/TX via spanningsdeler |

De RX van de HC-06 werkt met 3,3 V-signalen, de Arduino stuurt 5 V. Daarom zit er een spanningsdeler tussen D1/TX en RXD:

```
D1/TX ──[ R1 1kΩ ]──●──── RXD (HC-06)
                    │
                [ R2 1kΩ ]
                    │
                [ R3 1kΩ ]
                    │
                   GND
```

Spanning op RXD: 5 V × 2 kΩ / (1 kΩ + 2 kΩ) ≈ 3,3 V. De TXD van de HC-06 (3,3 V) mag rechtstreeks naar D0/RX: dat is voor de Nano al een geldig hoog niveau.

### opmerkingen
- De HC-06 werkt als Bluetooth-slavemodule. De smartphone maakt verbinding met de module.
- De baudrate in de Arduino-code en die van de HC-06 moeten overeenkomen (9600).
- D0 en D1 zijn op de Nano ook verbonden met de USB-chip. **Koppel de HC-06 los tijdens het uploaden**, anders mislukt de upload. Daarna kan de module terug aangesloten worden.
- Omdat de HC-06 op dezelfde seriële poort hangt als de USB, gebeurt de test volledig vanaf de smartphone.

### commando's

| commando (vanaf smartphone) | antwoord van de Arduino |
|---|---|
| `led on` | ingebouwde led (D13) gaat aan, antwoord `OK: led aan` |
| `led off` | ingebouwde led gaat uit, antwoord `OK: led uit` |
| `ping` | `pong` |
| andere tekst | `Ontvangen: <tekst>` (echo) |

Daarnaast stuurt de Arduino elke 5 seconden zelf `Arduino actief, uptime: … s`.

De communicatie is duplex: zowel de Arduino als de smartphone kunnen gegevens versturen en ontvangen.

### gebruiksaanwijzing
1. Koppel de HC-06 los en upload `hc-06_test.ino` naar de Arduino Nano.
2. Sluit de HC-06 aan volgens de tabel hierboven.
3. Schakel Bluetooth in op de smartphone.
4. Zoek naar de HC-06-module en maak verbinding. De standaardpincode is 1234 of 0000.
5. Open Serial Bluetooth Terminal op de smartphone en maak verbinding met de HC-06. Het ledje op de HC-06 knippert dan trager.
6. Controleer of elke 5 seconden het bericht "Arduino actief" binnenkomt (Arduino → smartphone).
7. Verstuur `led on` en `led off` en controleer of de led op de Nano reageert en of het antwoord verschijnt (smartphone → Arduino → smartphone).

Op deze manier wordt aangetoond dat er communicatie mogelijk is in beide richtingen tussen de Arduino Nano en de smartphone.
