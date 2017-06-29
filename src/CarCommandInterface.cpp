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

    distanceSinceStart = 0;
    if(current != nullptr && current->getDirection() == DIRECTION_FORWARD && current->getDestination() == destination) {
        return SUCCESS;
    }

    current = new Command(length, DIRECTION_FORWARD, destination);
    current->execute();

    return SUCCESS;
}

short CarCommandInterface::sendBackwardDrive(double length, Position *destination, short side) {

    distanceSinceStart = 0;
    if(side == DIRECTION_RIGHT) {
        current = new Command(length, DIRECTION_BWD_RIGHT, destination, MAXWHEELLOCK);
    } else if(side == DIRECTION_LEFT){
        current = new Command(length, DIRECTION_BWD_LEFT, destination, MAXWHEELLOCK);
    } else {
        current = new Command(length, DIRECTION_BACKWARD, destination, MAXWHEELLOCK);
    }
    current->execute();

    return SUCCESS;
}

short CarCommandInterface::sendBackwardDrive(double length, Position *destination) {

    return sendBackwardDrive(length, destination, DIRECTION_BACKWARD);
}

short CarCommandInterface::sendTurnAroundDrive(double length, Position *destination, short direction) {

    distanceSinceStart = 0;
    current = new Command(length, direction, destination, MAXWHEELLOCK);
    current->execute();

    return SUCCESS;
}

short CarCommandInterface::sendTurnAroundAndBackDrive(double length, Position *destination, short direction) {

    distanceSinceStart = 0;
    current = new Command(length, direction, destination, MAXWHEELLOCK, true);
    current->execute();

    while(!isCurrentDriveFinished()) {
        std::cout << "try to turn around fwd..." << distanceSinceStart << std::endl;
    }
    std::cout << "finished" << std::endl;

    sendStopCommand();
    distanceSinceStart = 0;
    if(DIRECTION_LEFT == direction) sendBackwardDrive(length, destination, DIRECTION_BWD_RIGHT);
    if(DIRECTION_RIGHT == direction) sendBackwardDrive(length, destination, DIRECTION_BWD_LEFT);

    while(!isCurrentDriveFinished()) {
        std::cout << "try to turn around back..." << distanceSinceStart << std::endl;
    }

    return SUCCESS;
}

short CarCommandInterface::sendStopCommand() {

    distanceSinceStart = 0;
    current = new Command(0, DIRECTION_STOP, nullptr);
    current->execute();

    return SUCCESS;
}

