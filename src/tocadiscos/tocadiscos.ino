#include <DFRobotDFPlayerMini.h>

#define ALTURA_MAXIMA 15
// Rango de lectura de la ficha baja
#define BAJA_MIN 3
#define BAJA_MAX 4.5
// Rango de lectura de la ficha alta
#define ALTA_MIN 6
#define ALTA_MAX 7.5

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

int medirDistancia(int i){
  digitalWrite(Trig[i], LOW);
  delayMicroseconds(2);
  digitalWrite(Trig[i], HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig[i], LOW);

  long tiempo = pulseIn(Echo[i], HIGH, 20000);
  if (tiempo == 0) return 999;
  return tiempo * 0.034 / 2;
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
