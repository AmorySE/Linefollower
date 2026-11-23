// Line follower: Arduino Nano + DRV8833-module + QTR-8A (8 sensoren) + HC-06 + PID
//
// - Bediening via bluetooth (HC-06 op D0/D1, 9600 baud) met een seriele terminal-app
//   op smartphone of pc. Typ "help" voor alle commando's.
// - Alle instellingen en de kalibratie worden in het EEPROM bewaard; bij het opstarten
//   worden de laatst opgeslagen instellingen geladen.
// - Start/stop-knop op D2 (naar GND, INPUT_PULLUP) via een hardware-interrupt.
//   Na elke keer opstarten: 1e klik = kalibreren (5 s zwaaien), 2e klik = rijden,
//   daarna wisselt elke klik tussen stop en rijden.
// - STBY (nSLEEP) van de DRV8833-module hangt vast aan +5V (zie schema): de driver staat
//   altijd aan, daarom wordt STBY niet vanuit de code aangestuurd.
// - Koppel de HC-06 los tijdens het uploaden (D0/D1 delen de USB-verbinding).

#include <EEPROM.h>

// ===== Pinnen =====
const int LINKS_1  = 5;    // AIN1
const int LINKS_2  = 6;    // AIN2
const int RECHTS_1 = 9;    // BIN1
const int RECHTS_2 = 10;   // BIN2
const int SENSOR_PIN[8] = {A0, A1, A2, A3, A4, A5, A6, A7};
const int KNOP = 2;        // knop naar GND (interrupt INT0)
const int STATUS_LED = LED_BUILTIN;   // D13: aan = rijden, knipperen = kalibreren

// ===== Vaste instellingen (hardware) =====
const bool LIJN_IS_ZWART    = true;   // false = witte lijn op zwarte ondergrond
const bool SENSOR_OMGEKEERD = false;  // true als sensor 1 rechts zit
const bool LINKS_OMKEREN    = false;  // true als linkerwiel achteruit draait bij "vooruit"
const bool RECHTS_OMKEREN   = false;  // idem voor rechts

const long BAUD = 9600;                       // standaardbaudrate HC-06
const unsigned long ONDERBREKING_MS = 150;    // lijn even kwijt (onderbreking): rechtdoor blijven rijden
const unsigned long LIJN_KWIJT_STOP = 1500;   // zo lang zonder lijn -> stoppen
const int KRUISPUNT_SENSOREN = 6;             // zoveel sensoren op zwart = kruispunt -> rechtdoor
const unsigned long DEBOUNCE_MS = 250;
const unsigned long DEBUG_INTERVAL_MS = 200;

// ===== Instellingen (aanpasbaar via bluetooth, bewaard in EEPROM) =====
const uint16_t EEPROM_MAGIC = 0x4C46;   // "LF": geldige instellingen aanwezig
const uint8_t  EEPROM_VERSIE = 1;

struct Instellingen {
  uint16_t magic;
  uint8_t  versie;
  int      power;          // basissnelheid 0..255
  int      maxSpeed;       // maximale PWM per motor 0..255
  float    diff;           // 0..1: deel van de correctie op het buitenste wiel
  float    kp, ki, kd;     // PID
  unsigned long cycle;     // cyclustijd regeling in microseconden
  bool     debug;          // waarden doorsturen
  bool     gekalibreerd;
  int      wit[8];         // kalibratie: waarde op wit
  int      zwart[8];       // kalibratie: waarde op zwart
};

Instellingen inst;

void controleerKalibratie();

// ===== Variabelen =====
int ruw[8];                // laatst gemeten ADC-waarden 0..1023
int donker[8];             // 0 = wit, 1000 = zwart (na kalibratie)
bool rijden = false;
bool sessieGekalibreerd = false;   // sinds het opstarten al gekalibreerd (knop: 1e klik = kalibreren)
int laatstePositie = 3500;
float laatsteFout = 0;
float integraal = 0;
float laatsteOutput = 0;
int pwmLinks = 0, pwmRechts = 0;
unsigned long laatstGezien = 0;
unsigned long vorigeCyclus = 0;
unsigned long gemetenCyclus = 0;
unsigned long vorigeDebug = 0;

volatile bool knopGedrukt = false;
volatile unsigned long laatsteKnop = 0;

char cmdBuffer[40];
byte cmdLengte = 0;

// ===== EEPROM =====
void standaardInstellingen() {
  inst.magic = EEPROM_MAGIC;
  inst.versie = EEPROM_VERSIE;
  inst.power = 90;
  inst.maxSpeed = 180;     // begrenzing: 6V-motoren op 7,4 V
  inst.diff = 0.5;
  inst.kp = 0.03;
  inst.ki = 0.0;
  inst.kd = 0.2;
  inst.cycle = 3000;
  inst.debug = false;
  inst.gekalibreerd = false;
  for (int i = 0; i < 8; i++) { inst.wit[i] = 0; inst.zwart[i] = 1023; }
}

void bewaar() {
  EEPROM.put(0, inst);     // put schrijft enkel bytes die veranderd zijn
}

void laadInstellingen() {
  EEPROM.get(0, inst);
  if (inst.magic != EEPROM_MAGIC || inst.versie != EEPROM_VERSIE) {
    standaardInstellingen();
    bewaar();
    Serial.println(F("Geen geldige instellingen in EEPROM: standaardwaarden geladen."));
  } else {
    Serial.println(F("Instellingen geladen uit EEPROM."));
  }
}

// ===== Motoren =====
void motorLinks(int s) {
  s = constrain(s, -inst.maxSpeed, inst.maxSpeed);
  pwmLinks = s;
  if (LINKS_OMKEREN) s = -s;
  if (s >= 0) { analogWrite(LINKS_1, s);  analogWrite(LINKS_2, 0); }
  else        { analogWrite(LINKS_1, 0);  analogWrite(LINKS_2, -s); }
}

void motorRechts(int s) {
  s = constrain(s, -inst.maxSpeed, inst.maxSpeed);
  pwmRechts = s;
  if (RECHTS_OMKEREN) s = -s;
  if (s >= 0) { analogWrite(RECHTS_1, s); analogWrite(RECHTS_2, 0); }
  else        { analogWrite(RECHTS_1, 0); analogWrite(RECHTS_2, -s); }
}

void stopMotoren() {
  motorLinks(0);
  motorRechts(0);
}

// Correctie verdelen over beide wielen: het buitenste wiel versnelt met diff * |output|,
// het binnenste wiel vertraagt met (1 - diff) * |output|.
void stuur(float output) {
  float links, rechts;
  if (output >= 0) {
    links  = inst.power + inst.diff * output;
    rechts = inst.power - (1 - inst.diff) * output;
  } else {
    links  = inst.power + (1 - inst.diff) * output;
    rechts = inst.power - inst.diff * output;
  }
  motorLinks((int)links);
  motorRechts((int)rechts);
}

// ===== Sensoren =====
void leesRuw() {
  for (int i = 0; i < 8; i++) {
    int pin = SENSOR_PIN[SENSOR_OMGEKEERD ? 7 - i : i];
    analogRead(pin);                 // eerste meting na kanaalwissel weggooien
    ruw[i] = analogRead(pin);
  }
}

// Vult donker[] met 0..1000 (hoog = lijn)
void leesSensoren() {
  leesRuw();
  for (int i = 0; i < 8; i++) {
    int bereik = inst.zwart[i] - inst.wit[i];
    if (bereik == 0) bereik = 1;
    long d = (long)(ruw[i] - inst.wit[i]) * 1000 / bereik;
    d = constrain(d, 0, 1000);
    donker[i] = LIJN_IS_ZWART ? d : 1000 - d;
  }
}

// Positie 0..7000 (3500 = midden). lijn = false als geen enkele sensor de lijn ziet.
// kruispunt = true als (bijna) alle sensoren zwart zien.
int leesPositie(bool &lijn, bool &kruispunt) {
  leesSensoren();
  long som = 0, gewogen = 0;
  int hoogste = 0, aantalZwart = 0;
  for (int i = 0; i < 8; i++) {
    if (donker[i] > hoogste) hoogste = donker[i];
    if (donker[i] > 500) aantalZwart++;
    gewogen += (long)donker[i] * i * 1000;
    som += donker[i];
  }
  lijn = (hoogste > 200 && som > 0);
  kruispunt = (aantalZwart >= KRUISPUNT_SENSOREN);
  if (!lijn || kruispunt) return laatstePositie;
  laatstePositie = gewogen / som;
  return laatstePositie;
}

// ===== Kalibratie =====
// Gemiddelde van 20 metingen per sensor (robot stil op een effen ondergrond)
void meetGemiddelde(int doel[8]) {
  long som[8] = {0};
  for (int n = 0; n < 20; n++) {
    leesRuw();
    for (int i = 0; i < 8; i++) som[i] += ruw[i];
    delay(5);
  }
  for (int i = 0; i < 8; i++) doel[i] = som[i] / 20;
}

void kalibreerWit() {
  meetGemiddelde(inst.wit);
  controleerKalibratie();
  Serial.println(F("Wit gekalibreerd."));
}

void kalibreerZwart() {
  meetGemiddelde(inst.zwart);
  controleerKalibratie();
  Serial.println(F("Zwart gekalibreerd."));
}

// 5 s heen en weer bewegen over de lijn: per sensor laagste (wit) en hoogste (zwart) waarde
void kalibreerZwaai() {
  Serial.println(F("Kalibreren (5 s): beweeg de robot heen en weer over de lijn..."));
  int minW[8], maxW[8];
  for (int i = 0; i < 8; i++) { minW[i] = 1023; maxW[i] = 0; }
  unsigned long start = millis();
  while (millis() - start < 5000) {
    leesRuw();
    for (int i = 0; i < 8; i++) {
      if (ruw[i] < minW[i]) minW[i] = ruw[i];
      if (ruw[i] > maxW[i]) maxW[i] = ruw[i];
    }
    digitalWrite(STATUS_LED, (millis() / 100) % 2);
  }
  digitalWrite(STATUS_LED, LOW);
  for (int i = 0; i < 8; i++) {
    inst.wit[i]   = minW[i];   // wit reflecteert veel IR -> lage spanning
    inst.zwart[i] = maxW[i];
  }
  knopGedrukt = false;         // knopdruk tijdens het kalibreren negeren
  controleerKalibratie();
  Serial.println(F("Kalibratie klaar."));
}

void controleerKalibratie() {
  bool ok = true;
  for (int i = 0; i < 8; i++) {
    if (abs(inst.zwart[i] - inst.wit[i]) < 100) {
      ok = false;
      Serial.print(F("Let op: sensor ")); Serial.print(i + 1);
      Serial.println(F(" ziet weinig verschil tussen wit en zwart."));
    }
  }
  inst.gekalibreerd = ok;
  sessieGekalibreerd = ok;
  bewaar();
}

// ===== Start / stop =====
void start() {
  if (!inst.gekalibreerd) {
    Serial.println(F("Eerst kalibreren: 'calibrate' of 'calibrate white' + 'calibrate black'."));
    return;
  }
  integraal = 0;
  laatsteFout = 0;
  laatsteOutput = 0;
  laatstGezien = millis();
  vorigeCyclus = micros();
  rijden = true;
  digitalWrite(STATUS_LED, HIGH);
  Serial.println(F("Start"));
}

void stop() {
  rijden = false;
  stopMotoren();
  digitalWrite(STATUS_LED, LOW);
  Serial.println(F("Stop"));
}

void knopISR() {
  unsigned long nu = millis();
  if (nu - laatsteKnop > DEBOUNCE_MS) {
    laatsteKnop = nu;
    knopGedrukt = true;
  }
}

// ===== Commando's =====
void toonHelp() {
  Serial.println(F("Commando's:"));
  Serial.println(F("  start | stop"));
  Serial.println(F("  calibrate          (5 s heen en weer over de lijn)"));
  Serial.println(F("  calibrate white    (alle sensoren op wit)"));
  Serial.println(F("  calibrate black    (alle sensoren op zwart)"));
  Serial.println(F("  set power [0..255] | set max [0..255] | set diff [0..1]"));
  Serial.println(F("  set kp [..] | set ki [..] | set kd [..]"));
  Serial.println(F("  set cycle [us]"));
  Serial.println(F("  debug on | debug off"));
  Serial.println(F("  info | sensors | reset | help"));
}

void toonInfo() {
  Serial.println(F("--- instellingen ---"));
  Serial.print(F("power: "));  Serial.println(inst.power);
  Serial.print(F("max: "));    Serial.println(inst.maxSpeed);
  Serial.print(F("diff: "));   Serial.println(inst.diff, 2);
  Serial.print(F("kp: "));     Serial.println(inst.kp, 4);
  Serial.print(F("ki: "));     Serial.println(inst.ki, 4);
  Serial.print(F("kd: "));     Serial.println(inst.kd, 4);
  Serial.print(F("cycle: "));  Serial.print(inst.cycle);
  Serial.print(F(" us (gemeten: ")); Serial.print(gemetenCyclus); Serial.println(F(" us)"));
  Serial.print(F("debug: "));  Serial.println(inst.debug ? F("on") : F("off"));
  Serial.print(F("gekalibreerd: ")); Serial.println(inst.gekalibreerd ? F("ja") : F("nee"));
  Serial.print(F("wit:   "));
  for (int i = 0; i < 8; i++) { Serial.print(inst.wit[i]); Serial.print(' '); }
  Serial.println();
  Serial.print(F("zwart: "));
  for (int i = 0; i < 8; i++) { Serial.print(inst.zwart[i]); Serial.print(' '); }
  Serial.println();
  Serial.print(F("status: ")); Serial.println(rijden ? F("rijden") : F("gestopt"));
}

void toonSensoren() {
  bool lijn, kruispunt;
  int positie = leesPositie(lijn, kruispunt);
  Serial.print(F("ruw: "));
  for (int i = 0; i < 8; i++) { Serial.print(ruw[i]); Serial.print(' '); }
  Serial.print(F("| 0-1000: "));
  for (int i = 0; i < 8; i++) { Serial.print(donker[i]); Serial.print(' '); }
  Serial.print(F("| positie: "));
  if (lijn) Serial.println(positie); else Serial.println(F("geen lijn"));
}

void fout(const __FlashStringHelper *tekst) {
  Serial.print(F("Fout: ")); Serial.println(tekst);
}

void verwerkCommando(char *regel) {
  // naar kleine letters
  for (char *p = regel; *p; p++) if (*p >= 'A' && *p <= 'Z') *p += 'a' - 'A';

  char *cmd  = strtok(regel, " ");
  char *arg1 = strtok(NULL, " ");
  char *arg2 = strtok(NULL, " ");
  if (cmd == NULL) return;

  if (strcmp(cmd, "start") == 0) { start(); return; }
  if (strcmp(cmd, "stop") == 0)  { stop();  return; }
  if (strcmp(cmd, "help") == 0)  { toonHelp(); return; }
  if (strcmp(cmd, "info") == 0)  { toonInfo(); return; }
  if (strcmp(cmd, "sensors") == 0) { toonSensoren(); return; }

  if (strcmp(cmd, "reset") == 0) {
    stop();
    standaardInstellingen();
    bewaar();
    Serial.println(F("Standaardinstellingen hersteld (opnieuw kalibreren)."));
    return;
  }

  if (strcmp(cmd, "debug") == 0) {
    if (arg1 && strcmp(arg1, "on") == 0)       inst.debug = true;
    else if (arg1 && strcmp(arg1, "off") == 0) inst.debug = false;
    else { fout(F("gebruik: debug on | debug off")); return; }
    bewaar();
    Serial.print(F("debug: ")); Serial.println(inst.debug ? F("on") : F("off"));
    return;
  }

  if (strcmp(cmd, "calibrate") == 0) {
    if (rijden) stop();
    if (arg1 == NULL)                      kalibreerZwaai();
    else if (strcmp(arg1, "white") == 0)   kalibreerWit();
    else if (strcmp(arg1, "black") == 0)   kalibreerZwart();
    else fout(F("gebruik: calibrate | calibrate white | calibrate black"));
    return;
  }

  if (strcmp(cmd, "set") == 0) {
    if (arg1 == NULL || arg2 == NULL) { fout(F("gebruik: set [param] [waarde]")); return; }
    float w = atof(arg2);
    if (strcmp(arg1, "power") == 0) {
      if (w < 0 || w > 255) { fout(F("power 0..255")); return; }
      inst.power = (int)w;
    } else if (strcmp(arg1, "max") == 0) {
      if (w < 0 || w > 255) { fout(F("max 0..255")); return; }
      inst.maxSpeed = (int)w;
    } else if (strcmp(arg1, "diff") == 0) {
      if (w < 0 || w > 1) { fout(F("diff 0..1")); return; }
      inst.diff = w;
    } else if (strcmp(arg1, "kp") == 0) {
      if (w < 0) { fout(F("kp >= 0")); return; }
      inst.kp = w;
    } else if (strcmp(arg1, "ki") == 0) {
      if (w < 0) { fout(F("ki >= 0")); return; }
      inst.ki = w;
      integraal = 0;
    } else if (strcmp(arg1, "kd") == 0) {
      if (w < 0) { fout(F("kd >= 0")); return; }
      inst.kd = w;
    } else if (strcmp(arg1, "cycle") == 0) {
      if (w < 1000 || w > 100000) { fout(F("cycle 1000..100000 us")); return; }
      inst.cycle = (unsigned long)w;
    } else {
      fout(F("onbekende parameter"));
      return;
    }
    bewaar();
    Serial.print(arg1); Serial.print(F(" = ")); Serial.println(arg2);
    return;
  }

  fout(F("onbekend commando, typ 'help'"));
}

void leesSerieel() {
  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (cmdLengte > 0) {
        cmdBuffer[cmdLengte] = '\0';
        verwerkCommando(cmdBuffer);
        cmdLengte = 0;
      }
    } else if (cmdLengte < sizeof(cmdBuffer) - 1) {
      cmdBuffer[cmdLengte++] = c;
    }
  }
}

// ===== Regeling =====
void regel() {
  bool lijn, kruispunt;
  int positie = leesPositie(lijn, kruispunt);
  unsigned long nu = millis();

  if (kruispunt) {
    // Kruispunt: rechtdoor rijden
    laatstGezien = nu;
    stuur(0);
    return;
  }

  if (!lijn) {
    unsigned long weg = nu - laatstGezien;
    if (weg < ONDERBREKING_MS) {
      stuur(0);                                   // korte onderbreking: rechtdoor
    } else if (weg < LIJN_KWIJT_STOP) {
      // lijn kwijt: draai naar de kant waar de lijn het laatst was
      int zoek = inst.power / 2;
      if (laatstePositie < 3500) { motorLinks(-zoek); motorRechts(zoek); }
      else                       { motorLinks(zoek);  motorRechts(-zoek); }
    } else {
      Serial.println(F("Lijn kwijt"));
      stop();
    }
    return;
  }

  laatstGezien = nu;

  float foutWaarde = positie - 3500;              // -3500 .. 3500
  integraal += foutWaarde;
  integraal = constrain(integraal, -50000, 50000);
  float afgeleide = foutWaarde - laatsteFout;
  laatsteFout = foutWaarde;

  laatsteOutput = inst.kp * foutWaarde + inst.ki * integraal + inst.kd * afgeleide;
  stuur(laatsteOutput);
}

void debugUitvoer() {
  if (!inst.debug || millis() - vorigeDebug < DEBUG_INTERVAL_MS) return;
  vorigeDebug = millis();
  if (rijden) {
    Serial.print(F("pos ")); Serial.print(laatstePositie);
    Serial.print(F(" out ")); Serial.print(laatsteOutput, 1);
    Serial.print(F(" L ")); Serial.print(pwmLinks);
    Serial.print(F(" R ")); Serial.print(pwmRechts);
    Serial.print(F(" cyc ")); Serial.println(gemetenCyclus);
  } else {
    toonSensoren();
  }
}

// ===== Setup en loop =====
void setup() {
  pinMode(LINKS_1, OUTPUT);
  pinMode(LINKS_2, OUTPUT);
  pinMode(RECHTS_1, OUTPUT);
  pinMode(RECHTS_2, OUTPUT);
  pinMode(STATUS_LED, OUTPUT);
  pinMode(KNOP, INPUT_PULLUP);
  stopMotoren();

  Serial.begin(BAUD);
  Serial.println(F("Line follower gestart. Typ 'help' voor de commando's."));
  laadInstellingen();
  Serial.println(F("Knop: 1e klik = kalibreren (5 s over de lijn zwaaien), 2e klik = rijden."));

  attachInterrupt(digitalPinToInterrupt(KNOP), knopISR, FALLING);
}

void loop() {
  leesSerieel();

  // Start/stop-knop: 1e klik na opstarten = kalibreren, daarna start/stop wisselen
  if (knopGedrukt) {
    knopGedrukt = false;
    if (rijden)                     stop();
    else if (!sessieGekalibreerd)   kalibreerZwaai();
    else                            start();
  }

  if (rijden) {
    unsigned long nu = micros();
    if (nu - vorigeCyclus >= inst.cycle) {
      gemetenCyclus = nu - vorigeCyclus;
      vorigeCyclus = nu;
      regel();
    }
  }

  debugUitvoer();
}
