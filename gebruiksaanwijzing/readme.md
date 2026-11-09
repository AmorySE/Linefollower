# Gebruiksaanwijzing

### opladen / vervangen batterijen
de 2x 18650 batterijen kunnen gewoon vervangen worden door de oude batterijen uit de houder te halen en nieuwe in te steken

### draadloze communicatie
#### verbinding maken
Schakel de batterijschakelaar op ON om het systeem onder spanning te zetten. Het ledje op de HC-06-module zal snel knipperen. Installeer een seriële communicatie-app, bijvoorbeeld Serial Bluetooth Terminal (beschikbaar in de Google Play Store). Maak hiermee verbinding met de bluetoothmodule (Stel zeker de juiste baudrate in). Zodra dit gelukt is, knippert het ledje trager. De microcontroller kan nu draadloos commando’s ontvangen.

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

### start/stop button
uitleg locatie + werking start/stop button
