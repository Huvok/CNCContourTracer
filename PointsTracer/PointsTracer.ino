#include "EZStepper.h"
#include <math.h>
#include <Servo.h>
#include <LiquidCrystal.h>

//                                              //AUTHORS: Hugo García, Efrén Pérez
//                                              //DATE: 4/20/2017
//                                              //PURPOSE: Arduino support for CNC contour tracer.

//===========================================================================================================
//                                              //EZStepper constructor:
//                                              //    1. Steps per revolution
//                                              //    2. to 5. Driver pins.
EZStepper stepperY(2094, 8, 9, 10 , 11);
EZStepper stepperX(2094, 22, 24, 26, 28);
Servo servo;
int intYButtonPin = 39;
int intXButtonPin = 41;
int intCurrentX = 0;
int intCurrentY = 0;

//                                              //LCD Display
LiquidCrystal lcd(43, 45, 47, 49, 51, 53); 
//RS=43  E=45  D4=47 D5=49 D6=51 D7=53
byte heart[8] = {
  0b00000,
  0b01010,
  0b11111,
  0b11111,
  0b11111,
  0b01110,
  0b00100,
  0b00000
};

byte smiley[8] = {
  0b00000,
  0b01010,
  0b01010,
  0b01010,
  0b00000,
  0b10001,
  0b01110,
  0b00000
};

//                                                      //Points in X, Y coordinates
struct point
{
   int intX;
   int intY;
};

typedef struct point Point;

//===================================================================================================================
//                                                      //GLOBAL VARIABLES

//                                                      //The biggest these two values are, the more steps a
//                                                      //    coordinate point unit is worth.
int INT_X_SENSIVITY = 100;
int INT_Y_SENSIVITY = 100;
//                                                      //Square
String datos = "9,10,9,11,9,12,9,13,10,13,11,13,12,13,12,12,12,11,12,10,11,10,10,10,9,10";
int intPointsNumber = 13;
//                                                      //Triangle
//String datos = "15,14,13,12,11,10,12,10,13,10,15,10,16,10,17,10,19,10,17,12,16,13,15,14";
//int intPointsNumber = 12;
//                                                      //GetFromSerial
//String datos;
//int intPointsNumber;
bool start = false;
//

int count = 0;
String digito = "";

//===================================================================================================================
//                                                      //VOID SETUP
void setup() 
{
  Serial.begin(9600);

//                                                      //LCD start
  lcd.begin(16, 2);

  lcd.createChar(0, heart);
  lcd.createChar(1, smiley);  
  lcd.setCursor(0,0);
  
  lcd.print("My Guedea ");  
  lcd.setCursor(0,1); 
  lcd.write(byte(0));
  delay(1000);
  
  servo.attach(2);
  servo.write(8);
  
  pinMode(intXButtonPin, INPUT);
  pinMode(intYButtonPin, INPUT);

  //                                                    //Set the velocity to 90 half-steps per second.
  stepperX.subSetStatesPerSec(90);
  stepperY.subSetStatesPerSec(90);
  
  //GetData();
  //Serial.println(datos);  
  //Serial.println(intPointsNumber); 
  Point pointsArr[intPointsNumber];
  getCoord(pointsArr);

  //                                                    //Move until the buttons are pressed, so we can decide
  //                                                    //    where is the (0, 0) coordinate.
  stepperX.subSetDirection(stepperX.LEFT);
  stepperY.subSetDirection(stepperY.LEFT);
  while (!(digitalRead(intXButtonPin) == HIGH))
  {
    stepperX.subMoveBySteps(1);
  }

  while (!(digitalRead(intYButtonPin) == HIGH))
  {
    stepperY.subMoveBySteps(1);
  }
  
  lcd.clear();
  lcd.print("ESTOY LISTO");
  lcd.setCursor(0,1);
  lcd.write(byte(0));
  delay(1000); 
  subTracePoints(pointsArr);
}

//-----------------------------------------------------------------------------------------------------------------
//                                                      //VOID SETUP METHODS

//                                                      //Obtain the coordinates of the data String.
void getCoord(Point points[])
{
  while (datos.length() > 0)
  {
    while (isDigit(datos.charAt(0)))
    {
      digito += datos.charAt(0);
      datos.remove(0,1);
    }
    
    points[count].intX = digito.toInt(); 
    datos.remove(0,1);
    digito = "";
  
    while (isDigit(datos.charAt(0)))
    {
      digito += datos.charAt(0);
      datos.remove(0,1);
    }
    
    points[count].intY = digito.toInt(); 
    datos.remove(0,1);
    digito = "";
    count += 1;
  }
}

//-----------------------------------------------------------------------------------------------------------------
void subTracePoints(Point arrpoint[])
{
  //                                                      //We must move to the first point before droping the
  //                                                      //    marker.
  subMoveToFirstPoint(arrpoint[0]);

  //                                                      //Trace all the coordinates.
  for (int intI = 1; intI < intPointsNumber; intI++)
  {

    //                                                    //Must see which one is the next point's motor direction.
    if (arrpoint[intI].intX > arrpoint[intI - 1].intX)
    {
      stepperX.subSetDirection(stepperX.RIGHT);
    }
    else
    {
      stepperX.subSetDirection(stepperX.LEFT);
    }

    if (arrpoint[intI].intY > arrpoint[intI - 1].intY)
    {
      stepperY.subSetDirection(stepperY.RIGHT);
    }
    else
    {
      stepperY.subSetDirection(stepperY.LEFT);
    }
    
    subTraceToPoint(arrpoint[intI]);
  }
  servo.write(8);
}

//-----------------------------------------------------------------------------------------------------------------
void subMoveToFirstPoint(Point pointFirst)
{
  //                                                  //The first point will be always be at (0,0) or to the
  //                                                  //    right direction of each stepper motor.
  stepperY.subSetDirection(stepperY.RIGHT);
  stepperX.subSetDirection(stepperX.RIGHT);
  stepperX.subMoveBySteps(pointFirst.intX * INT_X_SENSIVITY);
  stepperY.subMoveBySteps(pointFirst.intY * INT_Y_SENSIVITY);
  intCurrentX = pointFirst.intX;
  intCurrentY = pointFirst.intY;

  //                                                  //Get the marker down.
  servo.write(3);
}

//------------------------------------------------------------------------------------------------------------------
void subTraceToPoint(Point pointNext)
{
  //                                                  //Get the difference from the current point to the next one.
  int intNextX = pointNext.intX - intCurrentX;
  int intNextY = pointNext.intY - intCurrentY;

  //                                                  //Correction for negative differences.
  if (intNextX < 0)
  {
    intNextX *= -1;
  }

  if (intNextY < 0)
  {
    intNextY *= -1;
  }

  //                                                  //The "sensivity" is the ratio of how many steps a coordinate
  //                                                  //    unit is worth.
  int intXLeft = intNextX * INT_X_SENSIVITY;
  int intYLeft = intNextY * INT_Y_SENSIVITY;
  int intDiagonalRatio = 1;

  //                                                  //If it is a diagonal line, we must get a ratio to get it right.
  if (intXLeft > 0 && 
      intYLeft > 0)
  {
    if (intXLeft > intYLeft)
    {
      intDiagonalRatio = intXLeft / intYLeft;
    }
    else
    {
      intDiagonalRatio = intYLeft / intXLeft;
    }
  }

  //                                                  //Move until there are no steps left. The main if is to get if the
  //                                                  //    diagonal ratio goes in favor of the X or the Y axis.
  if (intXLeft > intYLeft)
  {
    while (intXLeft > 0 ||
         intYLeft > 0)
    {
      if (intXLeft > 0)
      {
        stepperX.subMoveBySteps(intDiagonalRatio);
        intXLeft -= 1;
      }
  
      if (intYLeft > 0)
      {
        stepperY.subMoveBySteps(1);
        intYLeft -= 1;
      }
    }
  }
  else
  {
    while (intXLeft > 0 ||
         intYLeft > 0)
    {
      if (intXLeft > 0)
      {
        stepperX.subMoveBySteps(1);
        intXLeft -= 1;
      }
  
      if (intYLeft > 0)
      {
        stepperY.subMoveBySteps(intDiagonalRatio);
        intYLeft -= 1;
      }
    }
  }

  intCurrentX = pointNext.intX;
  intCurrentY = pointNext.intY;
}

//==================================================================================================
//                                              //VOID LOOP
void loop() 
{
  
}

//==================================================================================================
//                                              //Get datos
//formato datos//
//  &tam,x1,y1,x2,y2,.......,xn,yn&"
void GetData(){
  bool flag = true;
  bool start = false;
  GetTam();
  while (flag){
    if (Serial.available()) { 
      char c = Serial.read();
      datos += c;
        if (c == '&') {
          flag = false;
          datos.remove(datos.length()-1);
        }
      }
  }
}
//==================================================================================================

void GetTam(){
  bool flag = true;
  String tam;
  while (flag){
    if (Serial.available()) { 
      char c = Serial.read();
      if (start) {
        tam += c;
        if (c == ',') {
          flag = false;
          datos.remove(datos.length()-1);
        }
      }
      if (c == '&') {start = true;}
    }
  }
  intPointsNumber = tam.toInt();
}
//==================================================================================================
