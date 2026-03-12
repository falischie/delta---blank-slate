////////////////////////////////////////
// Button Matrix Module (delta^)
// main.cpp
// Aimo Scampa
// June 2021
////////////////////////////////////////
#include <Arduino.h>
#include <stdint.h>
#include "SPI.h"
#include "functions.h"
#include <Bounce.h>
#include <EEPROM.h>

void setup()
{
  SPI.begin();
  hardwareInit();
  // attachInterrupt(9, CLKinState, FALLING);

}
////////////////////////////////////////
void loop()
{
  // cMillis = millis();
  // cMicros = micros(); 
  buttonRead();
  buttonReadCounter ++;
  if(buttonReadCounter > 3) buttonReadCounter = 0;

///////MATRIX//////////////////////////////////// 
  for(int row = 0; row < 3; row++)
  {    
    for(int column = 0; column < 3; column++)
    {
      // inside here I do all the button related stuff
    }
  ledSetH();
  gateSetH(); 
}
//END/////////////////////////////////////