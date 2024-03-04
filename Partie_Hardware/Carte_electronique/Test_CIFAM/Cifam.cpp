#include <Arduino.h>
#include <Wire.h>
#include "Cifam.hpp"

CIFAM::CIFAM()
{}

CIFAM::CIFAM(int mode)
{
    if(mode == 1) _mode = new INJECTION();
    if(mode == 2) _mode = new TEMPRETARD();
}

void CIFAM::Init()
{
    Wire.begin();

    pinMode(LED_DEBUG, OUTPUT);
    digitalWrite(LED_DEBUG, LOW);

    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);

    pinMode(DACCLEAR, OUTPUT);
    pinMode(DACDLDAC, OUTPUT);
    digitalWrite(DACCLEAR, LOW);
    digitalWrite(DACDLDAC, LOW);
    digitalWrite(DACCLEAR, HIGH);
}

void CIFAM::DacWrite(unsigned long int data, unsigned long int channel)
{
  int MSBD, LSBD; 			

  MSBD = ((data & 0xFF0) >> 4 );          
  LSBD = ((data & 0x00F) << 4 ) & 0x0FF;

  Wire.beginTransmission(ADDRESS_DAC);  	
  Wire.write(channel);                  
  Wire.write(MSBD);                    	
  Wire.write(LSBD);                    
  Wire.endTransmission();             
}

void CIFAM::GainOut1(float value)
{
    _Gain1 = value;
}

void CIFAM::GainOut2(float value)
{
    _Gain2 = value;
}

void CIFAM::GainOut3(float value)
{
    _Gain3 = value;
}

void CIFAM::GainOut4(float value)
{
    _Gain4 = value;
}

CIFAM::~CIFAM()
{
    delete _mode;
}
