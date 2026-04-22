#include <Stepper.h>

const int pinBoton = 2;
const int stepsPerRev = 2048;
Stepper myStepper(stepsPerRev, 8, 10, 9, 11);

bool motorEncendido = false;
bool botonPresionado = false;

void setup() {
  pinMode(pinBoton, INPUT_PULLUP); // Resistencia interna
  myStepper.setSpeed(10);          // Velocidad de giro del motor
}

void loop() {
  // 1. LEER EL BOTÓN
  if (digitalRead(pinBoton) == LOW && !botonPresionado) {
    motorEncendido = !motorEncendido; 
    botonPresionado = true;
    delay(200);
  }

  if (digitalRead(pinBoton) == HIGH) {
    botonPresionado = false;
  }

  //2. MOVER EL MOTOR
  if (motorEncendido) {
    myStepper.step(10); 
  } else {
    // Si está apagado, nos aseguramos de apagar las luces (bobinas)
    digitalWrite(8, LOW);
    digitalWrite(9, LOW);
    digitalWrite(10, LOW);
    digitalWrite(11, LOW);
  }
}
