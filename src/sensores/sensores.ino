CÓDIGO 4 SENSORES:
const int Trig[] = {9, 7, 5, 3}; 
const int Echo[] = {8, 6, 4, 2}; 

const int umbralDeteccion = 15; //Distancia entre sensor y disco

void setup() {
  Serial.begin(115200);
  for(int i=0; i<4; i++) {
    pinMode(Trig[i], OUTPUT);
    pinMode(Echo[i], INPUT);
  }
  Serial.println("--- ESPERANDO FICHAS ---");
}

int medir(int i) {
  digitalWrite(Trig[i], LOW);
  delayMicroseconds(5);
  digitalWrite(Trig[i], HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig[i], LOW);
  
  long tiempo = pulseIn(Echo[i], HIGH, 20000); 
  if (tiempo == 0) return 999;
  return tiempo * 0.034 / 2;
}

void loop() {
  for(int i=0; i<4; i++) {
    int distancia = medir(i);

    // Si la distancia es menor al umbral, hay una ficha
    if (distancia < umbralDeteccion) {
      Serial.print("S");
      Serial.print(i + 1);
      Serial.print(": ¡FICHA! (");
      Serial.print(distancia);
      Serial.print("cm)\t");
    } else {
      Serial.print("S");
      Serial.print(i + 1);
      Serial.print(": Vacío  \t");
    }
    
    delay(30); 
  }
  Serial.println(); 
  delay(200); 
}
