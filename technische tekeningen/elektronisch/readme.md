# Elektronisch schema

![Elektronisch schema linefollower](schema_linefollower.png)

EasyEDA Pro-bronbestand: [schema_linefollower.epro2](schema_linefollower.epro2) (openen via EasyEDA Pro → File → Open → EasyEDA Pro)

## Aansluitingen

Het schema komt overeen met de schema's van de proofs of concept in `code/proof of concepts/`.

### Voeding

| van | naar |
|---|---|
| batterijhouder 2x18650 + (7,4 V) | VM van de DRV8833-module en VIN van de Nano |
| batterijhouder 2x18650 − | GND van alle onderdelen (gemeenschappelijke massa) |

- De **aan/uit-schakelaar** zit ingebouwd in de batterijhouder: hij onderbreekt de + van de batterij, zodat motoren en Nano samen uitgeschakeld worden. Daarom staat hij niet als apart onderdeel in het schema.
- De Nano maakt met zijn eigen regelaar 5 V uit VIN. Die **+5V** voedt de QTR-8A, de HC-06 en STBY van de DRV8833-module.
- **Power-on-led:** de ingebouwde PWR-led van de Arduino Nano brandt zodra de schakelaar aan staat.

### H-brug (DRV8833-module)

| DRV8833-module | aangesloten op |
|---|---|
| AIN1 / AIN2 | D5 / D6 |
| BIN1 / BIN2 | D9 / D10 |
| STBY (= nSLEEP van de chip) | +5V |
| AO1 / AO2 | motor 1 + / − |
| BO1 / BO2 | motor 2 + / − |

### Sensoren (QTR-8A)

| QTR-8A | Arduino Nano |
|---|---|
| 1 … 8 | A0 … A7 |
| VCC | +5V |
| GND | GND |
| LEDON | niet aangesloten (intern opgetrokken, IR-leds altijd aan) |

### Bluetooth (HC-06)

| HC-06 | Arduino Nano |
|---|---|
| TX | D0/RX |
| RX | D1/TX via spanningsdeler |
| VCC | +5V |
| GND | GND |

Spanningsdeler: D1/TX → R1 (1 kΩ) → RX van de HC-06, en vanaf dat knooppunt R2 + R3 (2× 1 kΩ) naar GND. Zo krijgt de RX ongeveer 3,3 V in plaats van 5 V. Koppel de HC-06 los tijdens het uploaden (D0/D1 delen de USB-verbinding).

### Start/stop-knop

| drukknop | Arduino Nano |
|---|---|
| pin 1 | D2 (interrupt-pin INT0) |
| pin 2 | GND |

Geen externe weerstand: D2 gebruikt de interne pull-up (`INPUT_PULLUP`).
