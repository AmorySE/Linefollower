// Test HC-06 Bluetooth: duplex communicatie tussen Arduino Nano en smartphone
// HC-06 hangt aan de hardware-seriele poort: TX HC-06 -> D0/RX, D1/TX -> spanningsdeler -> RX HC-06
// LET OP: koppel de HC-06 (of minstens de draad naar D0) los tijdens het uploaden,
//         anders botst hij met de USB-chip en mislukt de upload.
// Baudrate 9600 = standaardinstelling van de HC-06.
//
// Commando's vanaf de smartphone (Serial Bluetooth Terminal), afsluiten met Enter:
//   led on   -> ingebouwde led (D13) aan
//   led off  -> ingebouwde led uit
//   ping     -> Arduino antwoordt "pong"
//   andere tekst wordt teruggestuurd (echo)
// Elke 5 s stuurt de Arduino zelf een bericht -> communicatie in beide richtingen.

const long BAUD = 9600;
const unsigned long INTERVAL_MS = 5000;

String bericht = "";
unsigned long vorigeMelding = 0;

void verwerk(String cmd) {
  cmd.trim();
  cmd.toLowerCase();
  if (cmd.length() == 0) return;

  if (cmd == "led on") {
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("OK: led aan");
  } else if (cmd == "led off") {
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("OK: led uit");
  } else if (cmd == "ping") {
    Serial.println("pong");
  } else {
    Serial.print("Ontvangen: ");
    Serial.println(cmd);
  }
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(BAUD);
  Serial.println("HC-06 Bluetooth test gestart!");
  Serial.println("Commando's: led on | led off | ping");
}

void loop() {
  // Smartphone -> Arduino: tekens verzamelen tot Enter
  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      verwerk(bericht);
      bericht = "";
    } else if (bericht.length() < 32) {
      bericht += c;
    }
  }

  // Arduino -> smartphone: periodiek bericht
  if (millis() - vorigeMelding >= INTERVAL_MS) {
    vorigeMelding = millis();
    Serial.print("Arduino actief, uptime: ");
    Serial.print(millis() / 1000);
    Serial.println(" s");
  }
}
