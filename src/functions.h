////////////////////////////////////////
// Button Matrix Module (delta^)
// functions.h
// Aimo Scampa 
// June 2021
////////////////////////////////////////
#include "hardware.h"
// #include "windows.h"
///////////////////////////////////////
int buttonState(int row, int column)
{
  return matrix[row][column].buttonState;
}
////////////////////////////////////////
void buttonHold(int row, int column)
{
  if(matrix[row][column].buttonState && eraseHeld == LOW)
  {
    if(cMillis - matrix[row][column].buttonTimer > BHOLD)
    {
      matrix[row][column].buttonHeld = HIGH;
      // buttonHeldFlag = HIGH;
      matrix[row][column].timerRunning = LOW; 
      // holdFLAG = HIGH;
    }
  }
  else
  {
    matrix[row][column].buttonTimer = cMillis;
    matrix[row][column].buttonHeld = LOW;
  } 
}
////////////////////////////////////////
void buttonShort(int row, int column)
{
  matrix[row][column].currentButtonState = matrix[row][column].buttonState;

  if(matrix[row][column].currentButtonState != matrix[row][column].previousButtonState)
  {
    if(matrix[row][column].currentButtonState && matrix[row][column].shortButtonFlag == LOW)
    {
      matrix[row][column].buttonShortTimer = cMillis;
      matrix[row][column].shortButtonFlag = HIGH;
    }
    else if(matrix[row][column].buttonState == 0 && matrix[row][column].shortButtonFlag)
    {
      if(cMillis - matrix[row][column].buttonShortTimer < SHORT )
      {
        matrix[row][column].buttonShort = HIGH;
        matrix[row][column].shortButtonFlag = LOW;
      }
      else
      { 
        matrix[row][column].buttonShort = LOW;
        matrix[row][column].shortButtonFlag = LOW;
      }
    }
  }

  matrix[row][column].previousButtonState = matrix[row][column].currentButtonState;
  // matrix[row][column].buttonShort = LOW;
}
////////////////////////////////////////
int tapState(int row, int column)
{
    return matrix[row][column].tapState;
}
////////////////////////////////////////
int previousState(int row, int column)
{
    return matrix[row][column].previousState;
}
////////////////////////////////////////
int buttonSetA(int row, int column)
{
  return matrix[row][column].buttonSetA;
}
////////////////////////////////////////
int buttonSetB(int row, int column)
{
  return matrix[row][column].buttonSetB;
}
////////////////////////////////////////
int buttonSetC(int row, int column)
{
  return matrix[row][column].buttonSetC;
}
/////////////////////////////////////////
int buttonSetXOR(int row, int column)
{
  return matrix[row][column].buttonSetXOR;
}
/////////////////////////////////////////
int buttonSetCV(int row, int column)
{
  return matrix[row][column].buttonSetCV;
}
/////////////////////////////////////////
int muteSet(int row, int column)
{
  return matrix[row][column].mute;
}
/////////////////////////////////////////
void buttonHoldFlag()
{
  if(matrix[0][0].buttonHeld || matrix[0][1].buttonHeld || matrix[0][2].buttonHeld || 
  matrix[1][0].buttonHeld || matrix[1][1].buttonHeld || matrix[1][2].buttonHeld ||
  matrix[2][0].buttonHeld || matrix[2][1].buttonHeld || matrix[2][2].buttonHeld ) buttonHeldFlag = HIGH;
  else buttonHeldFlag = LOW;
}
/////////////////////////////////////////
void buttonSetFlip(int row, int column)
{
    matrix[row][column].buttonSetCV = !matrix[row][column].buttonSetCV;
}
/////LED'S///////////////////////////////
void ledSet(int row, int column, int state)
{   
    matrix[row][column].ledStateA = state;
}
/////////////////////////////////////////
void ledSetB(int row, int column, int state)
{   
    matrix[row][column].ledStateB = state;
}
/////GATES///////////////////////////////
void gateSet(int row, int column, int state)
{   
    matrix[row][column].gateState = state;
}
/////ENCODER READS///////////////////////
void updateEncoder1(int row, int column)
{
  if(Encoder1B.update())
    {
        if(Encoder1B.risingEdge())Encoder1BGate = 1;
        else Encoder1BGate = 0;
    }
  if(Encoder1A.update())
    {
        if(Encoder1A.risingEdge()) Encoder1AGate = 1;
        else Encoder1AGate = 0;
    }

// Read the current state of CLK
matrix[row][column].currentStateCLK1 = Encoder1BGate;

// If last and current state of CLK are different, then pulse occurred
// React to only 1 state change to avoid double count
if (matrix[row][column].currentStateCLK1 != matrix[row][column].lastStateCLK1) //  && matrix[row][column].currentStateCLK1 == 1)
{

// If the DT state is different than the CLK state then
// the encoder is rotating CCW so decrement
if (Encoder1AGate != matrix[row][column].currentStateCLK1) 
  {

  } 
  // Encoder is rotating CW so increment
  else 
  {


    
  }

}
// Serial.println(matrix[row][column].encCounterTap1);
// Remember last CLK state
matrix[row][column].lastStateCLK1 = matrix[row][column].currentStateCLK1;


}

}
//ENC1_SHORT//////////////////////////////
void encButtonShort1()
{
  currentEncButtonState1 = button1Flag;

  if(currentEncButtonState1 != previousEncButtonState1)
  {
    if(currentEncButtonState1 && shortEncButtonFlag1 == LOW)
    {
      encButtonShortTimer1 = cMillis;
      shortEncButtonFlag1 = HIGH;
    }
    else if(button1Flag == 0 && shortEncButtonFlag1)
    {
      if(cMillis - encButtonShortTimer1 < SHORT )
      {
        EncButtonShort1 = HIGH;
        shortEncButtonFlag1 = LOW;
      }
      else
      { 
        EncButtonShort1 = LOW;
        shortEncButtonFlag1 = LOW;
      }
    }
  }

  previousEncButtonState1 = currentEncButtonState1;
  // matrix[row][column].buttonShort = LOW;
}
/////DAC UPDATE/////////////////////////////
void updateDAC(){
  if(matrix[0][0].clocks)
  m0OUT(matrix[0][0].slewOUT);
  else m0OUT(0);  
  if(matrix[0][1].clocks)
  duckOUT(0, matrix[0][1].slewOUT);  
  else duckOUT(0, 0);      
  if(matrix[0][2].clocks)
  duckOUT(1, matrix[0][2].slewOUT); 
  else duckOUT(1, 0);
}
//CALIBRATION/////////////////
void calibrationProcedure()
{
      if((digitalRead(19) == 0) && (digitalRead(6) == 0) && (digitalRead(1) == 0))
    {
      digitalWrite(16, LOW);
      digitalWrite(20, HIGH);
      digitalWrite(17,LOW);
      delay(500);
      //LOOK FOR 0V
      // while(cvAflag)
      // {
      //   if(digitalRead(19) == 0)
      //   {
          uint16_t cvAread = (analogRead(A1));
          // Serial.println("cvA");
          // Serial.println(cvAzeroV);
          digitalWrite(16, HIGH);
          digitalWrite(20, HIGH);
          digitalWrite(17,HIGH);
          delay(500);
      //     cvAflag = 0;
      //   } 
      // }
      digitalWrite(16, HIGH);
      digitalWrite(20, LOW);
      digitalWrite(17,LOW);
      delay(500);
      // while(cvBflag)
      // {
      //   if(digitalRead(6) == 0)
      //   {
          uint16_t cvBread = (analogRead(A0));
          // Serial.println("cvB");
          // Serial.println(cvBzeroV);
          digitalWrite(16, HIGH);
          digitalWrite(20, HIGH);
          digitalWrite(17,HIGH);
          delay(500);
      //     cvBflag = 0;
      //   } 
      // }
      digitalWrite(16, LOW);
      digitalWrite(20, LOW);
      digitalWrite(17,HIGH);
      delay(500);
      // while(cvCflag)
      // {
      //   if(digitalRead(1) == 0)
      //   { 
          uint16_t cvCread  = analogRead(A4);
          // Serial.println("cvC");
          // Serial.println(cvCzeroV);
          digitalWrite(16, HIGH);
          digitalWrite(20, HIGH);
          digitalWrite(17,HIGH);
          delay(500);
          // cvCflag = 0;
          calibration = 0;
      //   }
      // }
      //save to EEPROM
      byte data = cvAread >> 8;  
      data &= B00001111; 
      EEPROM.write(100, data);
      data = cvAread;
      EEPROM.write(101, data);

      data = cvBread >> 8;  
      data &= B00001111; 
      EEPROM.write(102, data);
      data = cvBread;
      EEPROM.write(103, data);

      data = cvCread >> 8;  
      data &= B00001111; 
      EEPROM.write(104, data);
      data = cvCread;
      EEPROM.write(105, data);
    }
}
//END////////////////////////////////////