
const int MOTOR_A_IN1 = 5;   // AIN1 - Motor A
const int MOTOR_A_IN2 = 6;   // AIN2 - Motor A

const int MOTOR_B_IN1 = 9;   // BIN1 - Motor B
const int MOTOR_B_IN2 = 10;  // BIN2 - Motor B

const int MAX_SPEED = 255;
const int STAP = 5;
const int VERTRAGING = 40;

// Motor A aansturen
void motorA(int snelheid) {
  if (snelheid >= 0) {
    analogWrite(MOTOR_A_IN1, snelheid);
    analogWrite(MOTOR_A_IN2, 0);
  } else {
    analogWrite(MOTOR_A_IN1, 0);
    analogWrite(MOTOR_A_IN2, -snelheid);
  }
}

// Motor B aansturen
void motorB(int snelheid) {
  if (snelheid >= 0) {
    analogWrite(MOTOR_B_IN1, snelheid);
    analogWrite(MOTOR_B_IN2, 0);
  } else {
    analogWrite(MOTOR_B_IN1, 0);
    analogWrite(MOTOR_B_IN2, -snelheid);
  }
}

// Beide motoren stoppen
void stopMotoren() {
  motorA(0);
  motorB(0);
}

// Motoren geleidelijk versnellen en vertragen
void ramp(int richtingA, int richtingB) {

  Serial.println();
  Serial.println("================================");
  Serial.println("Nieuwe motortest gestart");

  // Versnellen
  Serial.println("Motoren versnellen...");

  for (int snelheid = 0; snelheid <= MAX_SPEED; snelheid += STAP) {

    int pwmA = snelheid * richtingA;
    int pwmB = snelheid * richtingB;

    motorA(pwmA);
    motorB(pwmB);

    Serial.print("Set motor A PWM: ");
    Serial.print(abs(pwmA));
    Serial.print(" | Richting: ");
    Serial.print(pwmA >= 0 ? "Vooruit" : "Achteruit");

    Serial.print(" || Set motor B PWM: ");
    Serial.print(abs(pwmB));
    Serial.print(" | Richting: ");
    Serial.println(pwmB >= 0 ? "Vooruit" : "Achteruit");

    delay(VERTRAGING);
  }

  // Vertragen
  Serial.println("Motoren vertragen...");

  for (int snelheid = MAX_SPEED; snelheid >= 0; snelheid -= STAP) {

    int pwmA = snelheid * richtingA;
    int pwmB = snelheid * richtingB;

    motorA(pwmA);
    motorB(pwmB);

    Serial.print("Set motor A PWM: ");
    Serial.print(abs(pwmA));

    Serial.print(" || Set motor B PWM: ");
    Serial.println(abs(pwmB));

    delay(VERTRAGING);
  }

  stopMotoren();

  Serial.println("Motor A PWM: 0");
  Serial.println("Motor B PWM: 0");
  Serial.println("Beide motoren gestopt.");
  Serial.println("================================");

  delay(1000);
}

void setup() {
  pinMode(MOTOR_A_IN1, OUTPUT);
  pinMode(MOTOR_A_IN2, OUTPUT);

  pinMode(MOTOR_B_IN1, OUTPUT);
  pinMode(MOTOR_B_IN2, OUTPUT);

  Serial.begin(9600);

  stopMotoren();

  Serial.println("DRV8833 - Twee motoren test");
  Serial.println("Serial Monitor gestart!");
}

void loop() {

  // Test 1: Beide motoren vooruit
  Serial.println("\nTEST 1: Beide motoren vooruit");
  ramp(1, 1);

  // Test 2: Beide motoren achteruit
  Serial.println("\nTEST 2: Beide motoren achteruit");
  ramp(-1, -1);

  // Test 3: Motor A vooruit, motor B achteruit
  Serial.println("\nTEST 3: Motor A vooruit - Motor B achteruit");
  ramp(1, -1);

  // Test 4: Motor A achteruit, motor B vooruit
  Serial.println("\nTEST 4: Motor A achteruit - Motor B vooruit");
  ramp(-1, 1);
}
