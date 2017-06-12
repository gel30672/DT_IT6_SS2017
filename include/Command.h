//
// Created by Andreas Zinkl on 12.05.17.
//

#ifndef DT2017_COMMAND_H
#define DT2017_COMMAND_H

#include "Vector.h"
#include "DeviceConfiguration.h"
#include <math.h>
#include <iostream>
#include "ErrorHandling.h"


extern "C" {
#include "../src/libmdrv/MOTOR/motor.h"
#include "../src/libmdrv/STEERING/steering.h"
}
class Command {

private:
    Position* _destination;
    double _distance;
    short _direction;
    bool _active;
    short _steeringAngle;
    bool _simpleBackDrive;
    bool _finished;

public:
    Command(double distance, short dir, Position *destination);
    Command(double distance, short dir, Position *destination, short steeringAngle);
    Command(double distance, short dir, Position *destination, short steeringAngle, bool withSimpleBackDrive);
    ~Command();

    // Set Methods
    short deactivate();
    void execute();
    short markAsFinished();
    void setBackDrive(bool backDrive);

    // Get Methods
    bool isActive();
    bool isFinished();
    double getDistance();
    short getDirection();
    Position* getDestination();
    bool isCommandWithBackdrive();
};

#endif //DT2017_COMMAND_H
