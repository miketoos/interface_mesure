#include <Arduino.h>
#include "Cifam.h"
#include "TempRetard.hpp"
#include "Cifam.hpp"

CIFAM cifamtempretard;

unsigned int T1;
unsigned int T1BIS;
unsigned int T3;
unsigned int T4;
unsigned int T5;

float TRegimeMoteur;
float AngleAllumage;
float TChargeBobine;
float TDureeEtin;

TEMPRETARD::TEMPRETARD()
{
    //attachInterrupt(digitalPinToInterrupt(IN1), TEMPRETARD::Chrono1, FALLING);	
    attachInterrupt(digitalPinToInterrupt(IN2), TEMPRETARD::Chrono2, FALLING); 
    attachInterrupt(digitalPinToInterrupt(IN3), TEMPRETARD::Chrono3, CHANGE); 
    attachInterrupt(digitalPinToInterrupt(IN4), TEMPRETARD::Chrono4, FALLING); 	    
}


void TEMPRETARD::Chrono1()
{}

void TEMPRETARD::Chrono2()
{
    T1BIS = T1;
    T1 = micros();
    TRegimeMoteur = (T1 - T1BIS)/2;
    TRegimeMoteur = (TRegimeMoteur * 3600) / 1000;
    cifamtempretard.DacWrite(TRegimeMoteur * cifamtempretard._Gain1, OUT1);
}

unsigned int FLAG_1;
unsigned int FLAG_2;

void TEMPRETARD::Chrono3()
{
    FLAG_1++;
    if(FLAG_1 == 1)
    {
        T3 = micros();
    }

    if(FLAG_1 == 2)
    {
        T4 = micros();
        FLAG_2++;
        if(FLAG_2 == 1)
        {
            AngleAllumage = (T4 - T1);
            AngleAllumage = ((AngleAllumage * 360) / (T1 - T1BIS) * 10);
            cifamtempretard.DacWrite(AngleAllumage*cifamtempretard._Gain2, OUT2);
            FLAG_1 = 0;
        }
        if(FLAG_2 == 2)
        {
            TChargeBobine = (T4 - T3);
            cifamtempretard.DacWrite(TChargeBobine*cifamtempretard._Gain3, OUT3);
            FLAG_1 = 0;
            FLAG_2 = 0;
        }
    }
}

void TEMPRETARD::Chrono4()
{
    T5 = micros();
    TDureeEtin = (T5 - T4);
    cifamtempretard.DacWrite(TDureeEtin*cifamtempretard._Gain4, OUT4);
}


TEMPRETARD::~TEMPRETARD()
{

}
