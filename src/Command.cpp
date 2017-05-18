//
// Created by Andreas Zinkl on 12.05.17.
//

#include "../include/Command.h"

Command::Command(short fullDistance, short partDistance, Position* destination, short direction)
        : _fullDistance(fullDistance), _partDistance(partDistance), _destination(destination), _direction(direction) {}

Command::~Command() {
    // delete the positions if necessary
    delete _destination;
}

void Command::execute() {

    // We need to implement here the command which then tells the car what it needs to do
}