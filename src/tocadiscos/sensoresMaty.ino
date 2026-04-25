#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>

#define ALTURA_MAXIMA 13.0
#define BAJA_MIN 9.5
#define BAJA_MAX 11.5
#define ALTA_MIN 3.0
#define ALTA_MAX 8.5
#define UMBRAL_CONFIRMACION 2 // lecturas para confirmar la ficha

// pines para sensores ultrasónicos
const int Trig[] = {2, 4, 6, A0};
const int Echo[] = {3, 5, 7, A1};

//variables de estado
int estadoSensor[] = {0, 0, 0, 0};      // 0: vacío, 1: baja, 2: alta (para saber la ultima ficha)
int contadorCandidato[4] = {0, 0, 0, 0}; // contador de lecturas
int estadoCandidato[4]   = {0, 0, 0, 0}; // estado que se está intentando validar

//DFplayer 
DFRobotDFPlayerMini reproductor;
SoftwareSerial puertoMP3(12, 13); 

void setup() {
  Serial.begin(115200);
  puertoMP3.begin(9600);
  
  // Inicialización de sensores
  for (int i = 0; i < 4; i++) {
    pinMode(Trig[i], OUTPUT);
    pinMode(Echo[i], INPUT);
  }

  // Inicialización DFPlayer
  delay(1000); 
  if (reproductor.begin(puertoMP3)) {
    Serial.println("DFPlayer OK");
    reproductor.volume(20); // Volumen de 0 a 30
  } else {
    Serial.println("Error: DFPlayer no encontrado");
  }
}

float medirDistancia(int i) {
  long sumaDistancias = 0;
  int muestras = 2; //Cantidad de mediciones para validad la ficha 
  int validas = 0;

  for (int m = 0; m < muestras; m++) {
    digitalWrite(Trig[i], LOW);
    delayMicroseconds(2);
    digitalWrite(Trig[i], HIGH);
    delayMicroseconds(10);
    digitalWrite(Trig[i], LOW);

    long tiempo = pulseIn(Echo[i], HIGH, 20000); // Timeout de 25ms para evitar bloqueos largos
    if (tiempo > 0) {
      sumaDistancias += tiempo;
      validas++;
    }
    delay(3);
    
  }


int clasificarDistancia(float distancia) {
  if (distancia >= BAJA_MIN && distancia <= BAJA_MAX) return 1; // ficha baja

  if (distancia >= ALTA_MIN && distancia <= ALTA_MAX) return 2; // ficha alta

  if (distancia > ALTURA_MAXIMA) return 0; // vacío

  return -1; // error
}


void loop() {
  for (int i = 0; i < 4; i++) {
    float distancia = medirDistancia(i);
    int lecturaActual = clasificarDistancia(distancia);
    
    // Si la lectura es error, no hace nada
    if (lecturaActual == -1) {
      contadorCandidato[i] = 0;
      continue;
    }

    // Confirma ficha candidata o inicia una nueva
    if (lecturaActual == estadoCandidato[i]) {
      contadorCandidato[i]++;
    } else {
      estadoCandidato[i] = lecturaActual;
      contadorCandidato[i] = 1;
    }
    
    //Si se alcanza  umbral y el estado es distinto al anterior -> reiniciar contador de validacion
    if (contadorCandidato[i] >= UMBRAL_CONFIRMACION && lecturaActual != estadoSensor[i]) {
      estadoSensor[i] = lecturaActual;
      contadorCandidato[i] = 0;

      //Calculo del sensor
      if(lecturaActual != 0){
        int notaATocar = (i * 2) + lecturaActual; 
        Serial.print("Sensor "); 
        Serial.print(i + 1);
        Serial.print(" - Nota detectada: "); 
        Serial.print(notaATocar);
        
        // suena nota en el DFplayer
        reproductor.play(notaATocar);
        delay(10); 
      }
    }
  }
  //delay(20); //SIN PAUSA
}
