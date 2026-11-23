# Instructable

Een instructable is een stappenplan - zonder verdere uitleg - hoe je vertrekkend van de bill of materials en gebruik makend van de technische tekeningen de robot kan nabouwen. Ook de nodige stappen om de microcontroller te compileren en te uploaden staan beschreven.

**Nodige documenten**

- [Bill of materials](../bill%20of%20materials/)
- [Elektronisch schema](../technische%20tekeningen/elektronisch/)
- [Mechanische tekening (frame)](../technische%20tekeningen/mechanisch/)
- [Datasheets](../datasheets/)
- [Finale code](../code/finaal/)
- [Gebruiksaanwijzing](../gebruiksaanwijzing/)

**Gereedschap:** soldeerbout en soldeertin, kniptang, striptang, kleine schroevendraaier, multimeter, 3D-printer, pc met Arduino IDE, smartphone (Android) met Serial Bluetooth Terminal.

### stap 1: onderdelen bestellen
Bestel alle onderdelen uit de [bill of materials](../bill%20of%20materials/).

### stap 2: frame printen
1. Open [frame.ipt](../technische%20tekeningen/mechanisch/frame.ipt) in Autodesk Inventor.
2. Exporteer als STL (File → Export → CAD Format → STL).
3. Print het frame in PLA.
4. Verwijder de support en controleer of de batterijhouder en de motorbeugels passen.

### stap 3: modules solderen
1. Soldeer de header-pinnen aan de DRV8833-module.
2. Soldeer de header-pinnen aan de QTR-8A.
3. Controleer de soldeerverbindingen met de multimeter (geen kortsluiting tussen naburige pinnen).

### stap 4: onderdelen apart testen (proofs of concept)
Installeer de Arduino IDE en stel in: **Tools → Board → Arduino Nano**, **Processor → ATmega328P (Old Bootloader)**, juiste **Port**. Test elk onderdeel apart volgens de readme van de proof of concept:

1. [H-brug](../code/proof%20of%20concepts/hbridge/): beide motoren vooruit en achteruit, regelbare snelheid.
2. [Sensoren](../code/proof%20of%20concepts/sensor/): 8 sensoren reageren op wit en zwart.
3. [Start/stop interrupt](../code/proof%20of%20concepts/interrupt/): elke druk op de knop wordt geteld.
4. [Draadloze communicatie](../code/proof%20of%20concepts/wireless%20communication/): berichten in beide richtingen tussen Nano en smartphone.

### stap 5: mechanische montage
1. Monteer de N20-motoren in de motorbeugels en bevestig de beugels aan het frame.
2. Druk de wielen op de D-as van de motoren.
3. Kleef het hulpwiel onder het frame.
4. Bevestig de QTR-8A vooraan onder het frame, sensoren naar beneden, ±3 mm boven de grond, sensor 1 aan de linkerkant (rijrichting).
5. Plaats de batterijhouder (2x 18650) in het vak van het frame.

### stap 6: elektronica bedraden
Bedraad alles volgens het [elektronisch schema](../technische%20tekeningen/elektronisch/):

| van | naar |
|---|---|
| batterijhouder + | VM (DRV8833-module) en VIN (Nano) |
| batterijhouder − | GND van alle onderdelen |
| Nano +5V | STBY (DRV8833-module), VCC (QTR-8A), VCC (HC-06), led + in de drukknop |
| Nano D5 / D6 | AIN1 / AIN2 |
| Nano D9 / D10 | BIN1 / BIN2 |
| AO1 / AO2 | motor 1 + / − |
| BO1 / BO2 | motor 2 + / − |
| Nano A0 … A7 | QTR-8A sensor 1 … 8 |
| Nano D2 | drukknop (C), drukknop (NO) naar GND |
| Nano D0/RX | HC-06 TX |
| Nano D1/TX | R1 1 kΩ → HC-06 RX; HC-06 RX → R2 1 kΩ → R3 1 kΩ → GND |

1. Plaats de Nano, de DRV8833-module en de HC-06 in het elektronicavak.
2. Monteer de metalen drukknop met led op het frame.
3. Controleer met de multimeter (batterijen eruit) dat + en GND niet kortgesloten zijn.

### stap 7: finale code uploaden
1. Koppel de HC-06 los (D0/D1 delen de USB-verbinding).
2. Open [Code_Lijnvolger_Robot.ino](../code/finaal/Code_Lijnvolger_Robot/Code_Lijnvolger_Robot.ino) in de Arduino IDE.
3. Kies **Arduino Nano**, **ATmega328P (Old Bootloader)** en de juiste poort.
4. Klik op **Verify** en daarna op **Upload**.
5. Koppel de USB-kabel los en sluit de HC-06 terug aan.

### stap 8: batterijen plaatsen en inschakelen
1. Laad de 18650-batterijen op met de USB-lader.
2. Plaats ze in de batterijhouder (let op + en −).
3. Zet de aan/uit-schakelaar op ON: de led in de drukknop brandt.

### stap 9: bluetooth verbinden
1. Installeer **Serial Bluetooth Terminal** op de smartphone.
2. Koppel de HC-06 (pincode 1234 of 0000).
3. Verbind in de app met de HC-06 en stel **Newline → CR+LF** in.
4. Typ `help`: de lijst met commando's verschijnt.

### stap 10: kalibreren en rijden
1. Druk op de knop (of typ `calibrate`) en beweeg de robot 5 s heen en weer over de lijn.
2. Zet de robot op de lijn en druk opnieuw op de knop (of typ `start`).
3. Controleer de rijrichting: draait een wiel verkeerd, zet dan `LINKS_OMKEREN` of `RECHTS_OMKEREN` op `true` in de code en upload opnieuw (stap 7).
4. Stel de snelheid en de PID-regeling af met `set power`, `set kp`, `set kd` (zie [gebruiksaanwijzing](../gebruiksaanwijzing/)). De instellingen worden automatisch in het EEPROM bewaard.
