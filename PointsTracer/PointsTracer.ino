#include "EZStepper.h"
#include <math.h>

//                                              //Constructor:
//                                              //    1.  Steps per revolution
//                                              //    2. to 5. Driver pins.
EZStepper stepperY(2094, 8, 9, 10 , 11);
EZStepper stepperX(2094, 4, 5, 6, 7);
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
int INT_X_SENSIVITY = 100;
int INT_Y_SENSIVITY = 100;

//variables de transformacion
String datos = "9,10,9,11,9,12,9,13,10,13,11,13,12,13,12,12,12,11,12,10,11,10,10,10,9,10";
int numOfPoints = 13;
int count = 0;
String digito = "";
//-----------------

void setup() 
{
  Serial.begin(9600);
  //                                            //Set the velocity to 64 half-steps per second.
  pinMode(intXButtonPin, INPUT);
  pinMode(intYButtonPin, INPUT);
  stepperX.subSetStatesPerSec(100);
  stepperY.subSetStatesPerSec(100);
  
  Point pointsArr[numOfPoints];
  getCoord(pointsArr);
  Serial.print ("volvi");
  stepperX.subSetDirection(stepperX.LEFT);
  stepperY.subSetDirection(stepperY.LEFT);
 /* while (!(digitalRead(intXButtonPin) == HIGH))
  {
    stepperX.subMoveBySteps(1);
  }

  while (!(digitalRead(intYButtonPin) == HIGH))
  {
    stepperY.subMoveBySteps(1);
  }*/

  subTracePoints(pointsArr);
}

void loop() 
{
  
}

void subTracePoints(Point arrpoint[])
{
  subMoveToFirstPoint(arrpoint[0]);

  for (int intI = 1; intI < 13; intI++)
  {
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

void subMoveToFirstPoint(Point pointFirst)
{
  stepperY.subSetDirection(stepperY.RIGHT);
  stepperX.subSetDirection(stepperX.RIGHT);
  stepperX.subMoveBySteps(pointFirst.intX * INT_X_SENSIVITY);
  stepperY.subMoveBySteps(pointFirst.intY * INT_Y_SENSIVITY);
  intCurrentX = pointFirst.intX;
  intCurrentY = pointFirst.intY;
}

//----------------------------------------------------------------------------------------------------
void subTraceToPoint(Point pointNext)
{
  int intMax;
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
  
  Serial.print(pointNext.intX);
  Serial.print("-");
  Serial.println(pointNext.intY);
  Serial.print(intNextX);
  Serial.print("-");
  Serial.println(intNextY);
  
  if (intNextX > intNextY)
  {
    intMax = intNextX * INT_X_SENSIVITY;
  }
  else
  {
    intMax = intNextY * INT_Y_SENSIVITY;
  }

  int intXLeft = intNextX * INT_X_SENSIVITY;
  int intYLeft = intNextY * INT_Y_SENSIVITY;

  for (int intI = 0; intI < intMax; intI++)
  {
    if (intXLeft > 0)
    {
      stepperX.subMoveBySteps(1);
      intXLeft -= 1;
    }

    if (intYLeft > 0)
    {
      stepperY.subMoveBySteps(1);
      intYLeft -= 1;
    }
  }

  intCurrentX = pointNext.intX;
  intCurrentY = pointNext.intY;
}

//obtengo coordenadas del string de datos

void getCoord(Point points[]){
    while (datos.length() > 0){
      while (isDigit(datos.charAt(0))){
        digito += datos.charAt(0);
        datos.remove(0,1);
      }
      points[count].intX = digito.toInt(); 
      datos.remove(0,1);
      digito = "";
    
      while (isDigit(datos.charAt(0))){
        digito += datos.charAt(0);
        datos.remove(0,1);
      }
      points[count].intY = digito.toInt(); 
      datos.remove(0,1);
      digito = "";
    count += 1;
  }
}

