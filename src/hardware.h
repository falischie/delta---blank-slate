////////////////////////////////////////
// Button Matrix Module (delta^)
// hardware.h
// Aimo Scampa
// June 2021
////////////////////////////////////////
#include <Arduino.h>
#include "SPI.h"
#include <stdint.h>
#include <Bounce.h>
#include <EEPROM.h>
//#defines//////////////////////////////
#define DEBOUNCE2 5
#define BHOLD 400
#define SHORT 200
//gate length///////////////////////////
#define GT 10
//gpio pins for addressing button rows//
#define ROW_0 10 
#define ROW_1 24
#define ROW_2 25
#define ROW_3 0
//shift register bits for button columns
#define COLUMN_0 4 
#define COLUMN_1 5
#define COLUMN_2 6
#define COLUMN_3 7  
//shift register bits for gate outs/////
#define OUT_1 0
#define OUT_2 1
#define OUT_3 2
#define OUT_4 3
#define OUT_5 4
#define OUT_6 5
////Variables///////////////////////////
typedef struct button
{
  bool buttonState = 0;
  bool previousButtonState = 0;
  bool currentButtonState = 0;
  bool ledStateA = 0; // yellow
  bool ledStateB = 0; // white
  bool gateState;
} t_button;
/////////////////////////////////////////
t_button matrix[4][3];
uint8_t buttonBits;
uint8_t gateBits = 0b00000111;
uint8_t buttonReadCounter = 0;

uint8_t lsb = 0b01110000;
uint8_t msb = 0;
uint8_t Llsb = 0;

//DEBOUNCE///
//BUTTON MTX///////////////////////
Bounce button00 = Bounce(ROW_0, 5);
Bounce button01 = Bounce(ROW_0, 5);
Bounce button02 = Bounce(ROW_0, 5);
Bounce button10 = Bounce(ROW_1, 5);
Bounce button11 = Bounce(ROW_1, 5);
Bounce button12 = Bounce(ROW_1, 5);
Bounce button20 = Bounce(ROW_2, 5);
Bounce button21 = Bounce(ROW_2, 5);
Bounce button22 = Bounce(ROW_2, 5);
//MODE BUTTONS/////////////////////
Bounce button30 = Bounce(ROW_3, 15);
Bounce button31 = Bounce(ROW_3, 15);
Bounce button32 = Bounce(ROW_3, 15);
Bounce button03 = Bounce(ROW_0, 15); // new button set !!!
Bounce button13 = Bounce(ROW_1, 15); //
//ENCODERS//////////////////////////
Bounce Encoder1A = Bounce(23, 1);
Bounce Encoder1B = Bounce(22, 1);
Bounce Encoder2A = Bounce(3, 1);
Bounce Encoder2B = Bounce(4, 1);
Bounce Encoder3A = Bounce(8, 1);
Bounce Encoder3B = Bounce(7, 1);

//CLK IN////////////////////////////
Bounce CLKIN = Bounce(9, 15);

//ENCODERS////////////
//1
bool lastStateCLKA = digitalRead(22);
bool currentStateCLKA;
int encCounterA = 0;
int prevEncA = 0;

//2
bool lastStateCLKB = digitalRead(3);
bool currentStateCLKB;
int encCounterB = 0;
int prevEncB = 0;

//3
bool lastStateCLKC = digitalRead(8);
bool currentStateCLKC;
int encCounterC = 0;
int prevEncC = 0;

//PWM_COLORS/////////////////////////////
uint16_t REDpositive;
uint16_t REDnegative;
uint16_t GREEN;
uint16_t BLUE;
uint16_t YELLOW;
uint16_t PINK;
uint16_t PURPLE;

////Hardware setup///////////////////////
void hardwareInit()
{
    // Button Matrix Row Select
    pinMode(ROW_0, INPUT); 
    pinMode(ROW_1, INPUT);  
    pinMode(ROW_2, INPUT);  
    pinMode(ROW_3, INPUT);  
    // LED shiftRegister Chip Select
    pinMode(21, OUTPUT);
    digitalWrite(21, HIGH);  
    // Button Matrix ShiftRegister Chip Select
    pinMode(2, OUTPUT);
    digitalWrite(2, HIGH);   
    //SPI
    SPI.setMOSI(11);
	  SPI.setSCK(13);
    // RGB led
    pinMode(20, OUTPUT); // R
    pinMode(16, OUTPUT); // G
    pinMode(17, OUTPUT); // B 
    // DAC (CV OUT 2 and 3)
    pinMode(5, OUTPUT);
    digitalWrite(5, HIGH);
    // ADCs
    analogReadResolution(12);
    // M0 DAC (CV OUT 1)
    analogWriteResolution(12);
    // ENCODER 1
    pinMode(23, INPUT); // ENC 1 B
    pinMode(22, INPUT); // ENC 1 A
    pinMode(19, INPUT); // ENC 1 BUTTON
    // ENCODER 2
    pinMode(4, INPUT); // ENC 2 B
    pinMode(3, INPUT); // ENC 2 A
    pinMode(6, INPUT); // ENC 2 BUTTON
    // ENCODER 3
    pinMode(7, INPUT); // ENC 3 B
    pinMode(8, INPUT); // ENC 3 A
    pinMode(1, INPUT); // ENC 3 BUTTON
    // INS
    //pinMode(16, INPUT); // OLD CLOCK IN
    pinMode(9, INPUT); // NEW CLOCK IN
    pinMode(A1, INPUT); // CV IN 1
    pinMode(A0, INPUT); // CV IN 2
    pinMode(A4, INPUT); // CV IN 3
}
////Shift Registers//////////////////////
void shiftButton(uint8_t data) // SHIFT REGISTER CONTROLLING GATE
{
    buttonBits &= 0b00001111;
    buttonBits |= (1 << data);
    buttonBits &= 0b11110000;
    buttonBits |= gateBits;
    // SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0));
    digitalWrite(2, LOW); 
    SPI.transfer(buttonBits);
    digitalWrite(2, HIGH);
    // SPI.endTransaction();

}
/////////////////////////////////////////
void shiftLED() // SHIFT REGISTER CONTROLLING LEDs
{   
    digitalWrite(21, LOW); 
    SPI.transfer(Llsb);
    SPI.transfer(lsb);
    SPI.transfer(msb);
    digitalWrite(21, HIGH);
}
////Buttons//////////////////////////////
void buttonRead()
{
  if(buttonReadCounter == 0)
  {
    shiftButton(COLUMN_0);
    if(button00.update())
    {
        if(button00.risingEdge()) matrix[0][0].buttonState = 1;
        else matrix[0][0].buttonState = 0;
    }
    if(button10.update())
    {
        if(button10.risingEdge()) matrix[1][0].buttonState = 1;
        else matrix[1][0].buttonState = 0;
    }
    if(button20.update())
    {
        if(button20.risingEdge()) matrix[2][0].buttonState = 1;
        else matrix[2][0].buttonState = 0;

    }
    //ERASE BTN///////
    if(button30.update())
    {
        if(button30.risingEdge())
        {
          eraseBTN = 1;
        }
        else 
        {
          eraseBTN = 0;
        }
    }
  }
  else if(buttonReadCounter == 1)
  {
    shiftButton(COLUMN_1);
    if(button01.update())
    {
        if(button01.risingEdge()) matrix[0][1].buttonState = 1;
        else matrix[0][1].buttonState = 0;
    }
    
    if(button11.update())
    {
        if(button11.risingEdge()) matrix[1][1].buttonState = 1;
        else matrix[1][1].buttonState = 0;
    }
    
    if(button21.update())
    {
        if(button21.risingEdge()) matrix[2][1].buttonState = 1;
        else matrix[2][1].buttonState = 0;
    }
    //PASTE BTN////////////////
    if(button31.update())
    {
        if(button31.risingEdge()) pasteBTN = 1;
        else pasteBTN = 0;
    }
  }
  else if(buttonReadCounter == 2)
  {
    shiftButton(COLUMN_2);
    if(button02.update())
    {
        if(button02.risingEdge())  matrix[0][2].buttonState = 1;
        else matrix[0][2].buttonState = 0;
    }
    if(button12.update())
    {
        if(button12.risingEdge()) matrix[1][2].buttonState = 1;
        else  matrix[1][2].buttonState = 0;
    }
    if(button22.update())
    {
        if(button22.risingEdge()) matrix[2][2].buttonState = 1;
        else  matrix[2][2].buttonState = 0;
    }
  
    //COPY BTN//////////////////
    if(button32.update())
    {
        if(button32.risingEdge())
        {
            copyBTN = 1;
        }
        else 
        {
            copyBTN = 0;
        }
    }
  }
  if(buttonReadCounter == 3)
  {
    shiftButton(COLUMN_3);
    // T BTN /////
    if(button03.update())
    {
        if(button03.risingEdge()) cvBTNheld = HIGH;
        else cvBTNheld = LOW;
    }
    // GRAVITY BTN /////
    if(button13.update())
    {
        if(button13.risingEdge()) gravitybtnHeld = HIGH;
        else gravitybtnHeld = LOW;
    }
  }
}
/////////////////////////////////////////
void gateSetH() // GATE OUTPUTS
{  

    ////GATE_1////////////////////
    if(matrix[1][0].gateState) bitWrite(gateBits,0,0);
    else bitWrite(gateBits,0,1); 
    ////GATE_4//////////////////////
    if(matrix[2][0].gateState)bitWrite(lsb,4,0);
    else bitWrite(lsb,4,1);
    ////GATE_2///////////////////
    if(matrix[1][1].gateState) bitWrite(gateBits,1,0); 
    else bitWrite(gateBits,1,1);
    ////GATE_5///////////////////
    if(matrix[2][1].gateState) bitWrite(lsb,5,0);
    else bitWrite(lsb,5,1);
    ////GATE_3///////////////////
    if(matrix[1][2].gateState) bitWrite(gateBits,2,0);
    else bitWrite(gateBits,2,1);
    ////GATE_6////////////////
    if(matrix[2][2].gateState) bitWrite(lsb,6,0);
    else bitWrite(lsb,6,1);
}
/////////////////////////////////////////
void ledSetH()
{   

  //Orange LEDs/////
    if (matrix[0][0].ledStateA == 1) msb = bitWrite(msb,0,1);
    else if (matrix[0][0].ledStateA == 0) msb = bitWrite(msb,0,0);

    if (matrix[1][0].ledStateA == 1) bitWrite(msb,1,1);
    else if (matrix[1][0].ledStateA == 0) bitWrite(msb,1,0);

    if (matrix[2][0].ledStateA == 1) bitWrite(msb,2,1);
    else if (matrix[2][0].ledStateA == 0) bitWrite(msb,2,0);
    
    if (matrix[0][1].ledStateA == 1) bitWrite(msb,3,1);
    else if (matrix[0][1].ledStateA == 0) bitWrite(msb,3,0);

    if (matrix[1][1].ledStateA == 1) bitWrite(msb,4,1);
    else if (matrix[1][1].ledStateA == 0) bitWrite(msb,4,0);

    if (matrix[2][1].ledStateA == 1) bitWrite(msb,5,1);
    else if (matrix[2][1].ledStateA == 0) bitWrite(msb,5,0);

    if (matrix[0][2].ledStateA == 1) bitWrite(msb,6,1);
    else if (matrix[0][2].ledStateA == 0) bitWrite(msb,6,0);

    if (matrix[1][2].ledStateA == 1) bitWrite(msb,7,1);
    else if (matrix[1][2].ledStateA == 0) bitWrite(msb,7,0);

    if (matrix[2][2].ledStateA == 1) bitWrite(lsb,0,1);
    else if (matrix[2][2].ledStateA == 0) bitWrite(lsb,0,0);

    //White LEDs///////   
    if (matrix[0][0].ledStateB == 1) bitWrite(Llsb,7,1);
    else if (matrix[0][0].ledStateB == 0) bitWrite(Llsb,7,0);

    if (matrix[1][0].ledStateB == 1) bitWrite(Llsb,6,1);
    else if (matrix[1][0].ledStateB == 0) bitWrite(Llsb,6,0);

    if (matrix[2][0].ledStateB == 1) bitWrite(Llsb,5,1);
    else if (matrix[2][0].ledStateB == 0) bitWrite(Llsb,5,0);

    if (matrix[0][1].ledStateB == 1) bitWrite(Llsb,4,1);
    else if (matrix[0][1].ledStateB == 0) bitWrite(Llsb,4,0);

    if (matrix[1][1].ledStateB == 1) bitWrite(Llsb,3,1);
    else if (matrix[1][1].ledStateB == 0) bitWrite(Llsb,3,0);

    if (matrix[2][1].ledStateB == 1) bitWrite(Llsb,2,1);
    else if (matrix[2][1].ledStateB == 0) bitWrite(Llsb,2,0);

    if (matrix[0][2].ledStateB == 1) bitWrite(Llsb,1,1);
    else if (matrix[0][2].ledStateB == 0) bitWrite(Llsb,1,0);

    if (matrix[1][2].ledStateB == 1) bitWrite(Llsb,0,1);
    else if (matrix[1][2].ledStateB == 0) bitWrite(Llsb,0,0);
    
    if (matrix[2][2].ledStateB == 1) bitWrite(lsb,7,1);
    else if (matrix[2][2].ledStateB == 0) bitWrite(lsb,7,0);

    shiftLED();   
}
////CLK IN STATE/////////////////////////
void CLKinState()
{
  if(CLKIN.update())
  {
      if(CLKIN.fallingEdge())CLKINstate = 1;
      else CLKINstate = 0;
  }

}
////DUCKS////////////////////////////////
void duckOUT(int channel, unsigned int value)
{
    if(channel == 0) // CV OUT 2
    {
        digitalWrite(5, LOW); 
        byte data = value >> 8;   
        data &= B00001111;  
        data |= B00110000; 
        SPI.transfer(data); 
        data = value;
        SPI.transfer(data); 
        digitalWrite(5, HIGH); 
    }
     if(channel == 1) // CV OUT 3
    {
        digitalWrite(5, LOW); 
        byte data = value >> 8;  
        data &= B00001111;  
        data |= B10110000; 
        SPI.transfer(data); 
        data = value;
        SPI.transfer(data); 
        digitalWrite(5, HIGH); 
    }
}
/////////////////////////////////////////
void m0OUT(unsigned int value) // CV OUT 1
{
  analogWrite(A12,value); 
}
//END////////////////////////////////////