//
// Created by Andreas Zinkl on 08.06.17.
//

#include "../include/CarCommandInterface.h"

extern "C" {
#include "libmdrv/SENSING/current_sensing.h"
}

// CONSTRUCTOR AND DECONSTRUCTOR

CarCommandInterface::CarCommandInterface() {}

CarCommandInterface::~CarCommandInterface() {}


// PUBLIC FUNCTIONS

bool CarCommandInterface::isCurrentDriveFinished() {

    bool result = false;

    if(current == nullptr) {
        result = true;

    } else {

        if(current->getDirection() == DIRECTION_FORWARD) {
            result = true;

        } else if(distanceSinceStart >= current->getDistance()) {

            result = true;

            if(current->isCommandWithBackdrive()) {
                sendBackwardDrive(CIRCLERADIUS, current->getDestination());
                result = false;
            }
        }
    }

    return result;
}

short CarCommandInterface::sendForwardDrive(double length, Position *destination) {

    if(current != nullptr && current->getDirection() == DIRECTION_FORWARD && current->getDestination() == destination) {
        return SUCCESS;
    }

    current = new Command(length, DIRECTION_FORWARD, destination);
    current->execute();

    return SUCCESS;
}

short CarCommandInterface::sendBackwardDrive(double length, Position *destination) {

    current = new Command(length, DIRECTION_BACKWARD, destination);
    current->execute();

    return SUCCESS;
}

short CarCommandInterface::sendTurnAroundDrive(double length, Position *destination, short direction) {

    current = new Command(length, direction, destination, MAXWHEELLOCK);
    current->execute();

    return SUCCESS;
}

short CarCommandInterface::sendTurnAroundAndBackDrive(double length, Position *destination, short direction) {

    current = new Command(length, direction, destination, MAXWHEELLOCK, true);
    current->execute();

    return SUCCESS;
}

short CarCommandInterface::sendStopCommand() {

    current = new Command(0, DIRECTION_STOP, nullptr);
    current->execute();

    return SUCCESS;
}