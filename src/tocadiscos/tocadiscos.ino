#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>

#define ALTA_MIN 6.0
#define ALTA_MAX 9.0
#define BAJA_MIN 9.1
#define BAJA_MAX 11.5
#define ALTURA_MAXIMA 12.3
#define UMBRAL_CONFIRMACION 2
#define TIEMPO_ESPERA 150

const int Trig[] = {2, 4, 6, A0};
const int Echo[] = {3, 5, 7, A1};

int estadoSensor[]      = {0, 0, 0, 0};
int contadorCandidato[] = {0, 0, 0, 0};
int estadoCandidato[]   = {0, 0, 0, 0};
int notaPendiente[]     = {0, 0, 0, 0};
unsigned long tiempoUltimaDeteccion[] = {0, 0, 0, 0};

DFRobotDFPlayerMini reproductor;
SoftwareSerial puertoMP3(12, 13);

void setup() {
  Serial.begin(115200);
  puertoMP3.begin(9600);

  for (int i = 0; i < 4; i++) {
    pinMode(Trig[i], OUTPUT);
    pinMode(Echo[i], INPUT);
  }

  delay(1000);
  if (reproductor.begin(puertoMP3)) {
    Serial.println("DFPlayer OK");
    reproductor.volume(15);
  } else {
    Serial.println("Error: DFPlayer no encontrado");
  }
}

float medirDistancia(int i) {
  digitalWrite(Trig[i], LOW);
  delayMicroseconds(2);
  digitalWrite(Trig[i], HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig[i], LOW);

  long tiempo = pulseIn(Echo[i], HIGH, 15000);
  if (tiempo == 0) return 999.0;
  return tiempo * 0.034 / 2.0;
}

int clasificarDistancia(float distancia) {
  if (distancia >= ALTA_MIN && distancia <= ALTA_MAX) return 2;
  if (distancia >= BAJA_MIN && distancia <= BAJA_MAX) return 1;
  if (distancia > ALTURA_MAXIMA)                      return 0;
  return -1;
}

void loop() {
  unsigned long ahora = millis();

  for (int i = 0; i < 4; i++) {
    float distancia = medirDistancia(i);
    int lecturaActual = clasificarDistancia(distancia);

    if (lecturaActual == -1) {
      contadorCandidato[i] = 0;
      continue;
    }

    if (lecturaActual == estadoCandidato[i]) {
      contadorCandidato[i]++;
    } else {
      estadoCandidato[i] = lecturaActual;
      contadorCandidato[i] = 1;
    }

    if (contadorCandidato[i] >= UMBRAL_CONFIRMACION && lecturaActual != estadoSensor[i]) {

      if (lecturaActual == 1) {
        // Ignorar borde de salida de ficha grande
        if (estadoSensor[i] == 2) continue;

        // Ficha pequeña: guardar y esperar por si viene una grande
        notaPendiente[i] = (i * 2) + 1;
        tiempoUltimaDeteccion[i] = ahora;
        estadoSensor[i] = 1;
        contadorCandidato[i] = 0;

      } else if (lecturaActual == 2) {
        // Ficha grande: reproducir ya y cancelar pequeña pendiente
        notaPendiente[i] = 0;
        estadoSensor[i] = 2;
        contadorCandidato[i] = 0;
        int notaATocar = (i * 2) + 2;
        Serial.print("Sensor "); Serial.print(i + 1);
        Serial.print(" - Nota: "); Serial.println(notaATocar);
        reproductor.play(notaATocar);
        delay(10);

      } else if (lecturaActual == 0) {
        estadoSensor[i] = 0;
        contadorCandidato[i] = 0;
      }
    }

    // Si hay nota pequeña pendiente y ha pasado el tiempo, reproducirla
    if (notaPendiente[i] > 0 && (ahora - tiempoUltimaDeteccion[i]) > TIEMPO_ESPERA) {
      Serial.print("Sensor "); Serial.print(i + 1);
      Serial.print(" - Nota: "); Serial.println(notaPendiente[i]);
      reproductor.play(notaPendiente[i]);
      delay(10);
      notaPendiente[i] = 0;
    }
  }
}
