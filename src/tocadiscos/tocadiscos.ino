#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>

#define ALTURA_MAXIMA 13
// Rango de lectura de la ficha baja
#define BAJA_MIN 9.5
#define BAJA_MAX 11
// Rango de lectura de la ficha alta
#define ALTA_MIN 3.5
#define ALTA_MAX 8

//Control de lecturas erróneas
#define UMBRAL_CONFIRMACION 4
int lecturas_vacias[] = {0, 0, 0, 0};

const int Trig[] = {2, 4, 6, A0}; 
const int Echo[] = {3, 5, 7, A1}; 

//0 para vacío, 1 para ficha baja, 2 para ficha alta
int estadoSensor[] = {0, 0, 0, 0};

DFRobotDFPlayerMini reproductor;
SoftwareSerial puertoMP3(12, 13);

void setup() {
  Serial.begin(115200);
  puertoMP3.begin(9600);

  delay(1000); 
  if (reproductor.begin(puertoMP3)) {
    Serial.println("DFPlayer OK");
    reproductor.volume(20);
  } else {
    Serial.println("DFPlayer no encontrado - Comprobando sensores...");
  }

  for (int i = 0; i < 4; i++) {
    pinMode(Trig[i], OUTPUT);
    pinMode(Echo[i], INPUT);
  }
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

    long tiempo = pulseIn(Echo[i], HIGH, 20000); 
    
    int d;
    if (tiempo == 0) {
      d = 999;
    } else {
      d = tiempo * 0.034 / 2;
    }
    sumaDistancias += d;
    delay(5);
  }
  return sumaDistancias / muestras;
}

void loop() {
  for(int i = 0; i < 4; i++){
    int distancia = medirDistancia(i);
    
    if (distancia >= BAJA_MIN && distancia <= BAJA_MAX){
      lecturas_vacias[i] = 0;
      if (estadoSensor[i] != 1){
        int notaATocar = (i * 2) + 1;
        Serial.print("Sensor "); Serial.print(i+1);
        Serial.print(" - Nota detectada: "); Serial.println(notaATocar);
        estadoSensor[i] = 1;
        reproductor.play(notaATocar);
        delay(10);
      }
    }else if (distancia >= ALTA_MIN && distancia <= ALTA_MAX){
      lecturas_vacias[i] = 0;
      if (estadoSensor[i] != 2){
        int notaATocar = (i * 2) + 2;
        Serial.print("Sensor "); Serial.print(i+1);
        Serial.print(" - Nota detectada: "); Serial.println(notaATocar);
        estadoSensor[i] = 2;
        reproductor.play(notaATocar);
        delay(10);
      }
    }else if (distancia > ALTURA_MAXIMA) {
      if (estadoSensor[i] != 0) {
        lecturas_vacias[i]++;
        if (lecturas_vacias[i] >= UMBRAL_CONFIRMACION){
          Serial.println("Estado: Vacío");
          estadoSensor[i] = 0;
        }
      }
    }
  }
  delay(30);
}
