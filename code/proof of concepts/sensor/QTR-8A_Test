// Test QTR-8A met kalibratie: 0 = wit, 1000 = zwart, plus lijnpositie 0..7000
// Bij de start: houd de sensor 2-3 mm boven de grond en beweeg 5 s over wit EN zwart.
// Serial Monitor: 115200 baud

const int PINNEN[8] = {A0, A1, A2, A3, A4, A5, A6, A7};
const bool PLOTTER = false;   // true = bruikbaar in Tools > Serial Plotter

int minW[8], maxW[8];

int lees(int pin) {
  analogRead(pin);                 // eerste meting weggooien
  long som = 0;
  for (int i = 0; i < 8; i++) som += analogRead(pin);
  return som / 8;
}

void padPrint(int v) {             // getal netjes uitgelijnd (4 tekens)
  if (v < 1000) Serial.print(" ");
  if (v < 100)  Serial.print(" ");
  if (v < 10)   Serial.print(" ");
  Serial.print(v);
}

void kalibreer() {
  for (int i = 0; i < 8; i++) { minW[i] = 1023; maxW[i] = 0; }
  Serial.println("Kalibreren (5 sec): beweeg de sensor over wit EN zwart, 2-3 mm boven de grond");
  unsigned long start = millis();
  while (millis() - start < 5000) {
    for (int i = 0; i < 8; i++) {
      int v = lees(PINNEN[i]);
      if (v < minW[i]) minW[i] = v;
      if (v > maxW[i]) maxW[i] = v;
    }
  }
  for (int i = 0; i < 8; i++) {
    if (maxW[i] - minW[i] < 100) {
      Serial.print("Let op: sensor ");
      Serial.print(i + 1);
      Serial.println(" reageert nauwelijks (contact, afstand of soldeerverbinding?)");
    }
  }
  Serial.println("Klaar. 0 = wit, 1000 = zwart\n");
}

void setup() {
  Serial.begin(115200);
  delay(2000);                     // tijd om de sensor boven het papier te houden
  kalibreer();
}

void loop() {
  int donker[8];
  long som = 0, gewogen = 0;
  bool lijn = false;

  for (int i = 0; i < 8; i++) {
    int v = lees(PINNEN[i]);
    int bereik = maxW[i] - minW[i];
    if (bereik < 1) bereik = 1;
    donker[i] = constrain((long)(v - minW[i]) * 1000 / bereik, 0, 1000);
    if (donker[i] > 500) lijn = true;
    gewogen += (long)donker[i] * i * 1000;
    som += donker[i];
  }

  if (PLOTTER) {
    for (int i = 0; i < 8; i++) { Serial.print(donker[i]); Serial.print(i < 7 ? "," : "\n"); }
    delay(50);
    return;
  }

  // Balk: elk teken is een sensor
  Serial.print("[");
  for (int i = 0; i < 8; i++) {
    char c = '.';
    if (donker[i] > 750) c = '#';
    else if (donker[i] > 500) c = 'O';
    else if (donker[i] > 250) c = 'o';
    Serial.print(c);
  }
  Serial.print("]  ");

  for (int i = 0; i < 8; i++) { padPrint(donker[i]); Serial.print(" "); }

  Serial.print("  positie: ");
  if (lijn && som > 0) Serial.println(gewogen / som);   // 0..7000, midden = 3500
  else Serial.println("geen lijn");
  delay(150);
}
