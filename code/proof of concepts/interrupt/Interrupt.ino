// Interrupt-test start/stop-knop
// Drukknop tussen D2 en GND, geen externe weerstand: D2 staat op INPUT_PULLUP (interne pull-up ~20-50 kOhm).
// Niet ingedrukt = HIGH, ingedrukt = LOW -> interrupt op de dalende flank (FALLING).
// Serial Monitor: 115200 baud

const int KNOP = 2;
const unsigned long DEBOUNCE_MS = 200;

volatile unsigned long laatsteDruk = 0;
volatile unsigned int teller = 0;
volatile bool nieuweDruk = false;

void knopISR() {
  unsigned long nu = millis();
  if (nu - laatsteDruk > DEBOUNCE_MS) {
    laatsteDruk = nu;
    teller++;
    nieuweDruk = true;
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(KNOP, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(KNOP), knopISR, FALLING);
  Serial.println("Interrupt-test gestart: druk op de knop");
}

void loop() {
  if (nieuweDruk) {
    noInterrupts();                        // gedeelde variabelen veilig kopieren
    unsigned int t = teller;
    nieuweDruk = false;
    interrupts();
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    Serial.print("Knop gedrukt, totaal: ");
    Serial.println(t);
  }
}
