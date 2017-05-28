//
// Created by Andreas Zinkl on 07.05.17.
//

#ifndef DT2017_DRIVECALCULATION_H
#define DT2017_DRIVECALCULATION_H

#include "DeviceConfiguration.h"
#include <math.h>
#include "Vector.h"
#include "Command.h"
#include <stack>

#define DIRECTION_CHANGE_NEEDED true
#define DIRECTION_CHANGE_NOT_NEEDED false

class DriveCalculation {

private:
    Position current;
    Position lastPositionKnown;
    Position circleCore;
    Position destination;
    Vector* initVector;

    std::stack<Command> drivingCommands;

    void initCalculation();
    void calculateTurningPoint(Vector *currVec, short direction);

    short checkDestinationDirection();

public:
    DriveCalculation(Position* initStart, Position* initEnd);
    ~DriveCalculation();

    short calculate(Position* start, Position* end);

    Command getNextCommand();
};


#endif //ALGORITHMS_CPP_DRIVECALCULATION_H
