#ifndef EZStepper_h
#define EZStepper_h
#include "Arduino.h"
#include <math.h>

//                                                          //AUTHOR: Hugo García
//                                                          //CO-AUTHOR: Efrén Bernabe
//                                                          //DATE: 3/19/2017
//                                                          //PURPOSE: Library for easy control of stepper motors
//                                                          //    (targeted and tested in four-bit drivers).

//======================================================================================================================
class EZStepper
{
    public:
        //==================================================================================================================
        enum State
        {
            _0001,
            _0011,
            _0010,
            _0110,
            _0100,
            _1100,
            _1000,
            _1001,
        };
        //==============================================================================================================

        //==============================================================================================================
        enum Direction
        {
            RIGHT,
            LEFT
        };
        //==============================================================================================================

    //------------------------------------------------------------------------------------------------------------------
    //                                                      //PROPERTIES
    private:
        int intStepsPerRev;
        int intPinA;
        int intPinB;
        int intPinC;
        int intPinD;
        double numCurrentDegrees = 0;
        State state = _0001;
        Direction direction = RIGHT;
        int intStatesPerSec = 8;

    //------------------------------------------------------------------------------------------------------------------
    //                                                      //CONSTRUCTORS
    public:
        EZStepper(int intStepsPerRev,
                  int intPinA,
                  int intPinB,
                  int intPinC,
                  int intPinD);

    //------------------------------------------------------------------------------------------------------------------
    //                                                      //METHODS
    public:
        void subInitBySignal(int intSignalPin);
        void subMoveByDegrees(double numDestination);
        void subMoveBySteps(int intSteps);
        double numGetCurrentDegrees();
        void subSetDirection(Direction direction);
        void subSetStatesPerSec(int intStatesPerSec);
        int intGetStatesPerSec();

    private:
        void subMove();
};
//======================================================================================================================

#endif
