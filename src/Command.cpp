//
// Created by Andreas Zinkl on 12.05.17.
//

#include <unistd.h>
#include "../include/Command.h"

double distanceSinceStart = 0.0;

Command::Command(double distance, short dir, Position *destination)
        : _distance(distance), _direction(dir), _destination(destination), _active(false), _steeringAngle(0), _simpleBackDrive(false) {

    /*if(PRINT_ERROR_CODE) {
        std::cout << std::endl;
        std::cout << "## NEW COMMAND CREATED ##" << std::endl;
        std::cout << "## Direction(" << _direction << ") ## Distance(" << _distance << ") ## " << std::endl;
        if(start != nullptr && destination != nullptr) std::cout << "## Start(" << start->x << "|" << start->y << ") ## End(" << destination->x << "|" << destination->y << ") ##" << std::endl;
        std::cout << std::endl;
    }*/
}

Command::Command(double distance,short dir, Position *destination, short steeringAngle)
        : _distance(distance), _direction(dir), _destination(destination), _active(false), _steeringAngle(steeringAngle), _simpleBackDrive(false) {

}

Command::Command(double distance, short dir, Position *destination, short steeringAngle, bool withSimpleBackDrive)
        : _distance(distance), _direction(dir), _destination(destination), _active(false), _steeringAngle(steeringAngle), _simpleBackDrive(withSimpleBackDrive) {

}

Command::~Command() {}

void Command::execute() {

    // Need to call steering for the wheelangle
    switch (_direction) {

        case DIRECTION_LEFT:
            SteerDegrees(LEFT_WHEEL_ANGLE);
            break;
        case DIRECTION_BWD_LEFT:
            SteerDegrees(RIGHT_WHEEL_ANGLE);
            break;
        case DIRECTION_BWD_RIGHT:
            SteerDegrees(LEFT_WHEEL_ANGLE);
            break;
        case DIRECTION_RIGHT:
            SteerDegrees(RIGHT_WHEEL_ANGLE);
            break;

        default:
            SteerDegrees(STRAIGHT_WHEEL_ANGLE);
    }

    // Need to call the motor to start driving
    if(_direction == DIRECTION_FORWARD
       ||_direction == DIRECTION_LEFT
       || _direction == DIRECTION_RIGHT) {
        MotorMoveRpm(FULLSPEED);
    } else if(_direction == DIRECTION_BACKWARD
              || _direction == DIRECTION_BWD_RIGHT
              || _direction == DIRECTION_BWD_LEFT) {
        MotorMoveRpm((-1)*FULLSPEED);
    } else {
        MotorMoveRpm(0);
    }

    // We need to say, this top command is active
    _active = true;
}

short Command::getDirection() {
    return _direction;
}

bool Command::isActive() {
    return _active;
}

double Command::getDistance() {
    return _distance;
}

bool Command::isCommandWithBackdrive() {
    return _simpleBackDrive;
}

bool Command::isFinished() {
    return distanceSinceStart >= _distance;
}

void Command::setBackDrive(bool backDrive) {
    _simpleBackDrive = backDrive;
}

Position* Command::getDestination() {
    return _destination;
}

short Command::markAsFinished() {
    _finished = true;
    return SUCCESS;
}

short Command::deactivate() {
    _active = false;
}