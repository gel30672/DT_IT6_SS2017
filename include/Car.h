//
// Created by Andreas Zinkl on 08.06.17.
//

#ifndef DT2017_CAR_H
#define DT2017_CAR_H

#include "LocDet.h"
#include "Vector.h"
#include "ErrorHandling.h"
#include "CarState.h"
#include "Command.h"
#include "CarCommandInterface.h"

extern "C" {
#include "../src/libmdrv/SENSING/current_sensing.h"
}

class Car {

public:

    // Constructor
    Car();
    ~Car();

    // Run-Methods
    void *driveTo(Position* destination);

    // Get-Methods
    Position* getLastKnownPosition();
    Position* getCurrentPosition();
    Vector* getCurrentDirection();
    double getDrovenDistance();
    short getCurrentCarState();

private:

    // Content
    Position _currentPosition;
    Position _lastKnownPosition;
    Position* _currentDestination;
    Vector* _currentDirection;
    short _carState;

    // Other Modules
    LocDet* _localization;
    CarCommandInterface* _cmdInterface;

    // internal functionality
    short init();
    short initDrive();
    short updateCurrentPosition();
    short updateCarState(short state);
    short resetDrovenDistance();
    bool didReachDestination();
};


#endif //DT2017_CAR_H
