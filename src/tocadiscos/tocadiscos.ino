#include <Stepper.h>

// --- CONFIGURACIÓN MOTOR ---
const int stepsPerRev = 2048;
Stepper myStepper(stepsPerRev, 8, 10, 9, 11);
const int pinBoton = 12;
bool motorEncendido = false;
bool botonPresionado = false;

// --- CONFIGURACIÓN MULTIPLEXOR ---
const int S0 = 2;
const int S1 = 3;
const int S2 = 4;
const int S3 = 5;
const int SIG_PIN = A1; // Pin donde leemos el ECHO multiplexado
const int TRIG_GENERAL = 6; // Pin que dispara todos los TRIG

const int umbralDeteccion = 15;

void setup() {
  Serial.begin(115200);
  
  // Pines del motor y botón
  pinMode(pinBoton, INPUT_PULLUP);
  myStepper.setSpeed(10);

  // Pines del multiplexor
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(SIG_PIN, INPUT);
  pinMode(TRIG_GENERAL, OUTPUT);

  Serial.println("SISTEMA LISTO: Motor + Multiplexor");
}

// Función para decirle al multiplexor qué canal leer
void seleccionarCanal(int canal) {
  digitalWrite(S0, bitRead(canal, 0));
  digitalWrite(S1, bitRead(canal, 1));
  digitalWrite(S2, bitRead(canal, 2));
  digitalWrite(S3, bitRead(canal, 3));
}

int medirMultiplexado(int canal) {
  seleccionarCanal(canal);
  delayMicroseconds(2); // Estabilizar el canal

  // Lanzar pulso
  digitalWrite(TRIG_GENERAL, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_GENERAL, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_GENERAL, LOW);

  // Leer el pin SIG del multiplexor
  long tiempo = pulseIn(SIG_PIN, HIGH, 15000); 
  if (tiempo == 0) return 999;
  return tiempo * 0.034 / 2;
}

void loop() {
  // 1. CONTROL DEL BOTÓN
  if (digitalRead(pinBoton) == LOW && !botonPresionado) {
    motorEncendido = !motorEncendido;
    botonPresionado = true;
    delay(50);
  }
  if (digitalRead(pinBoton) == HIGH) botonPresionado = false;

  // 2. MOVIMIENTO DEL MOTOR
  if (motorEncendido) {
    myStepper.step(10); 
  } else {
    digitalWrite(8, LOW); digitalWrite(9, LOW);
    digitalWrite(10, LOW); digitalWrite(11, LOW);
  }

  // 3. LECTURA DE SENSORES (Canales 0 a 3)
  for(int i=0; i<4; i++) {
    int distancia = medirMultiplexado(i);
    if (distancia < umbralDeteccion) {
      Serial.print("S"); Serial.print(i); Serial.print(": FICHA  ");
    } else {
      Serial.print("S"); Serial.print(i); Serial.print(": ---    ");
    }
  }
  Serial.println();
}
