#define directionPin1 2
#define stepPin1 3
#define directionPin2 4
#define stepPin2 5

#define stepsPerRevolution 6400
#define stepsPerDegree (stepsPerRevolution / 360.0)  // ~17.78 passos por grau

#define delayMicrosecondsDesired 300

void setup() {
  pinMode(directionPin1, OUTPUT);
  pinMode(stepPin1, OUTPUT);
  pinMode(directionPin2, OUTPUT);
  pinMode(stepPin2, OUTPUT);
}

// Função para girar um único motor
void rotateMotor(int motor, int degrees, bool direction) {
  if (degrees % 15 != 0) return; // Garante múltiplos de 15°

  int steps = round(degrees * stepsPerDegree); // Converte graus em passos

  if (motor == 1) {
    digitalWrite(directionPin1, direction ? HIGH : LOW);
    for (int i = 0; i < steps; i++) {
      digitalWrite(stepPin1, HIGH);
      delayMicroseconds(delayMicrosecondsDesired);
      digitalWrite(stepPin1, LOW);
      delayMicroseconds(delayMicrosecondsDesired);
    }
  } else if (motor == 2) {
    digitalWrite(directionPin2, direction ? HIGH : LOW);
    for (int i = 0; i < steps; i++) {
      digitalWrite(stepPin2, HIGH);
      delayMicroseconds(delayMicrosecondsDesired);
      digitalWrite(stepPin2, LOW);
      delayMicroseconds(delayMicrosecondsDesired);
    }
  }
}

// Função para girar os motores sequencialmente
void rotateSequentially(int degMotor1, bool dir1, int degMotor2, bool dir2) {
  if (degMotor1 % 15 == 0) {
    rotateMotor(1, degMotor1, dir1);
    delay(500);  // Pequena pausa entre os motores
  }

  if (degMotor2 % 15 == 0) {
    rotateMotor(2, degMotor2, dir2);
    delay(500);
  }
}

void loop() {
  // Exemplo: Motor 1 gira 90° em uma direção, depois Motor 2 gira 180° na outra
  rotateSequentially(90, true, 180, false);
  delay(2000);

  // Exemplo: Motor 1 gira 45° em uma direção, depois Motor 2 gira 15° na outra
  rotateSequentially(45, false, 15, true);
  delay(2000);

  while (1); // Fim do programa
}
