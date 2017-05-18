//
// Created by Andreas Zinkl on 07.05.17.
//

#ifndef DT2017_DRIVECALCULATION_H
#define DT2017_DRIVECALCULATION_H

#include "DeviceConfiguration.h"
#include <math.h>
#include "Vector.h"
#include "Command.h"
#include "CStack.h"

#define DIRECTION_CHANGE_NEEDED true
#define DIRECTION_CHANGE_NOT_NEEDED false
#define FORWARD_DIRECTION -1

class DriveCalculation {

private:
    Position current;
    Position lastPositionKnown;
    Position circleCore;
    Position destination;

    CStack<Command> drivingCommands;

    void initCalculation();
    void updateCurrentPosition(float x, float y);
    short checkCurrentDirection();
    short checkDestinationDirection();
    void changeTo(short direction);

public:
    DriveCalculation(CStack<Command> commandStack);
    ~DriveCalculation();

    int calculate(Position* start, Position* end);
};


#endif //ALGORITHMS_CPP_DRIVECALCULATION_H
