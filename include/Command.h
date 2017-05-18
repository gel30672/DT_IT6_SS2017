//
// Created by Andreas Zinkl on 12.05.17.
//

#ifndef DT2017_COMMAND_H
#define DT2017_COMMAND_H

//todo these configurations need to go to the device-config-header!!!
#include "Vector.h"

#define FULLSPEED 20
#define BACKSPEED (-FULLSPEED)
#define WHEEL_ANGLE(x) (x == 0 ? -20 : 20) //todo this needs to be the max wheel angle from the device config header

class Command {

private:
    short _fullDistance;
    short _partDistance;
    Position* _destination;
    short _direction;

public:
    Command(short fullDistance, short partDistance, Position* destination, short direction);
    ~Command();

    void execute();
};

#endif //DT2017_COMMAND_H
