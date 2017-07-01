//
// Created by Andreas Zinkl on 08.06.17.
//

#include <unistd.h>
#include "../include/Car.h"

// CONSTRUCTOR

Car::Car(int* needEmergencyStop) {

    // init the car
    init();
    _emergency = needEmergencyStop;
    _finishedDrive = false;
    _updatePositionCounter = 0;
}

Car::~Car() {
    //delete &_currentPosition;
    //delete _currentDirection;
}


// PUBLIC FUNCTIONS

short Car::go2(Position* dest) {

    // check if we reached the dest and return with YES!!
    _currentDestination = dest;
    _finishedDrive = false;

    // Look for the current direction and the destination direction
    Vector* currentDirection = getCurrentDirection();
    Vector* destinationDirection = getCurrentDestinationDirection();

    //std::cout << "DIRECTIONVECTOR = " << currentDirection->getX() << " | " << currentDirection->getY() << std::endl;

    // calculate the angle between
    short side = currentDirection->getSideOf(destinationDirection);
    double angle =0.0;
    if(side == DIRECTION_LEFT) {
        angle = currentDirection->getAngleTo(destinationDirection);
    } else {
        angle = destinationDirection->getAngleTo(currentDirection);
    }

    // check what to do.. straight or turnaround
    int res = SUCCESS;
    double lastAngle = angle;
    while(angle > 80) {
        std::cout << "ANGLE is " << angle << std::endl;

        // first proceed a turnaround
        res = orientationTurn(angle, side, destinationDirection);
        if(res == NEW_ROUTE_NEEDED) return NEW_ROUTE_NEEDED;

        // init the direction
        updateCurrentPosition();
        initDrive();
        updateCurrentPosition();

        // update the directions
        angle = _currentDirection->getAngleTo(_currentDestinationDirection);
        side = _currentDirection->getSideOf(_currentDestinationDirection);

        // check if we reached the destination
        if(_finishedDrive) return SUCCESS;
    }
    //res = orientationTurn(angle, side, destinationDirection);
    //if(res == NEW_ROUTE_NEEDED) return NEW_ROUTE_NEEDED;
    res = straightDrive(angle, side, destinationDirection);

    return res;
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

    // check if we reached the destination
    if(didReachDestination()) {
        _cmdInterface->sendStopCommand();
        _cmdInterface->activateEmergencyStop();
    }

    // return the current position;
    return &_currentPosition;
}

Vector* Car::getCurrentDirection() {
    updateCurrentPosition();
    return _currentDirection;
}

Vector* Car::getCurrentDestinationDirection() {
    updateCurrentPosition();
    return _currentDestinationDirection;
}

double Car::getDrovenDistance() {
    return distanceSinceStart;
}

short Car::getCurrentCarState() {
    return _carState;
}

void Car::setLaserSensor(LaserSensor *laser) {
    _laser = laser;
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
    //std::cout << "got position from uwb sensor" << std::endl;

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
    if(_currentDestination != nullptr) {
    	std::cout << ">>> CURRENT DIRECTION = " << _currentDirection->getX() << "," << _currentDirection->getY() << std::endl;
        _currentDestinationDirection = new Vector(*_currentDestination, _currentPosition);
    	std::cout << ">>> CURRENT DESTDIRECTION = " << _currentDestinationDirection->getX() << "," << _currentDestinationDirection->getY() << std::endl;
    }

    didReachDestination();
}

short Car::resetDrovenDistance() {
    distanceSinceStart = 0.0;
}

bool Car::didReachDestination() {

    //std::cout << "did reach check" << std::endl;

	if(_currentDestination == nullptr) return false;
    //updateCurrentPosition();

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
        std::cout << ">>>>>>>>>>>>>>>>>> REACHED DESTINATION!!!! <<<<<<<<<<<<<<<<<<<" << std::endl;
        _finishedDrive = true;
        _cmdInterface->sendStopCommand();
        return true;
    }

    return false;
}

int Car::straightDrive(float angle, short side, Vector* destVector) {

    // need to tasks here..
    // task 1 = drive to the destination
    // task 2 = correct the drive by setting a angle
    double lastSavedDistance = distanceSinceStart = 0;
    short directionCheckCnt = 1;
    double lastDistanceToDest = destVector->getLength();

    // just drive forward
    SteerDegrees(STRAIGHT_WHEEL_ANGLE);
    _cmdInterface->sendForwardDrive(destVector->getLength(), destVector->getHead());
    int obstacleCounter = 0;

    // drive straight forward
    while(!didReachDestination()) {

    	updateCurrentPosition();

    	int laserResults = checkForObstacles(obstacleCounter);
    	//if(laserResults) orientationTurn(angle, side, getCurrentDirection());

        // we need to configure steering if necessary
        if (angle == 0.0) {
            SteerDegrees(STRAIGHT_WHEEL_ANGLE);
        } else if(side == DIRECTION_LEFT) {
            SteerDegrees(-1*angle/3+STRAIGHT_WHEEL_ANGLE);
        } else if(side == DIRECTION_RIGHT) {
            SteerDegrees(angle/3+STRAIGHT_WHEEL_ANGLE);
        }

        // now we need to check, if the steering drove is done
        if(distanceSinceStart-lastSavedDistance > WHEELBASE) {

            // reset steering
            angle = 0.0;
            lastSavedDistance = distanceSinceStart;
        }

        // calculate the angle between
        Vector *currentDirection = getCurrentDirection();
        short side = currentDirection->getSideOf(destVector);
        double angle =0.0;
        if(side == DIRECTION_LEFT) {
            angle = currentDirection->getAngleTo(destVector);
        } else {
            angle = destVector->getAngleTo(currentDirection);
        }

        // check if we drive away
        /*lastDistanceToDest = getCurrentDestinationDirection()->getLength();
        updateCurrentPosition();
        if(lastDistanceToDest > getCurrentDestinationDirection()->getLength()) {
            int res = orientationTurn(180, _currentDirection->getSideOf(_currentDestinationDirection), _currentDestinationDirection);
            if(res == NEW_ROUTE_NEEDED) return NEW_ROUTE_NEEDED;
            _cmdInterface->sendForwardDrive(_currentDestinationDirection->getLength(), destVector->getHead());
        }*/

        // check for obstacles
        //int laserResults = checkForObstacles(obstacleCounter);
        //if(laserResults != SUCCESS) {
        //   return laserResults;
        //}
    }
    std::cout << "finished straight" << std::endl;
    return SUCCESS;
}

int Car::orientationTurn(float angle, short side, Vector* destVector) {

    // we need just to change to the correct orientation
    double fullTurnDistance = CALC_DISTANCE_BY_ANGLE(angle/2);

    // define the laser value
    int laserResults = 0;
    int obstacleCounter = 0;

    // now proceed the turnaround
    sleep(1);
    _cmdInterface->sendTurnAroundDrive(fullTurnDistance, destVector->getHead(), side);
    while(!_cmdInterface->isCurrentDriveFinished()) {

        // check for obstacles
        laserResults = checkForObstacles(obstacleCounter);
        if(laserResults != SUCCESS) break;
        //if(laserResults != SUCCESS) return laserResults;

        // always also check if we reached the destination
        updateCurrentPosition();
        if(didReachDestination()) return SUCCESS;
    }

    _cmdInterface->sendStopCommand();
    sleep(2); // wait because the car needs to stand still

    _cmdInterface->sendBackwardDrive(fullTurnDistance, destVector->getHead(), side);
    while(!_cmdInterface->isCurrentDriveFinished()) {

        // always also check if we reached the destination
    	updateCurrentPosition();
        if(didReachDestination()) return SUCCESS;
    }

    _cmdInterface->sendStopCommand();
    //updateCurrentPosition();

    return SUCCESS;
}

short Car::checkForObstacles(int obstacleCounter) {

    // check the laser sensor
    short laserResults = _laser->doLaserScanAndMapUpdate(getCurrentDirection());

    // check if there's a obstacle in front of us
    if(laserResults & 2) {
    	std::cout << "obstacle!!!!!!" << std::endl;
		_cmdInterface->sendStopCommand();


        // if the obstacle does not move -> we need a new route
        if(obstacleCounter >= 20) return NEW_ROUTE_NEEDED;
    }

    // check if we need a new route
    if(laserResults && NEW_ROUTE_NEEDED) return NEW_ROUTE_NEEDED;

    return SUCCESS;
}
