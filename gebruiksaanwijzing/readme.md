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
Zet de aan/uit-schakelaar op de batterijhouder op ON om het systeem onder spanning te zetten (de led in de start/stop-knop brandt). Het ledje op de HC-06-module zal snel knipperen. Installeer een seriële communicatie-app, bijvoorbeeld Serial Bluetooth Terminal (beschikbaar in de Google Play Store). Maak hiermee verbinding met de bluetoothmodule (Stel zeker de juiste baudrate in). Zodra dit gelukt is, knippert het ledje trager. De microcontroller kan nu draadloos commando’s ontvangen.

#### commando's
debug [on/off]  
start  
stop  
set cycle [µs]  
set power [0..255]  
set diff [0..1]  
set kp [0..]  
set ki [0..]  
set kd [0..]  
calibrate black  
calibrate white  

### kalibratie
De sensoren meten waardes tussen 0 en 1000 om onderscheid te maken tussen wit en zwart. Deze waardes kunnen variëren door lichtinval, reflecties of andere storingen. Voor een betrouwbare werking moet de robot opnieuw gekalibreerd worden bij een verandering van omgeving of ondergrond. 

### settings
De robot rijdt stabiel met volgende parameters:  

### aan/uit-schakelaar
De aan/uit-schakelaar zit op de batterijhouder (2x 18650). Hij schakelt de volledige robot (motoren en Arduino Nano) in of uit. Brandt de led in de start/stop-knop, dan staat de robot aan.

### start/stop button
De start/stop-knop is een metalen drukknop met ingebouwde led, aangesloten tussen D2 en GND (interne pull-up, geen externe weerstand). De led in de knop is de power-on-led.

Werking:
1. Na het inschakelen brandt de led op de Nano (D13): druk op de knop om de kalibratie te starten.
2. Beweeg de robot gedurende 5 seconden met de hand heen en weer over de lijn (sensoren over wit én zwart).
3. De led brandt opnieuw: zet de robot op de lijn en druk op de knop. De robot begint te rijden.
4. Druk tijdens het rijden op de knop om te stoppen; de led brandt. Druk opnieuw om verder te rijden.

Verliest de robot de lijn langer dan 2 seconden, dan stopt hij ook vanzelf; druk dan op de knop om opnieuw te starten.
