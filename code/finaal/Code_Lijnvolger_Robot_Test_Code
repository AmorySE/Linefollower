// Line follower: Arduino Nano + DRV8833 + QTR-8A (8 sensoren) + PID

// ===== Pinnen =====
const int LINKS_1  = 5;    // AIN1
const int LINKS_2  = 6;    // AIN2
const int RECHTS_1 = 9;    // BIN1
const int RECHTS_2 = 10;   // BIN2
const int SENSOR_PIN[8] = {A0, A1, A2, A3, A4, A5, A6, A7};
const int KNOP = 2;        // knop naar GND

// ===== Instellingen =====
const bool LIJN_IS_ZWART    = true;   // false = witte lijn op zwarte ondergrond
const bool SENSOR_OMGEKEERD = false;  // true als sensor 1 rechts zit
const bool LINKS_OMKEREN    = false;  // true als linkerwiel achteruit draait bij "vooruit"
const bool RECHTS_OMKEREN   = false;  // idem voor rechts

const int BASIS_SPEED = 90;    // rijsnelheid (0-255), begin laag
const int MAX_SPEED   = 180;   // limiet i.v.m. 7.4V op 6V-motoren
const int ZOEK_SPEED  = 80;    // draaisnelheid als de lijn kwijt is
const unsigned long LIJN_KWIJT_STOP = 2000;  // ms zonder lijn, daarna stoppen

float Kp = 0.03;
float Ki = 0.0;
float Kd = 0.2;

const bool DEBUG = false;      // true = waarden naar de Serial Monitor (115200 baud)

// ===== Variabelen =====
int minW[8], maxW[8];
int donker[8];                 // 0 = licht, 1000 = lijn
int laatstePositie = 3500;
int laatsteFout = 0;
float integraal = 0;
unsigned long laatstGezien = 0;

// ===== Motoren =====
void motorLinks(int s) {
  if (LINKS_OMKEREN) s = -s;
  s = constrain(s, -MAX_SPEED, MAX_SPEED);
  if (s >= 0) { analogWrite(LINKS_1, s);  analogWrite(LINKS_2, 0); }
  else        { analogWrite(LINKS_1, 0);  analogWrite(LINKS_2, -s); }
}

void motorRechts(int s) {
  if (RECHTS_OMKEREN) s = -s;
  s = constrain(s, -MAX_SPEED, MAX_SPEED);
  if (s >= 0) { analogWrite(RECHTS_1, s); analogWrite(RECHTS_2, 0); }
  else        { analogWrite(RECHTS_1, 0); analogWrite(RECHTS_2, -s); }
}

void stopMotoren() {
  motorLinks(0);
  motorRechts(0);
}

// ===== Sensoren =====
int lees(int pin) {
  analogRead(pin);                       // eerste meting weggooien
  long som = 0;
  for (int i = 0; i < 4; i++) som += analogRead(pin);
  return som / 4;
}

// Vult donker[] met 0..1000 (hoog = lijn)
void leesSensoren() {
  for (int i = 0; i < 8; i++) {
    int pinIndex = SENSOR_OMGEKEERD ? 7 - i : i;
    int v = lees(SENSOR_PIN[pinIndex]);
    int bereik = maxW[pinIndex] - minW[pinIndex];
    if (bereik < 1) bereik = 1;
    long d = (long)(v - minW[pinIndex]) * 1000 / bereik;
    d = constrain(d, 0, 1000);
    donker[i] = LIJN_IS_ZWART ? d : 1000 - d;
  }
}

// Positie 0..7000 (3500 = midden). lijn = false als niets de lijn ziet.
int leesPositie(bool &lijn) {
  leesSensoren();
  long som = 0, gewogen = 0;
  int hoogste = 0;
  for (int i = 0; i < 8; i++) {
    if (donker[i] > hoogste) hoogste = donker[i];
    gewogen += (long)donker[i] * i * 1000;
    som += donker[i];
  }
  lijn = (hoogste > 200 && som > 0);
  if (!lijn) return laatstePositie;
  laatstePositie = gewogen / som;
  return laatstePositie;
}

// ===== Knop en kalibratie =====
void wachtOpKnop() {
  while (digitalRead(KNOP) == HIGH) delay(10);   // wacht op indrukken
  delay(50);
  while (digitalRead(KNOP) == LOW)  delay(10);   // wacht op loslaten
  delay(100);
}

// Beweeg de robot met de hand 5 seconden heen en weer over de lijn
void kalibreer() {
  for (int i = 0; i < 8; i++) { minW[i] = 1023; maxW[i] = 0; }
  unsigned long start = millis();
  while (millis() - start < 5000) {
    for (int i = 0; i < 8; i++) {
      int v = lees(SENSOR_PIN[i]);
      if (v < minW[i]) minW[i] = v;
      if (v > maxW[i]) maxW[i] = v;
    }
    digitalWrite(LED_BUILTIN, (millis() / 100) % 2);
  }
  digitalWrite(LED_BUILTIN, LOW);
}

void stopEnWacht() {
  stopMotoren();
  integraal = 0;
  laatsteFout = 0;
  digitalWrite(LED_BUILTIN, HIGH);
  wachtOpKnop();                          // druk opnieuw om weer te starten
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
  laatstGezien = millis();
}

// ===== Setup en loop =====
void setup() {
  pinMode(LINKS_1, OUTPUT);
  pinMode(LINKS_2, OUTPUT);
  pinMode(RECHTS_1, OUTPUT);
  pinMode(RECHTS_2, OUTPUT);
  pinMode(KNOP, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  stopMotoren();
  if (DEBUG) Serial.begin(115200);

  // Stap 1: druk op de knop, beweeg de robot 5 s met de hand over de lijn
  digitalWrite(LED_BUILTIN, HIGH);
  wachtOpKnop();
  kalibreer();

  // Stap 2: zet de robot op de lijn en druk op de knop om te starten
  digitalWrite(LED_BUILTIN, HIGH);
  wachtOpKnop();
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
  laatstGezien = millis();
}

void loop() {
  // Noodstop: knop tijdens het rijden
  if (digitalRead(KNOP) == LOW) {
    while (digitalRead(KNOP) == LOW) delay(10);
    stopEnWacht();
    return;
  }

  bool lijn;
  int positie = leesPositie(lijn);

  if (!lijn) {
    // Lijn kwijt: draai naar de kant waar hij het laatst was
    if (laatstePositie < 3500) {
      motorLinks(-ZOEK_SPEED);  motorRechts(ZOEK_SPEED);
    } else {
      motorLinks(ZOEK_SPEED);   motorRechts(-ZOEK_SPEED);
    }
    if (millis() - laatstGezien > LIJN_KWIJT_STOP) stopEnWacht();
    return;
  }

  laatstGezien = millis();

  int fout = positie - 3500;
  integraal += fout;
  integraal = constrain(integraal, -5000, 5000);
  float afgeleide = fout - laatsteFout;
  laatsteFout = fout;

  float correctie = Kp * fout + Ki * integraal + Kd * afgeleide;

  motorLinks(BASIS_SPEED + correctie);
  motorRechts(BASIS_SPEED - correctie);

  if (DEBUG) {
    Serial.print(positie); Serial.print("\t");
    Serial.println(correctie);
  }
}
