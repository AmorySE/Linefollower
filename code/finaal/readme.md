# Finale code

**Bestand:** [Code_Lijnvolger_Robot/Code_Lijnvolger_Robot.ino](Code_Lijnvolger_Robot/Code_Lijnvolger_Robot.ino)

Het bestand staat in een map met dezelfde naam, zodat het rechtstreeks in de Arduino IDE opent. Er zijn geen extra bibliotheken nodig (`EEPROM.h` zit standaard in de Arduino IDE).

## Uploaden

1. Koppel de HC-06 los (D0/D1 delen de USB-verbinding).
2. Open `Code_Lijnvolger_Robot.ino` in de Arduino IDE.
3. Kies **Tools → Board → Arduino Nano**, processor **ATmega328P (Old Bootloader)** en de juiste poort.
4. Klik op **Upload**.
5. Sluit de HC-06 terug aan.

## Pinnen

Komen overeen met het [elektronisch schema](../../technische%20tekeningen/elektronisch/).

| functie | pin |
|---|---|
| motor links (AIN1 / AIN2) | D5 / D6 |
| motor rechts (BIN1 / BIN2) | D9 / D10 |
| sensoren QTR-8A 1 … 8 | A0 … A7 |
| start/stop-knop (naar GND, `INPUT_PULLUP`, interrupt INT0) | D2 |
| HC-06 (TX / RX via spanningsdeler) | D0 / D1, 9600 baud |
| statusled (aan = rijden, knipperen = kalibreren) | D13 (ingebouwde led) |

STBY van de DRV8833-module hangt vast aan +5V: de driver staat altijd aan en wordt niet vanuit de code aangestuurd.

## Opbouw

- **Instellingen in EEPROM:** power, max, diff, Kp, Ki, Kd, cycle, debug en de kalibratie zitten in één struct die met `EEPROM.put` / `EEPROM.get` bewaard en geladen wordt. Een controlewaarde (`magic`) toont of er geldige instellingen in het EEPROM staan; zo niet, dan worden standaardwaarden geladen. De robot start dus altijd met de laatst opgeslagen instellingen.
- **Commando's via bluetooth:** tekens worden verzameld tot Enter en dan verwerkt (`start`, `stop`, `calibrate`, `set kp 0.05`, …). Zie de [gebruiksaanwijzing](../../gebruiksaanwijzing/) voor de volledige lijst, of typ `help`.
- **Sensoren:** per sensor één ADC-meting (na een weggegooide meting), omgerekend met de kalibratie naar 0 (wit) … 1000 (zwart). De positie is het gewogen gemiddelde: 0 … 7000, midden = 3500.
- **PID-regeling:** wordt uitgevoerd met een vaste cyclustijd (`set cycle`, standaard 3000 µs). Fout = positie − 3500. De output wordt met `diff` verdeeld: het buitenste wiel versnelt met `diff × output`, het binnenste wiel vertraagt met `(1 − diff) × output`. Elke motor wordt begrensd op `max` (6V-motoren op 7,4 V).
- **Kruispunten:** zien 6 of meer sensoren zwart, dan rijdt de robot rechtdoor.
- **Onderbrekingen:** is de lijn minder dan 150 ms weg, dan rijdt de robot rechtdoor; daarna draait hij naar de kant waar de lijn het laatst was, en na 1,5 s zonder lijn stopt hij.
- **Start/stop-knop:** hardware-interrupt op D2 met debounce. Na elke keer opstarten: 1e klik = kalibreren (5 s zwaaien), 2e klik = rijden, daarna wisselt elke klik tussen stop en rijden. Het commando `start` gebruikt de laatst bewaarde kalibratie uit het EEPROM.
- **Debug:** met `debug on` worden 5x per seconde positie, PID-output, PWM links/rechts en de gemeten cyclustijd doorgestuurd (bij stilstand: de sensorwaarden).

## Vaste instellingen (bovenaan in de code)

| constante | betekenis |
|---|---|
| `LIJN_IS_ZWART` | `false` voor een witte lijn op zwarte ondergrond |
| `SENSOR_OMGEKEERD` | `true` als sensor 1 rechts zit |
| `LINKS_OMKEREN`, `RECHTS_OMKEREN` | draairichting van een motor omkeren |
| `ONDERBREKING_MS`, `LIJN_KWIJT_STOP` | tijden voor onderbrekingen en lijn kwijt |
| `KRUISPUNT_SENSOREN` | aantal zwarte sensoren dat als kruispunt geldt |
