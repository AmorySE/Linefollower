# Finale code

**Bestand:** [Code_Lijnvolger_Robot/Code_Lijnvolger_Robot.ino](Code_Lijnvolger_Robot/Code_Lijnvolger_Robot.ino)

Het bestand staat in een map met dezelfde naam, zodat het rechtstreeks in de Arduino IDE opent.

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
| start/stop-knop (naar GND, `INPUT_PULLUP`) | D2 |
| statusled | D13 (ingebouwde led) |

STBY van de DRV8833-module hangt vast aan +5V: de driver staat altijd aan en wordt niet vanuit de code aangestuurd.

## Werking

1. **Start:** de led op D13 brandt. Druk op de knop.
2. **Kalibratie (5 s):** beweeg de robot met de hand over de lijn. Per sensor wordt de laagste (wit) en hoogste (zwart) waarde bijgehouden; daarna wordt elke sensor omgerekend naar 0 … 1000.
3. **Rijden:** zet de robot op de lijn en druk op de knop. Uit de 8 sensoren wordt de positie van de lijn berekend (0 … 7000, midden = 3500). Een PID-regelaar stuurt het snelheidsverschil tussen links en rechts.
4. **Stop:** druk tijdens het rijden op de knop. Opnieuw drukken = verder rijden.
5. **Lijn kwijt:** de robot draait naar de kant waar de lijn het laatst was. Na 2 s zonder lijn stopt hij en wacht op de knop.

## Instellingen (bovenaan in de code)

| constante | betekenis |
|---|---|
| `Kp`, `Ki`, `Kd` | PID-parameters |
| `BASIS_SPEED` | rijsnelheid (0 … 255) |
| `MAX_SPEED` | begrenzing (180) omdat de 6V-motoren op 7,4 V draaien |
| `ZOEK_SPEED` | draaisnelheid als de lijn kwijt is |
| `LIJN_IS_ZWART` | `false` voor een witte lijn op zwarte ondergrond |
| `SENSOR_OMGEKEERD` | `true` als sensor 1 rechts zit |
| `LINKS_OMKEREN`, `RECHTS_OMKEREN` | draairichting van een motor omkeren |
| `DEBUG` | `true` = positie en correctie naar de seriële poort (115200 baud) |
