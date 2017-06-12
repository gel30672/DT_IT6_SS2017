//
// Created by Andreas Zinkl on 08.06.17.
//

#include "../include/Car.h"

// CONSTRUCTOR

Car::Car() {

    // init the car
    init();
}

Car::~Car() {
    delete &_currentPosition;
    delete _currentDirection;
}


// PUBLIC FUNCTIONS

void* Car::driveTo(Position *destination) {

    bool _reachedDestination = false;
    _currentDestination = destination;

    while(!_reachedDestination) {

        // we need to check if we reached the destination
        if(didReachDestination()) {
            _reachedDestination = true;
            continue; // finished the drive!!
        }

        // check if we got a running command
        if(!_cmdInterface->isCurrentDriveFinished()) continue;

        // Check if we are already in the correct orientation
        bool isStraightDrive = getCurrentDirection()->isOnLineTo(_currentDestination);

        // calculate the direct distance to the destination
        Vector* directDist = new Vector(*_currentDestination, *getCurrentPosition());

        // if a turn is needed, then process a turn
        if(!isStraightDrive) {

            // check on which side the destination is
            short side = getCurrentDirection()->getSideOf(directDist);

            // calculate the angle to the destinatino
            double angleToDest = 0.0;
            if(side == DIRECTION_LEFT) {
                angleToDest = getCurrentDirection()->getAngleTo(directDist);
            } else {
                angleToDest = directDist->getAngleTo(getCurrentDirection());
            }

            // now check the angle to the destination
            // define steering angle
            double steeringAngle = 0.0;
            double turnAroundLength = 0.0;
            if(angleToDest < 20) {

                // little turn
                steeringAngle = 5.0;

                // Turn with 5 degrees
                double radius = WHEELBASE/sin(steeringAngle);
                turnAroundLength = (2*M_PI*CIRCLERADIUS*(steeringAngle/360)) + WHEELBASE;
                _cmdInterface->sendTurnAroundDrive(turnAroundLength, _currentDestination, side);
            } else {

                // calculate the length to the direction
                double directLenght = directDist->getLength();

                // check if we can drive the turnaround or if we need a special turn
                if(directLenght < CIRCLERADIUS) {

                    // calculate a 90 degrees turnaround length
                    turnAroundLength = CALC_DISTANCE_BY_ANGLE(90)+WHEELBASE;

                    // need to do a special turn
                    _cmdInterface->sendTurnAroundAndBackDrive(turnAroundLength, _currentDestination, side);
                } else {

                    // we can do an simple turn

                    // first we need to check how far we can drive or if an obstacle is there
                    bool isObstacleIncluded = false;//todo

                    if(!isObstacleIncluded) {

                        // calcualte the length on the turnaround
                        turnAroundLength = CALC_DISTANCE_BY_ANGLE(angleToDest)+WHEELBASE;//todo correct calculation of the angle

                        // there is no obstacle so we can drive a normal turn around
                        _cmdInterface->sendTurnAroundDrive(turnAroundLength, _currentDestination, side);
                    } else {

                        // calculate the length on the turnaround
                        turnAroundLength = 0.0; //todo

                        // send the turnaround cmd with the backward drive
                        _cmdInterface->sendTurnAroundAndBackDrive(turnAroundLength, _currentDestination, side);
                    }
                }
            }

        } else {

            // no turn needed, check if we have a straight drive and if not, create one and execute that one
            _cmdInterface->sendForwardDrive(directDist->getLength(), _currentDestination);
        }
    }

    // before we leave, update the droven distance for new commands
    resetDrovenDistance();
}

Position* Car::getLastKnownPosition() {
    return &_lastKnownPosition;
}

Position* Car::getCurrentPosition() {

    // First we need to update / check the current position by asking the uwb device
    updateCurrentPosition();

    // return the current position;
    return &_currentPosition;
}

Vector* Car::getCurrentDirection() {
    return _currentDirection;
}

double Car::getDrovenDistance() {
    return distanceSinceStart;
}

short Car::getCurrentCarState() {
    return _carState;
}


// PRIVATE FUNCTIONS

short Car::init() {

    // Turn on the localization
    _localization = new LocDet();

    // Turn on the command interface
    _cmdInterface = new CarCommandInterface();

    // now we need to do a init drive
    updateCurrentPosition();
    initDrive();
    updateCurrentPosition();

    // Set an initial car state
    updateCarState(CAR_INITIALIZED);

    return SUCCESS;
}

short Car::initDrive() {

    // start driving the initialization way
    Command(INIT_CONFIG_DISTANCE, DIRECTION_FORWARD, nullptr).execute();

    // wait till we got the
    while(distanceSinceStart < INIT_CONFIG_DISTANCE) {
        //if(PRINT_ERROR_CODE) std::cout << "ROUTEDRIVER: DROVEN INIT DISTANCE (cm) = " << distanceSinceStart << std::endl;
    }

    // Okay we did the initialization drive
    Command(INIT_CONFIG_DISTANCE, DIRECTION_STOP, nullptr).execute();

    distanceSinceStart = 0;
}

short Car::updateCarState(short state) {
    _carState = state;
}

short Car::updateCurrentPosition() {

    // save the old position if the new position changed
    Position tempCurrent = _currentPosition;

    // update the current position
    _localization->get_position(&_currentPosition);

    // we need to convert the given position values into our map values (mm -> cm -> 15cm raster)
    _currentPosition.x = _currentPosition.x/(10*MapRasterWidth_cm);
    _currentPosition.y = _currentPosition.y/(10*MapRasterWidth_cm);

    // only save the last current position, if the new current position has changed regarding the last current position
    if(_currentPosition.x != tempCurrent.x
            && _currentPosition.y != tempCurrent.y) {
        _lastKnownPosition = tempCurrent;
    }

    // now we need to update the direction
    _currentDirection = new Vector(_currentPosition, _lastKnownPosition);
}

short Car::resetDrovenDistance() {
    distanceSinceStart = 0.0;
}

bool Car::didReachDestination() {

    short x = 0;
    short y = 0;
    if(_currentPosition.x > _currentDestination->x) {
        x = _currentPosition.x - _currentDestination->x;
    } else {
        x = _currentDestination->x - _currentPosition.x;
    }

    if(_currentPosition.y > _currentDestination->y) {
        y = _currentPosition.y - _currentDestination->y;
    } else {
        y = _currentDestination->y - _currentPosition.y;
    }

    if(x < POSITION_PRECISION && y < POSITION_PRECISION) {
        return true;
    }

    return false;
}