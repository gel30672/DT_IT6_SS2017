//
// Created by Andreas Zinkl on 12.05.17.
//

#ifndef DT2017_COMMAND_H
#define DT2017_COMMAND_H

#include "Vector.h"
#include "DeviceConfiguration.h"
#include <math.h>

extern "C"{
#include "../src/libmdrv/MOTOR/motor.h"
#include "../src/libmdrv/STEERING/steering.h"
}
class Command {

private:
    double _distance;
    Position* _start;
    Position* _destination;
    short _direction;
    bool _active;

public:
    Command(double distance, Position* start, Position* destination, short dir);
    ~Command();

    void execute();
    bool isActive();
    Position* getStartPosition();
    Position* getDestinationPosition();
    Position* getPredictedPositionBy(double distance);
};

#endif //DT2017_COMMAND_H
