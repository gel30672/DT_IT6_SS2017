//
// Created by Andreas Zinkl on 08.06.17.
//

#include "../include/CarCommandInterface.h"

extern "C" {
#include "libmdrv/SENSING/current_sensing.h"
}

// CONSTRUCTOR AND DECONSTRUCTOR

CarCommandInterface::CarCommandInterface() : emergencyStop(false) {}

CarCommandInterface::~CarCommandInterface() {}


// PUBLIC FUNCTIONS

bool CarCommandInterface::isCurrentDriveFinished() {

    bool result = false;

    if(emergencyStop) {
        MotorMoveRpm(0);
        return result;
    } else {
        current->execute();
    }

    if(current == nullptr) {
        result = true;

    } else if(current->getDistance() <= distanceSinceStart) {

        result = true;

        if(current->isCommandWithBackdrive() && current->getDirection() != DIRECTION_FORWARD) {
            sendBackwardDrive(current->getDistance(), current->getDestination());
            distanceSinceStart = 0;
            current->setBackDrive(false);
            result = false;
        }
    }

    return result;
}

void CarCommandInterface::activateEmergencyStop() {
    emergencyStop = true;
}

void CarCommandInterface::deactivateEmergencyStop() {
    emergencyStop = false;
}

short CarCommandInterface::sendForwardDrive(double length, Position *destination) {

    if(current != nullptr && current->getDirection() == DIRECTION_FORWARD && current->getDestination() == destination) {
        return SUCCESS;
    }

    current = new Command(length, DIRECTION_FORWARD, destination);
    current->execute();

    return SUCCESS;
}

short CarCommandInterface::sendBackwardDrive(double length, Position *destination, int side) {

    if(side == DIRECTION_LEFT) {
        current = new Command(length, DIRECTION_BWD_RIGHT, destination, MAXWHEELLOCK);
    } else if(side == DIRECTION_RIGHT){
        current = new Command(length, DIRECTION_BWD_LEFT, destination, MAXWHEELLOCK);
    } else {
        current = new Command(length, DIRECTION_BACKWARD, destination, MAXWHEELLOCK);
    }
    current->execute();

    return SUCCESS;
}

short CarCommandInterface::sendBackwardDrive(double length, Position *destination) {

    return sendBackwardDrive(length, destination, DIRECTION_BACKWARD);;
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