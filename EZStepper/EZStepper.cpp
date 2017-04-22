#include "Arduino.h"
#include "EZStepper.h"
#include <math.h>

//                                                          //AUTHOR: Hugo García
//                                                          //CO-AUTHOR: Efrén Bernabe
//                                                          //DATE: 3/19/2017
//                                                          //PURPOSE: Library for easy control of stepper motors
//                                                          //    (targeted and tested in four-bit drivers).

//======================================================================================================================
//----------------------------------------------------------------------------------------------------------------------
//                                                          //CONSTRUCTORS
EZStepper::EZStepper(
    //                                                      //Number of steps the motor takes to make a full revolution.
    int intStepsPerRev,
    //                                                      //The four pins connected to the driver.
    int intPinA,
    int intPinB,
    int intPinC,
    int intPinD)
{
    this->intPinA = intPinA;
    this->intPinB = intPinB;
    this->intPinC = intPinC;
    this->intPinD = intPinD;

    //                                                      //Initialize the pins as outputs.
    pinMode(this->intPinA, OUTPUT);
    pinMode(this->intPinB, OUTPUT);
    pinMode(this->intPinC, OUTPUT);
    pinMode(this->intPinD, OUTPUT);

    //                                                      //At this version only half-step mode is supported.
    this->intStepsPerRev = intStepsPerRev * 2;
}

//----------------------------------------------------------------------------------------------------------------------
//                                                          //METHODS
//                                                          //Used to move the motor until a signal is detected in order
//                                                          //      to get to the starting position (0°).
void EZStepper::subInitBySignal(int intSignalPin)
{
    //                                                      //Declare the signal pin as input.
    pinMode(intSignalPin, INPUT);

    //                                                      //Move in the current direction until a signal is received.
    while(digitalRead(intSignalPin) == LOW)
    {
        subMove();
    }
}

//----------------------------------------------------------------------------------------------------------------------
//                                                          //Move to the stated degrees (i.e. if current position is
//                                                          //      270°, this->direction = LEFT and numDestination =
//                                                          //      45, it will move 90° to the LEFT.
void EZStepper::subMoveByDegrees(double numDestination)
{
    //                                                      //Calculate how many steps it needs to move to reach the
    //                                                      //      desired position.
    int intLeftSteps;
    //                                                      //Destination degree is higher than actual and before 0°.
    if (this->numCurrentDegrees > numDestination)
    {
        //                                                  //Get how many steps are needed according to the direction.
        if (this->direction == RIGHT)
        {
            intLeftSteps = roundf((this->numCurrentDegrees - numDestination) / (360.0/(this->intStepsPerRev / 2)));
        }
        else
        {
            intLeftSteps = roundf((360 - this->numCurrentDegrees) / (360.0/(this->intStepsPerRev / 2)));
            intLeftSteps = intLeftSteps + roundf((numDestination) / (360.0/(this->intStepsPerRev / 2)));
        }
    }
    //                                                      //Destination degree is lower than actual and after 0°.
    else
    {
        if (this->direction == RIGHT)
        {
            intLeftSteps = roundf((this->numCurrentDegrees) / (360.0/(this->intStepsPerRev / 2)));
            intLeftSteps = intLeftSteps + roundf((360 - numDestination) / (360.0/(this->intStepsPerRev / 2)));
        }
        else
        {
            intLeftSteps = roundf((numDestination - this->numCurrentDegrees) / (360.0/(this->intStepsPerRev / 2)));
        }
    }

    //                                                      //Move the calculated steps.
    subMoveBySteps(intLeftSteps);
}

//----------------------------------------------------------------------------------------------------------------------
//                                                          //Move as many steps as the user entered.
void EZStepper::subMoveBySteps(int intSteps)
{
    //                                                      //2 times the steps, because only half-step is supported.
    //                                                      //      It must move one extra half-step because the current
    //                                                      //      state is the first cycle of the for.
    for (int intI = 0; intI <= intSteps * 2; intI++)
    {
        subMove();
    }

    //                                                      //Calculate the new current position.
    if (this->direction == RIGHT)
    {
        //                                                  //Divide by 2 because we have to convert from half-step to
        //                                                  //      step.
        this->numCurrentDegrees = this->numCurrentDegrees - ((360.0 / (this->intStepsPerRev / 2)) * intSteps);
    }
    else
    {
        this->numCurrentDegrees = this->numCurrentDegrees + ((360.0 / (this->intStepsPerRev / 2)) * intSteps);
    }

    //                                                      //If current position is negative, adjust it.
    if (this->numCurrentDegrees < 0)
    {
        while (this->numCurrentDegrees < 0)
        {
            this->numCurrentDegrees = this->numCurrentDegrees + 360;
        }
    }

    //                                                      //If position is above 360°, get the modulo i.e. real
    //                                                      //      position with respect to the origin.
    this->numCurrentDegrees = fmod(this->numCurrentDegrees, 360.0);
}

//----------------------------------------------------------------------------------------------------------------------
double EZStepper::numGetCurrentDegrees()
{
    return this->numCurrentDegrees;
}

//----------------------------------------------------------------------------------------------------------------------
//                                                          //Change the direction of movement (See enum Direction)
void EZStepper::subSetDirection(EZStepper::Direction direction)
{
    this->direction = direction;
}

//----------------------------------------------------------------------------------------------------------------------
//                                                          //Change the velocity (how many half-steps per second).
void EZStepper::subSetStatesPerSec(int intStatesPerSec)
{
    this->intStatesPerSec = intStatesPerSec;
}

//----------------------------------------------------------------------------------------------------------------------
int EZStepper::intGetStatesPerSec()
{
    return this->intStatesPerSec;
}

//----------------------------------------------------------------------------------------------------------------------
//                                                          //Moves from the current state to the next state (one half-
//                                                          //      step. See enum State.
void EZStepper::subMove()
{
    if (this->direction == RIGHT)
    {
        switch (this->state)
        {
            case _0001:
                this->state = _0011;
                digitalWrite(this->intPinA, HIGH);
                digitalWrite(this->intPinB, LOW);
                digitalWrite(this->intPinC, LOW);
                digitalWrite(this->intPinD, LOW);
                break;
            case _0011:
                this->state = _0010;
                digitalWrite(this->intPinA, HIGH);
                digitalWrite(this->intPinB, HIGH);
                digitalWrite(this->intPinC, LOW);
                digitalWrite(this->intPinD, LOW);
                break;
            case _0010:
                this->state = _0110;
                digitalWrite(this->intPinA, LOW);
                digitalWrite(this->intPinB, HIGH);
                digitalWrite(this->intPinC, LOW);
                digitalWrite(this->intPinD, LOW);
                break;
            case _0110:
                this->state = _0100;
                digitalWrite(this->intPinA, LOW);
                digitalWrite(this->intPinB, HIGH);
                digitalWrite(this->intPinC, HIGH);
                digitalWrite(this->intPinD, LOW);
                break;
            case _0100:
                this->state = _1100;
                digitalWrite(this->intPinA, LOW);
                digitalWrite(this->intPinB, LOW);
                digitalWrite(this->intPinC, HIGH);
                digitalWrite(this->intPinD, LOW);
                break;
            case _1100:
                this->state = _1000;
                digitalWrite(this->intPinA, LOW);
                digitalWrite(this->intPinB, LOW);
                digitalWrite(this->intPinC, HIGH);
                digitalWrite(this->intPinD, HIGH);
                break;
            case _1000:
                this->state = _1001;
                digitalWrite(this->intPinA, LOW);
                digitalWrite(this->intPinB, LOW);
                digitalWrite(this->intPinC, LOW);
                digitalWrite(this->intPinD, HIGH);
                break;
            case _1001:
                this->state = _0001;
                digitalWrite(this->intPinA, HIGH);
                digitalWrite(this->intPinB, LOW);
                digitalWrite(this->intPinC, LOW);
                digitalWrite(this->intPinD, HIGH);
                break;
            default:
                break;
        }

        //                                                  //Delay modified by the velocity.
        delay(1000.0/this->intStatesPerSec);
    }
    else
    {
        switch (this->state)
        {
            case _0001:
                this->state = _1001;
                digitalWrite(this->intPinA, HIGH);
                digitalWrite(this->intPinB, LOW);
                digitalWrite(this->intPinC, LOW);
                digitalWrite(this->intPinD, LOW);
                break;
            case _0011:
                this->state = _0001;
                digitalWrite(this->intPinA, HIGH);
                digitalWrite(this->intPinB, HIGH);
                digitalWrite(this->intPinC, LOW);
                digitalWrite(this->intPinD, LOW);
                break;
            case _0010:
                this->state = _0011;
                digitalWrite(this->intPinA, LOW);
                digitalWrite(this->intPinB, HIGH);
                digitalWrite(this->intPinC, LOW);
                digitalWrite(this->intPinD, LOW);
                break;
            case _0110:
                this->state = _0010;
                digitalWrite(this->intPinA, LOW);
                digitalWrite(this->intPinB, HIGH);
                digitalWrite(this->intPinC, HIGH);
                digitalWrite(this->intPinD, LOW);
                break;
            case _0100:
                this->state = _0110;
                digitalWrite(this->intPinA, LOW);
                digitalWrite(this->intPinB, LOW);
                digitalWrite(this->intPinC, HIGH);
                digitalWrite(this->intPinD, LOW);
                break;
            case _1100:
                this->state = _0100;
                digitalWrite(this->intPinA, LOW);
                digitalWrite(this->intPinB, LOW);
                digitalWrite(this->intPinC, HIGH);
                digitalWrite(this->intPinD, HIGH);
                break;
            case _1000:
                this->state = _1100;
                digitalWrite(this->intPinA, LOW);
                digitalWrite(this->intPinB, LOW);
                digitalWrite(this->intPinC, LOW);
                digitalWrite(this->intPinD, HIGH);
                break;
            case _1001:
                this->state = _1000;
                digitalWrite(this->intPinA, HIGH);
                digitalWrite(this->intPinB, LOW);
                digitalWrite(this->intPinC, LOW);
                digitalWrite(this->intPinD, HIGH);
                break;
            default:
                break;
        }

        delay(1000.0/this->intStatesPerSec);
    }
}

//======================================================================================================================
