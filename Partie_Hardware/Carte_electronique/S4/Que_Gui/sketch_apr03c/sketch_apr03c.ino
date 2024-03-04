#include <Arduino.h>
#include "sam.h"

// Pin
#define Sortie A3

// Timer
#define Frequence 40000
float Periode = (1/Frequence)/2;


void TimerInit()
{
  int value;
  value = Periode / 83.3;
  Serial.println(value);
  
  PM->APBCMASK.reg |= 1 << (3 + 8);                         // Activation Timer 6
  GCLK->CLKCTRL.reg = 1 << 4 | 0 << 8 | 0x1B;               // Activation | Génération ( 48MHz ) | TimerSetting

  TC3->COUNT16.CTRLA.reg =  2 << 8 | 1 << 5;                // Prescalier | Match Fréquency  
  TC3->COUNT16.CC[0].reg = value;                           // 12.5µs / 83.3ns = 150 -> Periode / Resolution = CC[0]
  TC3->COUNT16.INTFLAG.bit.MC0 = 1;                         // Initialisation du Flag
  TC3->COUNT16.CTRLA.bit.ENABLE = 1;                        // Démarrage
  NVIC_EnableIRQ(TC3_IRQn);                                 // Activation Interruption
  //NVIC_SetPriority(TC3_IRQn, 1);                            // Priorité du processus
}

void digitalToggle(unsigned char Pin)
{
  PORT_IOBUS->Group[0].OUTTGL.reg = 1 << Pin;
}

void TC3_Handler()
{
  // Action
  digitalToggle(Sortie);
  
  // Reset
  TC3->COUNT16.INTFLAG.reg = 1 << 4;
}

void setup() 
{
  Serial.begin(9600);
  pinMode(Sortie, OUTPUT);
  TimerInit();
}

void loop() 
{

}
