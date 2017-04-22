#include <EZStepper.h>
#include <Math.h>

//                                              //Constructor:
//                                              //    1.  Steps per revolution
//                                              //    2. to 5. Driver pins.
EZStepper stepperX(2094, 8, 9, 10 , 11);
EZStepper stepperY(2094, 8, 9, 10 , 11);

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
  //                                            //Set the velocity to 64 half-steps per second.
  stepperX.subSetStatesPerSec(50);
  stepperY.subSetStatesPerSec(50);
}

void loop() 
{
  Point point1, point2, point3, point4;
  point1.intX = 2;
  point1.intY = 5;
  point2.intX = 2;
  point2.intY = 3;
  point3.intX = 4;
  point3.intY = 3;
  point4.intX = 4;
  point4.intY = 5;
  Point arrpoint[] = { point1, point2, point3, point4 };

  subTracePoints(arrpoint);
  /*  stepper.subSetDirection(stepper.LEFT);
  //                                            //Move to 45° with respect of the origin i.e. from 270° (previous position) to 45°, or 90° to the left.
  stepper.subMoveBySteps(10000);*/

  /*stepper.subSetDirection(stepper.RIGHT);
  //                                            //Move to 45° with respect of the origin i.e. from 270° (previous position) to 45°, or 90° to the left.
  stepper.subMoveBySteps(2000);*/
}

void subTracePoints(Point arrpoint[])
{
  subMoveToFirstPoint(arrpoint[0]);
  
  for (int intI = 1; intI < (sizeof(arrpoint)/sizeof(arrpoint[0])); intI++)
  {
    subTraceToPoint(arrpoint[intI]);
  }
}

void subMoveToFirstPoint(Point pointFirst)
{
  stepperX.subMoveBySteps(pointFirst.intX * INT_X_SENSIVITY);
  stepperY.subMoveBySteps(pointFirst.intY * INT_Y_SENSIVITY);
}

void subTraceToPoint(Point pointNext)
{
  int intMax;
  if (pointNext.intX > pointNext.intY)
  {
    intMax = pointNext.intX;
  }
  else
  {
    intMax = pointNext.intY;
  }

  int intXLeft = pointNext.intX * INT_X_SENSIVITY;
  int intYLeft = pointNext.intY * INT_Y_SENSIVITY;
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
}

