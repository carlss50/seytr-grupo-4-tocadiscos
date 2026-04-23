#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>

#define PULSADOR 9
#define PULSADOR_ANUNCIO 8

SoftwareSerial softSerial(10, 11);

DFRobotDFPlayerMini reproductor;

bool reproduciendo = true;

void setup() {
  softSerial.begin(9600);
  Serial.begin(9600);
  pinMode(PULSADOR, INPUT_PULLUP);
  pinMode(PULSADOR_ANUNCIO, INPUT_PULLUP);

  if (!reproductor.begin(softSerial, true, false)){
    Serial.println("Error de inicialización.");
    while(true);  
  }
  Serial.println("Reproductor parado.");
  delay(1000);

  reproductor.volume(25);
  reproductor.enableLoopAll();
}

void loop() {
  if (digitalRead(PULSADOR) == LOW && reproduciendo == true){
    reproductor.pause();
    Serial.println("Pausa");
    reproduciendo = false;
    delay(500);
  }else if(digitalRead(PULSADOR) == LOW && reproduciendo == false){
    reproductor.start();
    Serial.println("Reanudando");
    reproduciendo = true;
    delay(500);
  }

  if (digitalRead(PULSADOR_ANUNCIO) == LOW){
    reproductor.advertise(1);
    Serial.println("Anuncio");
    delay(500);
  }
}
