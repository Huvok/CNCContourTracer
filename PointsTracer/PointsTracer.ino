#include "EZStepper.h"
#include <math.h>
#include <Servo.h>

//                                              //Constructor:
//                                              //    1. Steps per revolution
//                                              //    2. to 5. Driver pins.
EZStepper stepperY(2094, 8, 9, 10 , 11);
EZStepper stepperX(2094, 4, 5, 6, 7);
Servo servo;
int intYButtonPin = 2;
int intXButtonPin = 3;
int intCurrentX = 0;
int intCurrentY = 0;

struct point
{
   int intX;
   int intY;
};

typedef struct point Point;

//===================================================================================================================
//                                                      //GLOBAL VARIABLES
int INT_X_SENSIVITY = 100;
int INT_Y_SENSIVITY = 100;
//                                                      //Square
String datos = "9,10,9,11,9,12,9,13,10,13,11,13,12,13,12,12,12,11,12,10,11,10,10,10,9,10";
int intPointsNumber = 13;
//                                                      //Triangle
//String datos = "9,10,12,15,15,10,9,10";
// intPointsNumber 4;

int count = 0;
String digito = "";

//===================================================================================================================
//                                                      //VOID SETUP
void setup() 
{
  Serial.begin(9600);
  servo.attach(12);
  servo.write(21);
  delay(1000);
  //                                                    //Set the velocity to 100 half-steps per second.
  pinMode(intXButtonPin, INPUT);
  pinMode(intYButtonPin, INPUT);

  stepperX.subSetStatesPerSec(100);
  stepperY.subSetStatesPerSec(100);
  
  Point pointsArr[intPointsNumber];
  getCoord(pointsArr);
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
}

//-----------------------------------------------------------------------------------------------------------------
void subMoveToFirstPoint(Point pointFirst)
{
 
  stepperY.subSetDirection(stepperY.RIGHT);
  stepperX.subSetDirection(stepperX.RIGHT);
  stepperX.subMoveBySteps(pointFirst.intX * INT_X_SENSIVITY);
  stepperY.subMoveBySteps(pointFirst.intY * INT_Y_SENSIVITY);
  intCurrentX = pointFirst.intX;
  intCurrentY = pointFirst.intY;
   servo.write(19);
}

//----------------------------------------------------------------------------------------------------
void subTraceToPoint(Point pointNext)
{
  int intNextX = pointNext.intX - intCurrentX;
  int intNextY = pointNext.intY - intCurrentY;
  if (intNextX < 0)
  {
    intNextX *= -1;
  }

  if (intNextY < 0)
  {
    intNextY *= -1;
  }

  int intXLeft = intNextX * INT_X_SENSIVITY;
  int intYLeft = intNextY * INT_Y_SENSIVITY;
  int intDiagonalRatio = 1;
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
