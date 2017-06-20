//
// Created by Andreas Zinkl on 08.06.17.
//

#include <unistd.h>
#include "../include/Car.h"

// CONSTRUCTOR

Car::Car() {

    // init the car
    init();
}

Car::~Car() {
    //delete &_currentPosition;
    //delete _currentDirection;
}


// PUBLIC FUNCTIONS

short Car::go2(Position* dest) {

    // check if we reached the dest and return with YES!!
    _currentDestination = dest;

    // Look for the current direction and the destination direction
    Vector* currentDirection = getCurrentDirection();
    Vector* destinationDirection = new Vector(*dest, *getCurrentPosition());

    std::cout << "DIRECTIONVECTOR = " << currentDirection->getX() << " | " << currentDirection->getY() << std::endl;

    // calculate the angle between
    short side = currentDirection->getSideOf(destinationDirection);
    float angle = 0.0;
    if(side == DIRECTION_LEFT) {
        angle = currentDirection->getAngleTo(destinationDirection);
    } else {
        angle = destinationDirection->getAngleTo(currentDirection);
    }
    std::cout << "ANGLE is " << angle << std::endl;

    // check what to do.. straight or turnaround
    if(angle <= 20) {
        std::cout << "STRAIGHT!!!" << std::endl;
        straightDrive(angle, side, destinationDirection);
    } else {
        std::cout << "TURNAROUND!!!" << std::endl;
        orientationTurn(side, destinationDirection);
        go2(dest); // just 1 time recursive -> next time we need to drive less then 20 degrees
    }

    return SUCCESS;
}

void* Car::driveTo(Position *destination) {

    bool _reachedDestination = false;
    _currentDestination = destination;

    while(!_reachedDestination) {

        // we need to check if we reached the destination
        if(didReachDestination()) {
            _reachedDestination = true;
            break; // finished the drive!!
        }

        // check if we got a running command
        if(!_cmdInterface->isCurrentDriveFinished()) break;

        // Check if we are already in the correct orientation
        bool isStraightDrive = getCurrentDirection()->isOnLineTo(_currentDestination);

        // calculate the direct distance to the destination
        Vector* directDist = new Vector(*_currentDestination, *getCurrentPosition());

        std::cout << "CAR: CALC DRIVE.. " << std::endl;

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

            std::cout << ">>>> DRIVE STRAIGHT" << std::endl;

            // no turn needed, check if we have a straight drive and if not, create one and execute that one
            _cmdInterface->sendForwardDrive(directDist->getLength(), _currentDestination);
        }
    }

    // before we leave, update the droven distance for new commands
    resetDrovenDistance();
}

void Car::tellCarToStop() {
    _cmdInterface->sendStopCommand();
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
    updateCurrentPosition();
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
    std::cout << "Initialized LocDet" << std::endl;

    // Turn on the command interface
    _cmdInterface = new CarCommandInterface();

    // set current positions to -1
    _lastKnownPosition.x = -1;
    _lastKnownPosition.y = -1;
    _currentPosition.x = -1;
    _currentPosition.y = -1;

    // now we need to do a init drive
    updateCurrentPosition();
    std::cout << "updated position" << std::endl;
    initDrive();
    updateCurrentPosition();

    // Set an initial car state
    updateCarState(CAR_INITIALIZED);

    return SUCCESS;
}

short Car::initDrive() {

    std::cout << "init drive!!!" << std::endl;
    // start driving the initialization way
    _cmdInterface->sendForwardDrive(INIT_CONFIG_DISTANCE, nullptr);

    // wait till we got the
    while(distanceSinceStart < INIT_CONFIG_DISTANCE) {
        //if(PRINT_ERROR_CODE) std::cout << "ROUTEDRIVER: DROVEN INIT DISTANCE (cm) = " << distanceSinceStart << std::endl;
    }

    // Okay we did the initialization drive
    _cmdInterface->sendStopCommand();

    distanceSinceStart = 0;
}

short Car::updateCarState(short state) {
    _carState = state;
}

short Car::updateCurrentPosition() {

    // save the old position if the new position changed
    Position tempCurrent = _currentPosition;

    // receive current position from uwb
    Position uwbPosition;
    _localization->get_position(&uwbPosition);
    std::cout << "got position from uwb sensor" << std::endl;

    // we need to convert the given position values into our map values (mm -> cm -> 15cm raster)
    float currX = ((float)uwbPosition.x)/(10*MapRasterWidth_cm/UWBMapRasterPrecision);
    float currY = ((float)uwbPosition.y)/(10*MapRasterWidth_cm/UWBMapRasterPrecision);

    //std::cout << ">>> UWB POSITION X=" << currX << " | Y= " << currY << std::endl;

    _currentPosition.x = ceil(currX/UWBMapRasterPrecision);
    _currentPosition.y = ceil(currY/UWBMapRasterPrecision);

    // only save the last current position, if the new current position has changed regarding the last current position
    if( _currentPosition.x != tempCurrent.x
            && _currentPosition.y != tempCurrent.y) {
        _lastKnownPosition = tempCurrent;
    } else if(_lastKnownPosition.x == -1 && _lastKnownPosition.y == -1) {
        _lastKnownPosition = tempCurrent;
    }

    std::cout << ">>> CURRENT POSITION = (" << _currentPosition.x << " | " << _currentPosition.y << ")";
    if(_currentDestination != nullptr) {
        std::cout << " //// CURRENT DESTINATION = (" << _currentDestination->x << " | " << _currentDestination->y << ")" << std::endl;
    } else {
        std::cout << std::endl;
    }

    // now we need to update the direction
    //std::cout << "current pos = " << _currentPosition.x << "," << _currentPosition.y << std::endl;
    //std::cout << "last pos = " << _lastKnownPosition.x << "," << _lastKnownPosition.y << std::endl;
    _currentDirection = new Vector(_currentPosition, _lastKnownPosition);
}

short Car::resetDrovenDistance() {
    distanceSinceStart = 0.0;
}

bool Car::didReachDestination() {

    std::cout << "did reach check" << std::endl;

    updateCurrentPosition();

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

void Car::straightDrive(float angle, short side, Vector* destVector) {

    // need to tasks here..
    // task 1 = drive to the destination
    // task 2 = correct the drive by setting a angle
    double lastSavedDistance = distanceSinceStart = 0;
    short directionCheckCnt = 1;

    // just drive forward
    SteerDegrees(STRAIGHT_WHEEL_ANGLE);
    _cmdInterface->sendForwardDrive(destVector->getLength(), destVector->getHead());

    // drive
    std::cout << "DRIVE STRAIGHT " << destVector->getLength() << " cm" << std::endl;
    while(!didReachDestination() && distanceSinceStart < destVector->getLength()) {

        std::cout << "staright with angle " << angle << std::endl;

        // we need to configure steering if necessary
        if (angle == 0.0) {
            SteerDegrees(STRAIGHT_WHEEL_ANGLE);
        } else if(side == DIRECTION_LEFT) {
            SteerDegrees(-1*angle/2+STRAIGHT_WHEEL_ANGLE);
            std::cout << "LEFT" << std::endl;
        } else if(side == DIRECTION_RIGHT) {
            SteerDegrees(angle/2+STRAIGHT_WHEEL_ANGLE);
            std::cout << "RIGHT" << std::endl;
        }

        // now we need to check, if the steering drove is done
        if(distanceSinceStart-lastSavedDistance > WHEELBASE) {

            // reset steering
            std::cout << "resets" << std::endl;
            angle = 0.0;
            lastSavedDistance = 0;
        }

        // may check for angle correctness - only check every 10th try
        if(directionCheckCnt%2==0) {
            updateCurrentPosition();
            destVector->setFoot(&_currentPosition);
            short crSide = getCurrentDirection()->getSideOf(destVector);
            if (crSide == DIRECTION_LEFT) {
                angle = getCurrentDirection()->getAngleTo(destVector);
            } else {
                angle = destVector->getAngleTo(getCurrentDirection());
            }

            if(angle <= 5) {
                angle = 0.0;
            }
        }
    }
    std::cout << "finished straight" << std::endl;
    _cmdInterface->sendStopCommand();
}

void Car::orientationTurn(short side, Vector* destVector) {

    // calculate the angle
    double destAngle = getCurrentDirection()->getAngleTo(destVector);

    // we need just to change to the correct orientation
    double fullTurnDistance = CALC_DISTANCE_BY_ANGLE(destAngle/2);

    // now change the orientation - first step forward drive
    distanceSinceStart = 0;
    std::cout << "start turnaround forward" << std::endl;
    _cmdInterface->sendTurnAroundDrive(fullTurnDistance, destVector->getHead(), side);
    std::cout << "sent turnaround forward" << std::endl;
    while(distanceSinceStart <= fullTurnDistance) {
        // do nothing
        std::cout << "wait" << std::endl;
    }

    distanceSinceStart = 0;
    std::cout << "start turnaround forward" << std::endl;
    _cmdInterface->sendBackwardDrive(fullTurnDistance, destVector->getHead(), side);
    while(distanceSinceStart <= fullTurnDistance) {
        // do nothing
        std::cout << "wait" << std::endl;
    }

    std::cout << "finished turn" << std::endl;
    _cmdInterface->sendStopCommand();

    //update currentposition
    updateCurrentPosition();
}