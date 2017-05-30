//
// Created by Andreas Zinkl on 07.05.17.
//

#include "../include/DriveCalculation.h"

DriveCalculation::DriveCalculation(Position* initStart, Position* initEnd) {

    // save the positions for the init process
    current = *initEnd;
    lastPositionKnown = *initStart;

    // now initialize the whole calculation
    initCalculation();
}

DriveCalculation::~DriveCalculation() {
    delete &current;
    delete &lastPositionKnown;
    delete &destination;
    delete &circleCore;
}

void DriveCalculation::initCalculation() {

    // Init the car and direction calculation
    Command initCMD = Command(INIT_CONFIG_DISTANCE, nullptr, nullptr, DIRECTION_FORWARD);
    initCMD.execute();

    // calculate the direction (the vector) we've droven
    initVector = new Vector(current, lastPositionKnown);
}

short DriveCalculation::calculate(Position *start, Position *end) {

    // calculate the destination vector
    Vector* destVector = new Vector(*end, *start);

    // the position on the turning circle, when the car can drive straight to the destination
    Position* endOfTurning = nullptr;

    // calculate the length of the way
    double lengthTurning;
    double lengthStraight;

    // generate the first command for turning the car
    Command *cmdTurning = nullptr;

    // generate the second command for the car driving straight from the turningpoint to the destination
    Command *cmdStraight = nullptr;

    // first check if the last way we drove is on the way to the destination or if a turn is needed
    if(!initVector->isOnLineTo(end)) {

        // get the direction
        short dir = initVector->getSideOf(destVector);

        // now calculate the turning point for drive changing
        calculateTurningPoint(destVector, dir);

        // get the degrees for the turn
        double degrees = initVector->getAngleTo(destVector)*0.7;
        // TODO Check if we can fix that by a correct calculation
        // TODO 0.8 is a factor which could be used to get the percentage of the degree the car needs to drive, to get in the correct direction

        // calculate the length of the way
        short lengthTurning = CALC_DISTANCE_BY_ANGLE(degrees);

        // calculate the endofturning point
        Vector *turningVec = new Vector(*start, circleCore);
        turningVec->rotate(degrees);

        // Now get the endofturning
        endOfTurning = turningVec->getHead();

        // generate the first command for turning the car
        cmdTurning = new Command(lengthTurning, start, endOfTurning, dir);
    }

    // check the endOfTurning position
    if(endOfTurning == nullptr) endOfTurning = start;

    // calculate the length of the straigth command
    lengthStraight = Vector(*end, *endOfTurning).getLength();

    // Now generate the straight command
    cmdStraight = new Command(lengthStraight, endOfTurning, end, DIRECTION_FORWARD);

    // save first the cmdStraight and then the cmdTurning on the stack
    if(cmdStraight != nullptr) drivingCommands.push(*cmdStraight);
    if(cmdTurning != nullptr) drivingCommands.push(*cmdTurning);

    return 0;
}

void DriveCalculation::calculateTurningPoint(Vector *currVec, short dir) {

    // calculate a point which is on the same x-coord like the last known position
    // we will use this one for the calculation of the vector which is parallel to the x-axis
    struct Position parallelX;
    parallelX.x = current.x;
    parallelX.y = lastPositionKnown.y;

    // get the parallel x-axis vector by using the parallelX coordinate
    Vector* parrVec = new Vector(parallelX, lastPositionKnown);

    // get the additional rotation angle
    float rotationAngle = currVec->getAngleTo(parrVec);

    // calculate the end rotation angle in degrees
    float degrees = 0.0;
    if(dir == DIRECTION_LEFT) {
        degrees = (float) DEGTORAD(90 + rotationAngle);
    } else {
        degrees = (float) DEGTORAD(-90 - rotationAngle);
    }
    parallelX.x = current.x+CIRCLERADIUS;
    parrVec = new Vector(parallelX, lastPositionKnown);

    // rotate the parrallel x-axis vector
    parrVec->rotate(degrees);

    // save the new middle point calculated coordinates
    circleCore.x = parrVec->getHead()->x;
    circleCore.y = parrVec->getHead()->y;

    /*
     * we may get some rounding problems here,
     * first test everything and then may modify the details if there are big problems
     */
}

short DriveCalculation::checkDestinationDirection() {

    // Get the driving direction
    Vector* posVector = new Vector(current, lastPositionKnown);

    // Get the destination direction
    Vector* destVector = new Vector(destination, lastPositionKnown);

    // return the direction
    return posVector->getSideOf(destVector);
}