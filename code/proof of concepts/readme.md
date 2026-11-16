# Proof of Concepts

Een proof of concept (PoC) is een combinatie van minimale hardware en software waarmee wordt aangetoond dat een onderdeel van het project werkt. Een PoC kan dienen als eerste test van een onderdeel, maar ook helpen bij het debuggen van het uiteindelijke project.

De PoC's in deze map testen afzonderlijke onderdelen van de linefollower. Ze zijn bedoeld om de werking van die onderdelen eenvoudig te demonstreren; ze vormen niet noodzakelijk samen een volledige linefollower.

## Overzicht

| Onderdeel | Doel |
| --- | --- |
| [H-brug](hbridge/) | Aantonen dat twee DC-motoren onafhankelijk kunnen draaien en dat hun snelheid en draairichting regelbaar zijn. |
| [Sensoren](sensor/) | Aantonen dat meerdere sensoren onafhankelijk uitgelezen kunnen worden. |
| [Start/stop-interrupt](interrupt/) | De werking van een start/stop-knop testen met een hardware-interrupt. |
| [Draadloze communicatie](wireless%20communication/) | Duplexcommunicatie tussen een microcontroller en een smartphone testen met een HC-06 Bluetoothmodule. |

## Wat bevat een proof of concept?

Waar van toepassing bevat een PoC:

- een elektronisch schema;
- minimale code om de vooropgestelde functionaliteit te demonstreren;
- een korte uitleg, testresultaten en eventuele aandachtspunten.

De schakelingen komen bij voorkeur overeen met die van de uiteindelijke linefollower. Zo kan de code, waar mogelijk, zonder aanpassingen in het finale project gebruikt worden. Raadpleeg de README in de map van een PoC voor de specifieke aansluitingen, code en testinstructies.
