#include <DFRobotDFPlayerMini.h>

#define ALTURA_MAXIMA 15
// Rango de lectura de la ficha baja
#define BAJA_MIN 11.5
#define BAJA_MAX 13
// Rango de lectura de la ficha alta
#define ALTA_MIN 5.5
#define ALTA_MAX 10

const int Trig[] = {2, 4, 6, A0}; 
const int Echo[] = {3, 5, 7, A1}; 

//0 para vacío, 1 para ficha baja, 2 para ficha alta
int estadoSensor[] = {0, 0, 0, 0};


void setup() {
  Serial.begin(115200);
  for (int i = 0; i < 4; i++) {
    pinMode(Trig[i], OUTPUT);
    pinMode(Echo[i], INPUT);
  }
  Serial.println("--- ESPERANDO FICHAS ---");
}

int medirDistancia(int i) {
  long sumaDistancias = 0;
  int muestras = 3; // Vamos a tomar 3 muestras

  for (int m = 0; m < muestras; m++) {
    digitalWrite(Trig[i], LOW);
    delayMicroseconds(2);
    digitalWrite(Trig[i], HIGH);
    delayMicroseconds(10);
    digitalWrite(Trig[i], LOW);

    // El timeout de 20000ms es suficiente para unos 3 metros
    long tiempo = pulseIn(Echo[i], HIGH, 20000); 
    
    int d;
    if (tiempo == 0) {
      d = 999; // Si falla, le asignamos un valor de "vacío"
    } else {
      d = tiempo * 0.034 / 2;
    }
    
    sumaDistancias += d;
    delay(5); // Pequeña pausa entre disparos del mismo sensor
  }

  return sumaDistancias / muestras; // Devolvemos la media
}

void loop() {
  for(int i = 0; i < 4; i++){
    int distancia = medirDistancia(i);
    
    if (distancia >= BAJA_MIN && distancia <= BAJA_MAX){
      if (estadoSensor[i] != 1){
        int notaATocar = (i * 2) + 1;
        Serial.print("Sensor "); Serial.print(i+1);
        Serial.print(" - Nota detectada: "); Serial.println(notaATocar);
        estadoSensor[i] = 1;
      }
    }else if (distancia >= ALTA_MIN && distancia <= ALTA_MAX){
      if (estadoSensor[i] != 2){
        int notaATocar = (i * 2) + 2;
        Serial.print("Sensor "); Serial.print(i+1);
        Serial.print(" - Nota detectada: "); Serial.println(notaATocar);
        estadoSensor[i] = 2;
      }
    }else if (distancia > ALTURA_MAXIMA) {
      if (estadoSensor[i] != 0) {
        Serial.println("Estado: Vacío");
        estadoSensor[i] = 0;
      }
    }
  }
  delay(30);
}
