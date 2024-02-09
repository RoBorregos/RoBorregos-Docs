//Pines y conexiones para el brazo SCARA

const int dirPinM1 = 24;//Segundo Motor
const int stepPinM1 = 23;//Segundo  Motor
const int enableM1 = 22;//Segundo Motor
const int dirPinM2 = 32;//Tercer Motor
const int stepPinM2 = 31;//Tercer Motor
const int enableM2 = 30;// Tercer Motor
const int dirPinM0 = 27;//Cero motor
const int stepPinM0 = 26;//Cero motor
const int enableM0 = 25;//Cero motor
const int dirPinM4 = 35;//  Cuarto motor
const int stepPinM4 = 34;//Cuarto motor
const int enableM4 = 33;//Cuarto motor
//const int dirPinM3 = 47;// Tercer motor
//const int stepPinM3 = 48;//Tercer motor
//const int enableM3 = 49;//Tercer motor
//const int dirPinM5 = 37;// Quinto motor
//const int stepPinM5 = 53;//Quinto motor
//const int enableM5 = 42;//Quinto motor
//const int servo1 = 46;
//const int servo2 = 45;
//const int servo3 = 44;
//const int servo4 = 4;
//const int servo5 = 5;
//const int servo6 = 8;

const int steps = 5000;
int stepDelay;

void girarMotor(const int stepPin, const int dirPin){
  //Activar una direccion y fijar la velocidad con stepDelay
   digitalWrite(dirPin, HIGH);
   stepDelay = 250;
   // Giramos 200 pulsos para hacer una vuelta completa
   for (int x = 0; x < steps * 1; x++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(stepDelay);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(stepDelay);
   }
   delay(500);

   //Cambiamos la direccion y aumentamos la velocidad
   digitalWrite(dirPin, LOW);
   stepDelay = 150;
   // Giramos 400 pulsos para hacer dos vueltas completas
   for (int x = 0; x < steps; x++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(stepDelay);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(stepDelay);
   }
   delay(500);
}

void setPinout(const int enable, const int stepPin, const int dirPin){
   pinMode(dirPin, OUTPUT);
   pinMode(stepPin, OUTPUT);
   pinMode(enable, OUTPUT);
   digitalWrite(enable, LOW);
}

void setup() {
   // Marcar los pines como salida
   setPinout(enableM1,stepPinM1,dirPinM1);
   setPinout(enableM2,stepPinM2,dirPinM2);
   setPinout(enableM0,stepPinM0,dirPinM0);
   setPinout(enableM4,stepPinM4,dirPinM4);
}

void loop() {
   //girarMotor(stepPinM1, dirPinM1);
   girarMotor(stepPinM2, dirPinM2);
   //girarMotor(stepPinM0, dirPinM0);
   //girarMotor(stepPinM4, dirPinM4);
}
