#include <Arduino.h>  //Bibliothèque Arduino
#include <Wire.h> //Bibliothèque I2C

#define Gain 0.5  // Gain variable de la carte -> 1 paramètre de base 1V pour 1ms 
                  //                           -> 0.5 :   1ms => 0.5V
                  //                           -> 0.25 :  1ms => 0.25V
                  //                           -> 2 :     1ms => 2V

#define Adress_dac 0x48 // Adresse du DAC 
#define INJ1 0x30 // Adresse DAC sorti 1
#define INJ2 0x31 // Adresse DAC sorti 2
#define INJ3 0x32 // Adresse DAC sorti 3
#define INJ4 0x33 // Adresse DAC sorti 4

#define IN1 18    // Pin entré 1
#define IN2 19    // Pin entré 2
#define IN3 22    // Pin entré 3
#define IN4 23    // Pin entré 4

unsigned int FRONT;   // Variable pour lancer la transmission des données au DAC
unsigned int INJ,IN;  // Variable pour faire correspondre la sortie
unsigned long int TI; // Temps initial
unsigned long int TF;   // Temps final

void ES_Init()  // Fonction initialisation PIN
{
  pinMode(IN1, INPUT);       // Signal 1
  pinMode(IN2, INPUT);       // Signal 2
  pinMode(IN3, INPUT);       // Signal 3
  pinMode(IN4, INPUT);       // Signal 4

  pinMode(15, OUTPUT);      // LED test fonctionnement SAMD21
  digitalWrite(15,LOW);     // Initialisation Led éteinte
}

void DacInit()  // Fonction initialisation DAC
{
  pinMode(10, OUTPUT);      // Clear en sortie du DAC
  pinMode(11, OUTPUT);      // Ldac en sortie du DAC
  digitalWrite(10, LOW);    // Clear à 0
  digitalWrite(11, LOW);    // Ldac à 0
  digitalWrite(10, HIGH);   // Remise à 0 du DAC
}

void DacWrite (unsigned int data, unsigned int channel)
{
  int MSBD, LSBD;       // Variable bit de start et stop

  MSBD = (data & 0xFF0) >> 4;           // 8 bits de poids fort
  LSBD = ((data & 0x00F) << 4 ) & 0x0FF;// 8 bits de poids faible

  Wire.beginTransmission(Adress_dac);   // Commence la transmission sur l'adresse du DAC
  Wire.write(channel);                  // Commence la transmission sur la sortie
  Wire.write(MSBD);                     // On envoie 8 bits de poids forts
  Wire.write(LSBD);                     // On envoie 8 bits de poids faible
  Wire.endTransmission();               // On termine la transmission
}


void chrono1()  // Fonction d’interruption Pin 18 ( entré 1 )
{
  FRONT = 1;  // Drapeau de gestion de transmission à 1
  INJ = INJ1;   // Drapeau de gestion de sortie à INJ1 ( address 0x30 )
  IN=IN1;       // Drapeau de gestion de lecture de la pin
}

void chrono2()  // Fonction d'interruption Pin 19 ( entré 2 )
{
  FRONT = 1;  // Drapeau de gestion de transmission à 1
  INJ = INJ2;   // Drapeau de gestion de sortie à INJ2 ( address 0x31 )
  IN=IN2;       // Drapeau de gestion de lecture de la pin
}

void chrono3()  // Fonction d'interruption Pin 22 ( entré 3 )
{
  FRONT = 1;  // Drapeau de gestion de transmission à 1
  INJ = INJ3;   // Drapeau de gestion de sortie à INJ3 ( address 0x32 )
  IN=IN3;       // Drapeau de gestion de lecture de la pin
}

void chrono4()  // Fonction d'interruption Pin 23 ( entré 4 )
{
  FRONT = 1;  // Drapeau de gestion de transmission à 1
  INJ = INJ4;   // Drapeau de gestion de sortie à INJ4 ( address 0x33 )
  IN=IN4;       // Drapeau de gestion de lecture de la pin
}

void setup()  // Fonction Initialisation Microcontroleur 
{
  Wire.begin(); // Démarrage I2C
  Wire.setClock(3400000); // 180000 bauds
  ES_Init();  // Initialisation des ports d'entrée
  DacInit();    // Initialisation des ports d'activation du DAC
  Serial.begin(9600); // Démarrage de la liaison série 

  attachInterrupt(digitalPinToInterrupt(18), chrono1, FALLING); // procédure d'interruption entré 1, Pin 18
  attachInterrupt(digitalPinToInterrupt(19), chrono2, FALLING); // procédure d'interruption entré 2, Pin 19
  attachInterrupt(digitalPinToInterrupt(22), chrono3, FALLING); // procédure d'interruption entré 3, Pin 22  
  attachInterrupt(digitalPinToInterrupt(23), chrono4, FALLING); // procédure d'interruption entré 4, Pin 23  
}

void loop()    // Fonction principal
{
  if (FRONT == 1)     // Si un fonction d’interruption est activé
  {
    TI = micros();    // TI = valeur du compteur au temps présent
    while(digitalRead(IN)==0);  // Tant qu’on lit un état bas sur une Pin
    TF = micros();    // TF = valeur du compteur au temps présent
    DacWrite((TF-TI)*Gain, INJ);  // Fonction d’écriture du DAC
    FRONT = 0;      // Remettre le drapeau à un pour bien transmettre d’autre valeur
  }
}