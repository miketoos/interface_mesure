#include <Arduino.h>
#include "Cifam.h"
#include "Injection.hpp"
#include "Cifam.hpp"

CIFAM cifaminjection;

unsigned long int TI;
unsigned long int TF;
unsigned long int duree;

INJECTION::INJECTION()
{
    attachInterrupt(digitalPinToInterrupt(IN1), (Chrono1), FALLING);	
    attachInterrupt(digitalPinToInterrupt(IN2), (Chrono2), FALLING); 
    attachInterrupt(digitalPinToInterrupt(IN3), (Chrono3), FALLING); 
    attachInterrupt(digitalPinToInterrupt(IN4), (Chrono4), FALLING); 	    
}

void INJECTION::Chrono1()
{
    TI = micros(); 		
    while(digitalRead(IN1) == 0); 	
    TF = micros(); 		
    duree = TF - TI;     	
    cifaminjection.DacWrite((duree*cifaminjection._Gain1), OUT1);	
}

void INJECTION::Chrono2()
{
    TI = micros(); 		
    while(digitalRead(IN2) == 0); 	
    TF = micros(); 		
    duree = TF - TI; 		
    cifaminjection.DacWrite((duree*cifaminjection._Gain2), OUT2);
}

void INJECTION::Chrono3()
{
    TI = micros(); 		
    while(digitalRead(IN3) == 0); 	
    TF = micros(); 		
    duree = TF - TI; 		
    cifaminjection.DacWrite((duree*cifaminjection._Gain3), OUT3);	
}

void INJECTION::Chrono4()
{
    TI = micros(); 		
    while(digitalRead(IN4) == 0); 	
    TF = micros(); 		
    duree = TF - TI; 		
    cifaminjection.DacWrite((duree*cifaminjection._Gain3), OUT4);	
}


INJECTION::~INJECTION()
{

}
