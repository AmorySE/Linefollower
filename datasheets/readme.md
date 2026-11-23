# Datasheets

Datasheets van de componenten die in dit project gebruikt worden.

| onderdeel | datasheet | belangrijk voor dit project |
|---|---|---|
| Arduino Nano V3 (kloon, CH340) | [Arduino Nano datasheet](Arduino_nano.pdf), [pinout](Arduino_nano_pin.png) | ATmega328P, 5 V-logica; VIN 7–12 V; PWM op D3, D5, D6, D9, D10, D11; interrupt op D2 (INT0); analoge ingangen A0–A7 (10-bit ADC); D0/D1 = hardware-serieel (gedeeld met USB) |
| DRV8833-module | [DRV8833 datasheet (TI)](drv8833.pdf), [pinout module](DRV8833_pin.png) | dubbele H-brug; VM 2,7–10,8 V; 1,5 A RMS (2 A piek) per H-brug; STBY op de module = nSLEEP van de chip (hoog = actief); AIN1/AIN2 en BIN1/BIN2 met PWM |
| QTR-8A | [QTR-8x datasheet (Pololu)](QTR-8x.pdf) | 8 analoge reflectiesensoren (lage spanning = wit, hoge spanning = zwart); voeding 3,3–5 V; LEDON intern opgetrokken (los = leds aan); optimale afstand ±3 mm |
| HC-06 | [HC-06 datasheet](HC-06.pdf) | bluetooth-slavemodule; voeding module 3,6–6 V; RX/TX op 3,3 V-niveau (spanningsdeler op RX); standaard 9600 baud |
| Micro Metal Gearmotor (N20, 6 V, 300 rpm) | [Pololu Micro Metal Gearmotors](pololu-micro-metal-gearmotors-rev-6-1.pdf) | nominaal 6 V (op 7,4 V: snelheid begrensd in de code); afmetingen voor het frame; D-as voor de wielen |
| 18650 Li-ion-cel | [18650 datasheet](18650.pdf) | 3,7 V nominaal, 4,2 V vol, ±3,0 V leeg; 2 in serie = 7,4 V (8,4 V vol); opladen met een 18650-lader |
