# start/stop interrupt proof of concept
minimale hard- en software die de correcte werking van een start/stop drukknop aantoont, gebruik makend van een hardware interrupt

**Functionaliteit**

Een drukknop op D2 (interrupt-pin INT0 van de Nano) start een interrupt-routine (`knopISR`) op het moment dat de knop ingedrukt wordt. De routine telt de drukken en wordt softwarematig ontdenderd (debounce van 200 ms). In de `loop` wisselt de ingebouwde led (D13) bij elke druk van toestand (aan/uit = start/stop) en verschijnt het aantal drukken in de Serial Monitor.

**Arduino code**

zie bestand [Interrupt.ino](Interrupt.ino)

**Benodigdheden**

- Arduino Nano V3
- drukknop 6x6 mm
- dupont-draden

**Aansluitingen**

| drukknop | Arduino Nano |
|---|---|
| pin 1 | D2 |
| pin 2 | GND |

Er is **geen externe weerstand** nodig: D2 staat in de code op `INPUT_PULLUP`, waardoor de interne pull-upweerstand (±20–50 kΩ) van de ATmega328P de ingang naar 5 V trekt.

- knop niet ingedrukt → D2 = HIGH
- knop ingedrukt → D2 verbonden met GND → LOW

De interrupt reageert daarom op de dalende flank (`FALLING`):

```cpp
pinMode(KNOP, INPUT_PULLUP);
attachInterrupt(digitalPinToInterrupt(KNOP), knopISR, FALLING);
```

**Stappenplan**

1. Sluit de drukknop aan tussen D2 en GND.
2. Upload `Interrupt.ino` en open de Serial Monitor op **115200 baud**.
3. Druk op de knop: de led op de Nano gaat aan (start) en de teller verschijnt in de Serial Monitor.
4. Druk opnieuw: de led gaat uit (stop) en de teller loopt op.

**Testresultaten**

Bij elke druk verschijnt in de Serial Monitor:

```
Interrupt-test gestart: druk op de knop
Knop gedrukt, totaal: 1
Knop gedrukt, totaal: 2
```

Eén druk geeft telkens één melding: het ontdenderen werkt.
