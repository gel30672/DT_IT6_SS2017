//
// Created by Andreas Zinkl on 08.06.17.
//

#ifndef DT2017_CARCOMMANDINTERPRETER_H
#define DT2017_CARCOMMANDINTERPRETER_H

#include <vector>
#include <queue>
#include "Command.h"

class CarCommandInterface {

public:

    // Constructor
    CarCommandInterface();
    ~CarCommandInterface();

    // Execution Methods
    short sendForwardDrive(double length, Position* destination);
    short sendBackwardDrive(double length, Position* destination);
    short sendTurnAroundDrive(double length, Position* destination, short direction);
    short sendTurnAroundAndBackDrive(double length, Position* destination, short direction);
    short sendStopCommand();

    // Get Methods
    bool isCurrentDriveFinished();

private:

    // Content
    Command* current;
};


#endif //DT2017_CARCOMMANDINTERPRETER_H
