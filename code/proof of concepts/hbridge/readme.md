# H-Bridge proof of concept

minimale hard- & software + stappenplan dat aantoont dat 2 motoren onafhankelijk van elkaar kunnen draaien, en (traploos) regelbaar zijn in snelheid en draairichting.
**Functionaliteit**
Het geschreven programma zal in een loop demonstreren hoe de motoren geleidelijk aan versnellen tot het maximale snelheid, en vervolgens weer vertragen naar 0. Vervolgens wordt gedemonstreerd dat ze ook in tegengestelde richting kunnen draaien, en ook afzonderlijk van elkaar een andere draairichting kunnen aannemen.
**Arduino code**

const int LINKS_1 = 5;   // AIN1
const int LINKS_2 = 6;   // AIN2

const int RECHTS_1 = 9;  // BIN1
const int RECHTS_2 = 10; // BIN2

const int MAX_SPEED = 255;
const int STAP = 5;
const int VERTRAGING = 40;

// Linkermotor aansturen
void motorLinks(int snelheid) {
  if (snelheid >= 0) {
    analogWrite(LINKS_1, snelheid);
    analogWrite(LINKS_2, 0);
  } else {
    analogWrite(LINKS_1, 0);
    analogWrite(LINKS_2, -snelheid);
  }
}

// Rechtermotor aansturen
void motorRechts(int snelheid) {
  if (snelheid >= 0) {
    analogWrite(RECHTS_1, snelheid);
    analogWrite(RECHTS_2, 0);
  } else {
    analogWrite(RECHTS_1, 0);
    analogWrite(RECHTS_2, -snelheid);
  }
}

// Beide motoren stoppen
void stopMotoren() {
  motorLinks(0);
  motorRechts(0);
}

// Geleidelijk versnellen en vertragen
void ramp(int richtingLinks, int richtingRechts) {
  // Versnellen van 0 naar maximale snelheid
  for (int snelheid = 0; snelheid <= MAX_SPEED; snelheid += STAP) {
    motorLinks(snelheid * richtingLinks);
    motorRechts(snelheid * richtingRechts);
    delay(VERTRAGING);
  }

  // Vertragen van maximale snelheid naar 0
  for (int snelheid = MAX_SPEED; snelheid >= 0; snelheid -= STAP) {
    motorLinks(snelheid * richtingLinks);
    motorRechts(snelheid * richtingRechts);
    delay(VERTRAGING);
  }

  stopMotoren();
  delay(1000);
}

void setup() {
  pinMode(LINKS_1, OUTPUT);
  pinMode(LINKS_2, OUTPUT);
  pinMode(RECHTS_1, OUTPUT);
  pinMode(RECHTS_2, OUTPUT);

  stopMotoren();
}

void loop() {
  // 1. Beide motoren vooruit
  ramp(1, 1);

  // 2. Beide motoren achteruit
  ramp(-1, -1);

  // 3. Linkermotor vooruit, rechtermotor achteruit
  ramp(1, -1);

  // 4. Linkermotor achteruit, rechtermotor vooruit
  ramp(-1, 1);
}

**Elektronisch schema**
<img width="978" height="435" alt="image" src="https://github.com/user-attachments/assets/600df93a-4e3e-4253-8c35-007b93db7134" />

**Testresultaten**
