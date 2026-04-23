#include <DFRobotDFPlayerMini.h>

#define ALTURA_MAXIMA 10
// Rango de lectura de la ficha baja
#define BAJA_MIN 3
#define BAJA_MAX 4.5
// Rango de lectura de la ficha alta
#define ALTA_MIN 6
#define ALTA_MAX 7.5

//0 para vacío, 1 para ficha baja, 2 para ficha alta
int estadoSensor[4] = {0, 0, 0, 0};

const int Trig = 9; 
const int Echo = 8; 

void setup() {
  Serial.begin(115200);
  pinMode(Trig, OUTPUT);
  pinMode(Echo, INPUT);
  Serial.println("--- ESPERANDO FICHAS ---");
}

int medir(int i){
  digitalWrite(Trig, LOW);
  delayMicroseconds(5);
  digitalWrite(Trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig, LOW);

  long tiempo = pulseIn(Echo, HIGH, 20000);
  if (tiempo == 0) return 999;
  return tiempo * 0.034 / 2;
}

void loop() {
  int distancia = medir(0);
  if (distancia >= BAJA_MIN && distancia <= BAJA_MAX){
    if (estadoSensor[0] != 1){
      Serial.println("Nota 1: DO");
      estadoSensor[0] = 1;
    }
  }else if (distancia >= ALTA_MIN && distancia <= ALTA_MAX){
      if (estadoSensor[0] != 2){
        Serial.println("Nota 2: RE");
        estadoSensor[0] = 2;
      }
  }else if (distancia > ALTURA_MAXIMA) {
    if (estadoSensor[0] != 0) {
      Serial.println("Estado: Vacío");
      estadoSensor[0] = 0;
    }
  }
  delay(100);
}
