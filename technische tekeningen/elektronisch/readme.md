# Elektronisch schema

[Open het aansluitschema (SVG)](linefollower_aansluitschema.svg).

De tekening combineert de bestaande PoC-pinout voor de DRV8833, QTR-8A en HC-06 met de onderdelen uit de BOM. De DRV8833 heeft geen aparte `VCC`- of `EN`-pinnen nodig in deze aansluiting: motorvoeding gaat naar `VM`, de Nano stuurt de vier `IN`-pinnen en `nSLEEP` wordt hoog gemaakt.

**Let op:** de BOM bevat nog geen 5 V- en 6 V-buckconverter, schakelaar of zekering. De tekening toont die voedingen als benodigde onderdelen: 5 V voor logica/sensor/HC-06 en een gereguleerde 6 V voor de 6 V-motoren. Voeg deze onderdelen toe aan de BOM voordat je bouwt. Stel de motorvoeding in op 6,0 V en controleer dat de stallstroom van de motoren binnen de limiet van jouw DRV8833-module blijft. Controleer ook de exacte pinlabels en voedingsspecificaties van jouw HC-06-breakout en drivercarrier.

De 3 × 1 kΩ in de bestaande BOM worden gebruikt voor de HC-06 RXD-spanningsdeler: één weerstand in serie vanaf Nano D12 en twee weerstanden in serie van het knooppunt naar GND. Dit verlaagt het 5 V-signaal tot circa 3,3 V.
