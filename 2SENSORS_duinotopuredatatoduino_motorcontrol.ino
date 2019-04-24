// Variables globales
// Ces variables servent à l'encodage en SLIP.
#define trigPin 13
#define echoPin 12
#define trigPin2 7
#define echoPin2 6
#include <Servo.h>

Servo myservo;  // create servo object to control a servo
Servo myservo2; 
// twelve servo objects can be created on most boards

int pos = 0;  
int pos2 = 0;// variable to store the servo position
unsigned long previousMillis = 0;        // will store last time LED was updated
unsigned long previousMillis2 = 0;
// constants won't change:
const long interval = 1000; 

const byte END=192;
const byte ESC=219; 
const byte ESC_END=220;
const byte ESC_ESC=221;


int vel [] = {170, 170};
int packetSize;

const int sensor = 10; //L'entrée analogique utilisée.

// La taille maximum d'un paquet SLIP.  Ce nombre doit être plus grande que le
// paquet le plus grand attendu.
byte slipPacket[256]; // Cet array contiendra le paquet SLIP reçu.

void setup() {
  Serial.begin(115200);


  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);  
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
  myservo2.attach(3); 
}

void loop() {

packetSize = 0;

    
packetSize = SLIPSerialRead( slipPacket );
//
//for(int i=0; i<packetSize;i++) {
//    vel[i] = slipPacket[i];
//  } 
  
  if(packetSize > 0) {
    for(int i=0; i<packetSize;i++) {
    vel[i] = slipPacket[i];
    
  } 
  myservo.write(vel[0]);
  myservo2.write(vel[1]);
  }  

//  myservo.write(vel[1]);
// 
//  unsigned long currentMillis = millis();
//
//   if (currentMillis - previousMillis >= interval) {
//     previousMillis = currentMillis;
//
//     myservo.write(vel[1]);
////
////      if (pos < 180) {
////        pos += vel[1];
////       myservo.write(pos); 
////      } else if (pos > 180 || pos > 0 ) {
////       pos -= 180;
////       myservo.write(pos); 
////    }
//}
   unsigned long currentMillis2 = millis();

    if (currentMillis2 - previousMillis2 >= interval) {
      previousMillis2 = currentMillis2;

      
//
//      if (pos2 < 180) {
//        pos2 += 10;
//       myservo2.write(pos2); 
//      } else if (pos2 > 180 || pos2 > 0 ) {
//       pos2 -= 180;
//       myservo2.write(pos2); 
//     }
   }
    

    
    //  for (pos = 0; pos <= 180; pos += 30) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
      //myservo.write(pos);              // tell servo to go to position in variable 'pos'
   // delay(15);                       // waits 15ms for the servo to reach the position
 // }
    //  for (pos = 180; pos >= 0; pos -= 30) { // goes from 180 degrees to 0 degrees
     // myservo.write(pos);              // tell servo to go to position in variable 'pos'
   // delay(15);                       // waits 15ms for the servo to reach the position
 //}
//}
  long duration, distance;
  long duration2, distance2;
  digitalWrite(trigPin, LOW);        
  delayMicroseconds(2);              
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);           
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 29.1;
  digitalWrite(trigPin2, LOW);        
  delayMicroseconds(2);              
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);           
  digitalWrite(trigPin2, LOW);
  duration2 = pulseIn(echoPin2, HIGH);
  distance2 = (duration2/2) / 29.1;
  {
    // On lit le capteur et on envoie un paquet SLIP.
    Serial.write(END);   // On commence le paquet.
    SLIPSerialWrite( byte(distance >> 8) );  // On envoie le MSB
    SLIPSerialWrite( byte(distance & 255) ); // On envoie le LSB
    SLIPSerialWrite( byte(distance2 >> 8) );  // On envoie le MSB
    SLIPSerialWrite( byte(distance2 & 255) );
    Serial.write(END);   // On termine le paquet.
  }
  
  delay(3);  // On attend un peu pour le port série et l'ADC. 
}

//Fonction pour encoder les paquets SLIP.
void SLIPSerialWrite(byte value){
  if(value == END) { // Si c'est la valeur 192, on remplace par 219 et 220.
    Serial.write(ESC);
    Serial.write(ESC_END);
    return;
  } else if (value == ESC) {  // Si c'est la valeur 219, on fait suivre un 221.
    Serial.write(ESC);
    Serial.write(ESC_ESC);
    return;
  } else { // On envoie toutes les autres valeurs normalement.
    Serial.write(value);
    return;
  }
}

// Fonction pour recevoir et décoder les paquets SLIP.
// Retourne la taille du paquet SLIP.
// Prend un paramètre: le tableau (array) d'octets (bytes) qui contiendra le paquet SLIP.
int SLIPSerialRead(byte * slipPacket) { 
  int packetIndex = 0;  // L'index du paquet SLIP reçu.  Cette valeur sera
  // incrémentée à chaque octet reçu.  Elle sera remise à
  // zéro quand un paquet SLIP sera complet.

  // Deux témoins (flags)
  boolean escape = false; // Ce témoin devient vrai quand un octet ESC
  // est reçu.  Il redevient faux quand l'octet
  // suivant (ESC_ESC ou ESC_END) est reçu.
  boolean packetComplete = false; // Ce témoin devient vrai quand un
  // paquet SLIP est complet,
  // c'est-à-dire quand l'octet END est
  // reçu.

  // S'il n'y a rien sur le port série, on retourne zéro immédiatement.
  if ( Serial.available() == 0 ) {
    return 0;
  }

  // S'il y a quelque chose, on attend un paquet complet.
  while( !packetComplete ) {
    if( Serial.available() > 0) {
      byte b = Serial.read();
      if (escape) { // Si l'octet précédent était ESC (219)
        if (b == ESC_END) {  // On ajoute END (192)...
          slipPacket[packetIndex] = END;
        } else if (b == ESC_ESC) { // ...ou ESC (219)
          slipPacket[packetIndex] = ESC;
        }
        packetIndex++;  
        escape = 0; // On remet le témoin escape à faux.
      } else if (b == END) { // Le paquet est terminé.
        packetComplete = true;  // Le témoin packetComplete est maintenant vrai.
      } else if (b == ESC) { // Si l'octet est ESC (219)
        escape = 1; // Le témoin escape est maintenant vrai. (On n'incrémente pas packetIndex.)
      } else { // Cas normal.
        slipPacket[packetIndex] = b; // On ajoute l'octet au paquet et
        packetIndex ++; // on incrémente packetIndex.
      }
    }
  }
  // On retourne la taille du paquet.
  return packetIndex;
}
