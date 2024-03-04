
// Code Banc de Test CIFAM : Projet Janvier - Juin 2023 //

// Librairies
#include <Arduino.h>
#include <Wire.h>

// Addresses DAC
#define Adress_dac 0x48 // Adresse du DAC 
#define INJ1 0x30  // Adresse DAC sorti 1
#define INJ2 0x31 // Adresse DAC sorti 2
#define INJ3 0x32 // Adresse DAC sorti 3
#define INJ4 0x33 // Adresse DAC sorti 4
#define INJ5 0x34  // Adresse DAC sorti 5
#define INJ6 0x35 // Adresse DAC sorti 6


// Pin DAC
#define LCLEAR 6
#define LDAC 12

// Led
#define Led 13

// Fonction 
void DacInit();
void DacWrite(unsigned int data, unsigned int channel); 
void GenePW(unsigned int channel, unsigned int EtatBas);
void GeneSignal(unsigned int channel, unsigned int EtatBas, unsigned int EtatHaut);


void setup() 
{
 Serial.begin(9600);
 Serial.println("Serial ok");
 Wire.begin();
 Serial.println("Wire ok");
 DacInit();
 Serial.println("DAC ok");
 pinMode(Led, OUTPUT);
 pinMode(LCLEAR, OUTPUT);
 pinMode(LDAC, OUTPUT);
 digitalWrite(Led, HIGH);
 Serial.println("Setup ok");
}

void loop() 
{
  int Periode = 130000; //µs
  int Temps = 43000; //µs -> Dephasage
  GenePW(INJ2, 100); // Channel, Temps EtatBas
  delayMicroseconds(250); 
  GeneSignal(INJ4, 6000, 1000); // Channel, EtatBas, EtatHaut
  delayMicroseconds(Temps);
  GeneSignal(INJ4, 6000, 1000); // Channel, EtatBas, EtatHaut
  delayMicroseconds(Temps);  
}

void GenePW(unsigned int channel, unsigned int EtatBas)
{
  DacWrite(0, channel);
  delayMicroseconds(EtatBas);
  DacWrite(4000,channel);
}

void GeneSignal(unsigned int channel, unsigned int EtatBas, unsigned int EtatHaut)
{
  DacWrite(0, channel);
  delayMicroseconds(EtatBas);
  DacWrite(4000,channel);
  delayMicroseconds(EtatHaut);
  DacWrite(2000,channel);
}

void DacInit()  // Fonction initialisation DAC
{
  pinMode(LCLEAR, OUTPUT);      // Clear en sortie du DAC
  pinMode(LDAC, OUTPUT);      // Ldac en sortie du DAC
  digitalWrite(LCLEAR, LOW);    // Clear à 0
  digitalWrite(LDAC, LOW);    // Ldac à 0
  digitalWrite(LCLEAR, HIGH);   // Remise à 0 du DAC
}

void DacWrite(unsigned int data, unsigned int channel)
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
