#include <Arduino.h>
#include "sam.h"
#include "Cifam.hpp"

CIFAM cifam(MODE_INJECTION);
//CIFAM cifam(MODE_TEMPRETARD);

void setup() 
{
  cifam.Init();
  cifam.GainOut1(0.5);
  cifam.GainOut2(1);
  cifam.GainOut3(1);
  cifam.GainOut4(1);
}

void loop() 
{
  
}
