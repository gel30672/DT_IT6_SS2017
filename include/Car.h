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
#include "LaserSensor.h"

extern "C" {
#include "../src/libmdrv/SENSING/current_sensing.h"
}

class Car {

public:

    // Constructor
    Car(int* needEmergencyStop);
    ~Car();
    Position _currentPosition;

    // Run-Methods
    void *driveTo(Position* destination);
    short go2(Position* dest);
    void tellCarToStop();

    // Set-Methods
    void setLaserSensor(LaserSensor* laser);

    // Get-Methods
    Position* getLastKnownPosition();
    Position* getCurrentPosition();
    Vector* getCurrentDirection();
    bool didReachDestination();
    double getDrovenDistance();
    short getCurrentCarState();

private:

    // Content
    Position _lastKnownPosition;
    Position* _currentDestination;
    Vector* _currentDirection;
    LaserSensor* _laser;
    short _carState;
    int* _emergency;
    bool _finishedDrive;

    // Other Modules
    LocDet* _localization;
    CarCommandInterface* _cmdInterface;

    // internal functionality
    short init();
    short initDrive();
    short updateCurrentPosition();
    short updateCarState(short state);
    short resetDrovenDistance();
    int straightDrive(float angle, short side, Vector* destVector);
    int orientationTurn(short side, Vector* destVector);
};


#endif //DT2017_CAR_H
