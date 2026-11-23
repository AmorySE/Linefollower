# basis programma

volgende zaken werken samen in dit project

* instelbare parameters via seriële commando's (bluetooth of seriële monitor)
  * cyclustijd
  * debug on/off
  * start/stop
* parameters worden opgeslagen in het EEPROM-geheugen
* start/stop kan ook geactiveerd worden met een externe interrupt (drukknop op D2)
* draadloze communicatie (HC-06)

Met behulp van de cyclus wordt een knipperende led geprogrammeerd: zolang het programma actief is, wisselt de led op D13 bij elke cyclus van toestand.

**Arduino code:** zie bestand [Basisprogramma/Basisprogramma.ino](Basisprogramma/Basisprogramma.ino)

Dit basisprogramma is de basis van de [finale code](../finaal/): dezelfde manier van commando's verwerken, EEPROM-opslag en interrupt worden daar gebruikt.

### aansluitingen

| onderdeel | Arduino Nano |
|---|---|
| HC-06 TX | D0/RX |
| HC-06 RX | D1/TX via spanningsdeler (1 kΩ naar RX, 2× 1 kΩ naar GND) |
| HC-06 VCC / GND | +5V / GND |
| drukknop | D2 en GND (interne pull-up) |
| led | ingebouwde led D13 |

Zie de schema's van de proofs of concept [draadloze communicatie](../proof%20of%20concepts/wireless%20communication/) en [start/stop interrupt](../proof%20of%20concepts/interrupt/).

### commando's (9600 baud, afsluiten met Enter)

| commando | functie |
|---|---|
| `start` / `stop` | programma starten / stoppen (led knippert / dooft) |
| `set cycle [µs]` | cyclustijd (1000 … 10 000 000 µs), standaard 500 000 µs |
| `debug on` / `debug off` | bij elke cyclus het cyclusnummer doorsturen |
| `info` | huidige instellingen en status tonen |
| `help` | lijst met commando's |

### EEPROM

De instellingen zitten in één struct die met `EEPROM.put` wordt bewaard bij elke wijziging en met `EEPROM.get` wordt geladen bij het opstarten. Een controlegetal (`magic` = 0x4C46, "LF") toont of er geldige instellingen in het EEPROM staan; zo niet (nieuw bord), dan worden standaardwaarden geladen en bewaard.

### test

1. Koppel de HC-06 los, upload `Basisprogramma.ino`, sluit de HC-06 terug aan.
2. Verbind met Serial Bluetooth Terminal en typ `info`.
3. Typ `start`: de led knippert elke 0,5 s. Druk op de knop: de led stopt. Druk opnieuw: de led knippert weer.
4. Typ `set cycle 100000`: de led knippert sneller.
5. Zet de robot uit en terug aan, typ `info`: de cyclustijd is nog altijd 100 000 µs (geladen uit het EEPROM).
