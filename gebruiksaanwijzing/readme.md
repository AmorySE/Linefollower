# Gebruiksaanwijzing

### opladen / vervangen batterijen
De robot werkt op 2 oplaadbare 18650-batterijen (Li-ion) in de batterijhouder.

Opladen:
1. Zet de aan/uit-schakelaar op de batterijhouder op OFF.
2. Haal de 2 batterijen uit de houder.
3. Steek ze in de USB-lader voor 18650 (let op + en −) en sluit de lader aan op een USB-poort of -adapter.
4. Wacht tot het lampje op de lader aangeeft dat de batterijen vol zijn.
5. Steek de batterijen terug in de houder (let op + en −).

Laad de batterijen op zodra de robot merkbaar trager rijdt of de Arduino herstart: onder ongeveer 7 V krijgt de Nano te weinig spanning via VIN. Vervang ze enkel als ze beschadigd zijn of niet meer opladen.

### draadloze communicatie
#### verbinding maken
Zet de aan/uit-schakelaar op de batterijhouder op ON om het systeem onder spanning te zetten (de led in de start/stop-knop brandt). Het ledje op de HC-06-module knippert snel. Installeer een seriële communicatie-app, bijvoorbeeld Serial Bluetooth Terminal (Google Play Store), en koppel de HC-06 (pincode 1234 of 0000). Maak verbinding; zodra dit gelukt is, knippert het ledje trager. Stel in de app in dat elk bericht wordt afgesloten met een Enter (CR+LF). De baudrate van de HC-06 is 9600.

Typ `help` om de lijst met commando's te zien en `info` om alle instellingen te bekijken.

#### commando's

| commando | functie |
|---|---|
| `start` | robot start met rijden |
| `stop` | robot stopt |
| `calibrate` | kalibratie: beweeg de robot 5 s heen en weer over de lijn |
| `calibrate white` | kalibratie wit: zet alle sensoren boven wit papier |
| `calibrate black` | kalibratie zwart: zet alle sensoren boven een zwart vlak |
| `set power [0..255]` | basissnelheid |
| `set max [0..255]` | maximale snelheid per motor |
| `set diff [0..1]` | verdeling van de bijsturing: deel dat het buitenste wiel versnelt (rest: binnenste wiel vertraagt) |
| `set kp [..]` | proportionele factor PID |
| `set ki [..]` | integrerende factor PID |
| `set kd [..]` | differentiërende factor PID |
| `set cycle [µs]` | cyclustijd van de regeling (1000 … 100000 µs) |
| `debug on` / `debug off` | meetwaarden doorsturen (5x per seconde) |
| `info` | alle instellingen en de kalibratie tonen |
| `sensors` | sensorwaarden (ruw en 0 … 1000) en positie tonen |
| `reset` | standaardinstellingen herstellen |
| `help` | lijst met commando's |

Hoofdletters maken niet uit. Elke wijziging wordt meteen in het EEPROM bewaard: na het uitschakelen start de robot op met de laatst opgeslagen instellingen en kalibratie.

### kalibratie
De sensoren meten waardes tussen 0 en 1000 om onderscheid te maken tussen wit en zwart. Deze waardes kunnen variëren door lichtinval (zonlicht, tl-lampen), reflecties of andere storingen. Kalibreer daarom opnieuw bij een verandering van omgeving of ondergrond.

Twee manieren:
- **Zwaaien:** typ `calibrate` (of druk na het inschakelen een eerste keer op de knop) en beweeg de robot 5 seconden heen en weer over de lijn, 2-3 mm boven het papier. De led op de Nano knippert tijdens het kalibreren.
- **Wit en zwart apart:** zet de robot met alle sensoren op wit papier en typ `calibrate white`; zet hem daarna op een zwart vlak en typ `calibrate black`.

Ziet een sensor te weinig verschil tussen wit en zwart, dan meldt de robot dit en blijft hij als niet gekalibreerd staan.

### settings
Standaardwaarden (na `reset`): power 90, max 180, diff 0,5, kp 0,03, ki 0, kd 0,2, cycle 3000 µs.

De robot rijdt stabiel met volgende parameters:

### aan/uit-schakelaar
De aan/uit-schakelaar zit op de batterijhouder (2x 18650). Hij schakelt de volledige robot (motoren en Arduino Nano) in of uit. Brandt de led in de start/stop-knop, dan staat de robot aan.

### start/stop button
De start/stop-knop is een metalen drukknop met ingebouwde led. Hij is met draden aangesloten op het breadboard van de robot: de schakelaar tussen D2 en GND (interne pull-up, hardware-interrupt), de led tussen +5V en GND. De led in de knop is de power-on-led.

Werking (na elke keer inschakelen):
1. **1e klik: kalibreren.** Beweeg de robot 5 seconden heen en weer over de lijn; de led op de Nano (D13) knippert.
2. **2e klik: rijden.** Zet de robot op de lijn en druk op de knop: de robot rijdt en de led op de Nano brandt.
3. **Volgende klikken:** wisselen tussen stoppen (led dooft) en verder rijden.

Na het uit- en inschakelen begint het opnieuw bij stap 1, zodat de robot altijd gekalibreerd is voor het licht van dat moment. De andere instellingen (snelheid, PID, …) blijven bewaard in het EEPROM. Via bluetooth start `start` meteen met de laatst bewaarde kalibratie.

Op kruispunten rijdt de robot rechtdoor. Korte onderbrekingen in de lijn overbrugt hij door rechtdoor te rijden. Verliest de robot de lijn langer dan 1,5 seconden, dan stopt hij vanzelf; druk dan op de knop (of typ `start`) om opnieuw te starten.
