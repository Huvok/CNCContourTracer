#include <EZStepper.h>
#include <Math.h>

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

void setup() 
{
  Serial.begin(9600);
  //                                            //Set the velocity to 64 half-steps per second.
  pinMode(intXButtonPin, INPUT);
  pinMode(intYButtonPin, INPUT);
  stepperX.subSetStatesPerSec(100);
  stepperY.subSetStatesPerSec(100);
  Point point1, point2, point3, point4, point5, point6, point7, point8, point9, point10, point11, point12, point13;
  point1.intX = 9;
  point1.intY = 10;
  point2.intX = 9;
  point2.intY = 11;
  point3.intX = 9;
  point3.intY = 12;
  point4.intX = 9;
  point4.intY = 13;
  point5.intX = 10;
  point5.intY = 13;
  point6.intX = 11;
  point6.intY = 13;
  point7.intX = 12;
  point7.intY = 13;
  point8.intX = 12;
  point8.intY = 12;
  point9.intX = 12;
  point9.intY = 11;
  point10.intX = 12;
  point10.intY = 10;
  point11.intX = 11;
  point11.intY = 10;
  point12.intX = 10;
  point12.intY = 10;
  point13.intX = 9;
  point13.intY = 10;

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

  Point arrpoint[] = { point1, point2, point3, point4, point5, point6, point7, point8, point9, point10, point11, point12, point13 };
  subTracePoints(arrpoint);
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

