// Basisprogramma line follower: communicatie + EEPROM + interrupt
//
// - Commando's via bluetooth (HC-06 op D0/D1, 9600 baud) of de seriele monitor
//   (zonder HC-06). Elk commando afsluiten met Enter. Typ "help".
// - Instellingen (cyclustijd, debug) worden in het EEPROM bewaard en bij het
//   opstarten opnieuw geladen.
// - Start/stop met de drukknop op D2 (naar GND, INPUT_PULLUP) via een
//   hardware-interrupt, of met de commando's start/stop.
// - Demo: zolang het programma "loopt" knippert de led op D13 met de ingestelde
//   cyclustijd (aan/uit wisselen bij elke cyclus).
// - Koppel de HC-06 los tijdens het uploaden (D0/D1 delen de USB-verbinding).

#include <EEPROM.h>

const int KNOP = 2;                    // drukknop naar GND (interrupt INT0)
const int LED = LED_BUILTIN;           // D13
const long BAUD = 9600;                // standaardbaudrate HC-06
const unsigned long DEBOUNCE_MS = 250;

const uint16_t EEPROM_MAGIC = 0x4C46;  // controlegetal: geldige instellingen aanwezig
const uint8_t  EEPROM_VERSIE = 1;

struct Instellingen {
  uint16_t magic;
  uint8_t  versie;
  unsigned long cycle;     // cyclustijd in microseconden
  bool debug;
};

Instellingen inst;

bool actief = false;
unsigned long vorigeCyclus = 0;
unsigned long aantalCycli = 0;

volatile bool knopGedrukt = false;
volatile unsigned long laatsteKnop = 0;

char cmdBuffer[40];
byte cmdLengte = 0;

// ===== EEPROM =====
void standaardInstellingen() {
  inst.magic = EEPROM_MAGIC;
  inst.versie = EEPROM_VERSIE;
  inst.cycle = 500000;     // 0,5 s
  inst.debug = false;
}

void bewaar() {
  EEPROM.put(0, inst);
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

// ===== Interrupt =====
void knopISR() {
  unsigned long nu = millis();
  if (nu - laatsteKnop > DEBOUNCE_MS) {
    laatsteKnop = nu;
    knopGedrukt = true;
  }
}

// ===== Start / stop =====
void start() {
  actief = true;
  aantalCycli = 0;
  vorigeCyclus = micros();
  Serial.println(F("Start"));
}

void stop() {
  actief = false;
  digitalWrite(LED, LOW);
  Serial.println(F("Stop"));
}

// ===== Commando's =====
void toonHelp() {
  Serial.println(F("Commando's:"));
  Serial.println(F("  start | stop"));
  Serial.println(F("  set cycle [us]"));
  Serial.println(F("  debug on | debug off"));
  Serial.println(F("  info | help"));
}

void toonInfo() {
  Serial.print(F("cycle: ")); Serial.print(inst.cycle); Serial.println(F(" us"));
  Serial.print(F("debug: ")); Serial.println(inst.debug ? F("on") : F("off"));
  Serial.print(F("status: ")); Serial.println(actief ? F("actief") : F("gestopt"));
}

void verwerkCommando(char *regel) {
  for (char *p = regel; *p; p++) if (*p >= 'A' && *p <= 'Z') *p += 'a' - 'A';

  char *cmd  = strtok(regel, " ");
  char *arg1 = strtok(NULL, " ");
  char *arg2 = strtok(NULL, " ");
  if (cmd == NULL) return;

  if (strcmp(cmd, "start") == 0) { start(); return; }
  if (strcmp(cmd, "stop") == 0)  { stop();  return; }
  if (strcmp(cmd, "help") == 0)  { toonHelp(); return; }
  if (strcmp(cmd, "info") == 0)  { toonInfo(); return; }

  if (strcmp(cmd, "debug") == 0) {
    if (arg1 && strcmp(arg1, "on") == 0)       inst.debug = true;
    else if (arg1 && strcmp(arg1, "off") == 0) inst.debug = false;
    else { Serial.println(F("Fout: gebruik debug on | debug off")); return; }
    bewaar();
    Serial.print(F("debug: ")); Serial.println(inst.debug ? F("on") : F("off"));
    return;
  }

  if (strcmp(cmd, "set") == 0 && arg1 && arg2 && strcmp(arg1, "cycle") == 0) {
    long w = atol(arg2);
    if (w < 1000 || w > 10000000L) { Serial.println(F("Fout: cycle 1000..10000000 us")); return; }
    inst.cycle = (unsigned long)w;
    bewaar();
    Serial.print(F("cycle = ")); Serial.print(inst.cycle); Serial.println(F(" us"));
    return;
  }

  Serial.println(F("Fout: onbekend commando, typ 'help'"));
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

// ===== Setup en loop =====
void setup() {
  pinMode(LED, OUTPUT);
  pinMode(KNOP, INPUT_PULLUP);
  Serial.begin(BAUD);
  Serial.println(F("Basisprogramma gestart. Typ 'help'."));
  laadInstellingen();
  toonInfo();
  attachInterrupt(digitalPinToInterrupt(KNOP), knopISR, FALLING);
}

void loop() {
  leesSerieel();

  if (knopGedrukt) {
    knopGedrukt = false;
    if (actief) stop(); else start();
  }

  if (actief) {
    unsigned long nu = micros();
    if (nu - vorigeCyclus >= inst.cycle) {
      vorigeCyclus = nu;
      aantalCycli++;
      digitalWrite(LED, !digitalRead(LED));   // led wisselt bij elke cyclus
      if (inst.debug) {
        Serial.print(F("cyclus ")); Serial.println(aantalCycli);
      }
    }
  }
}
