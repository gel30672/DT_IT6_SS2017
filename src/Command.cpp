//
// Created by Andreas Zinkl on 12.05.17.
//

#include "../include/Command.h"

Command::Command(double distance, Position* start, Position* destination, short dir)
        : _distance(distance), _start(start), _destination(destination), _direction(dir), _active(false) {}

Command::~Command() {
    // delete the positions if necessary
    //delete _destination;
}

void Command::execute() {

    // Need to call steering for the wheelangle
    switch (_direction) {

        case DIRECTION_LEFT:
            SteerDegrees(WHEEL_ANGLE(_direction));
            break;
        case DIRECTION_RIGHT:
            SteerDegrees(WHEEL_ANGLE(_direction));
            break;

        default:
            SteerDegrees(0);
    }

    // Need to call the motor to start driving
    if(_direction == DIRECTION_STOP) {
        MotorMoveRpm(0);
    } else {
        MotorMoveRpm(FULLSPEED);
    }

    // We need to say, this top command is active
    _active = true;
}

bool Command::isActive() {
    return _active;
}

Position* Command::getStartPosition() {
    return _start;
}

Position* Command::getDestinationPosition() {
    return _destination;
}

Position* Command::getPredictedPositionBy(double distance) {

    //Define the result value with error coordinates
    Position* predictedPos = new struct Position;
    predictedPos->x = -1;
    predictedPos->y = -1;

    //check if we need to calculate the length of a straight vector
    if(_direction == DIRECTION_FORWARD || _direction == DIRECTION_BACKWARD) {

        // Get the current distance
        double percentage = ((100/_distance)*distance)/100;

        // calculate the vecotr
        Vector* vec = new Vector(*_destination, *_start);

        // get difference
        short xdiff = vec->getX()*percentage;
        short ydiff = vec->getY()*percentage;

        // Save the new coordinates
        predictedPos->x = _destination->x-xdiff;
        predictedPos->y = _destination->y-ydiff;
    }

    //check if we need to calculate the position after driving the turnaround circle
    if(_direction == DIRECTION_LEFT ||_direction == DIRECTION_RIGHT) {

        // Calculate the droven Angle
        double angle = CALC_DROVEN_ANGLE_BY_LENGTH(_distance);

        // Now we need the circle center
        // 1) First calculate the vector: start -> destination
        Vector *startDest;
        // check the direction to get the correct vector
        if(_direction == DIRECTION_LEFT) {
            startDest = new Vector(*_destination, *_start);
        } else {
            startDest = new Vector(*_start, *_destination);
        }

        // 2) Rotate the Max and now we got the Vector to the circle center
        startDest->rotate(angle);

        // 3) Change direction for the vector: circleCenter -> start
        startDest->changeDirection();

        // Now we need to calculate the predicted current position
        // 1) calculate the droven angle
        angle = CALC_DROVEN_ANGLE_BY_LENGTH(distance);

        // 2) now we need to calculate the vector to the predicted position
        startDest->rotate(angle);

        // 3) now we need just use the head as the predicted position
        if(_direction == DIRECTION_LEFT) {
            predictedPos->x = startDest->getHead()->x;
            predictedPos->y = startDest->getHead()->y;
        } else {
            predictedPos->x = startDest->getFoot()->x;
            predictedPos->y = startDest->getFoot()->y;
        }
    }

    // Return the predicted position
    return predictedPos;
}
