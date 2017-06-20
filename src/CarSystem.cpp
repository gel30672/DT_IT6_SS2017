//
// Created by Andreas Zinkl on 08.06.17.
//

#include "../include/CarSystem.h"

// CONSTRUCTOR AND DECONSTRUCTOR

CarSystem::CarSystem() {
    _map = nullptr;
    _car = nullptr;
    _routeCalc = nullptr;
    _laser = nullptr;
    _reachedDestinationsIndex = 0;
    _carFinishedDestination = true;
}

CarSystem::~CarSystem() {
    //delete _map;
    //delete _car;
    //delete _routeCalc;
}


// PUBLIC FUNCTIONS

short CarSystem::initialize() {

    // Initialize all internal modules
    std::cout << "init map" << std::endl;
    short mapRes = initMap();
    std::cout << "init car" << std::endl;
    short carRes = initCar();
    std::cout << "init route" << std::endl;
    short LaserRes = initLaserSensor();
    short routeRes = initRouteCalculation();
    std::cout << "end inits" << std::endl;

    // print the init states
    if(PRINT_ERROR_CODE) {
        std::cout << "##SYSTEM INITIALIZATION##" << std::endl;
        std::cout << "##CARSYSTEM## Map =" << mapRes << std::endl;
        std::cout << "##CARSYSTEM## Car =" << carRes << std::endl;
        std::cout << "##CARSYSTEM## Route =" << routeRes << std::endl;
    }

    // check if the init was without any errors
    if(mapRes != SUCCESS || carRes != SUCCESS || routeRes != SUCCESS) {
        return ERROR_WHILE_SYSTEM_INITIALIZATION;
    }

    // okay everything went through well - ask for a destination and calculate the route
    calculateRoute();

    return SUCCESS;
}

void CarSystem::checkSensor()
{
    _laser->doLaserScanAndMapUpdate();
}

Position* CarSystem::getCurrentDestination() {

    if(_reachedDestinationsIndex >= _routeCalc->_destinations.size()) {
        std::cout << "STOP THE CAR" << std::endl;
        _car->tellCarToStop();
        return nullptr;
    }
    _reachedDestinationsIndex = _routeCalc->_destinations.size()-1; // just take the last coordinate -> destination

    // check if the routing is intialized
    if(_routeCalc == nullptr) return nullptr;

    //check if we have destinations
    if(_routeCalc->_destinations.size() <= 0) return nullptr;
    if(_routeCalc->_destinations.size() == _reachedDestinationsIndex) return nullptr;

    // now return the current destination‚
    return _routeCalc->_destinations[_reachedDestinationsIndex];
}

short CarSystem::finishedCurrentDestination() {

    // this finishes the current destination and increases the counter to get the next destinatino
    _reachedDestinationsIndex++;
    _carFinishedDestination = true;

    return SUCCESS;
}

short CarSystem::run() {

    // This needs to check the result of the car
    if(!_carFinishedDestination) {
        return SUCCESS;
    } // We need to wait, till the car finished the drive

    // We can drive to a destination!
    // check if we got a destination
    if(_routeCalc == nullptr) return ERROR_WHILE_ROUTECALC_INIT;
    if(_routeCalc->_destinations.size() <= 0) return ERROR_WHILE_DRIVECALC_INIT;
    if(_routeCalc->_destinations.size()-1 <= _reachedDestinationsIndex) {
        exit(-1);
        return SUCCESS;
    }

    // okay so now tell the car to drive to a new destination
    _carFinishedDestination = false;

    tellCarToDrive(getCurrentDestination());

    return SUCCESS;
}

void CarSystem::tellCarToDrive(Position *position) {

    std::cout << "CARSYSTEM: CALLED CAR" << std::endl;

    // define the destination and tell it the car
    _car->go2(position);

    // the car has finished it's drive
    std::cout << ">>> FINISHED DRIVE <<<" << std::endl;
    finishedCurrentDestination();
    _car->tellCarToStop();    // tell the car to stop because we reached the destination

    // check if we are at the destination
    if(_reachedDestinationsIndex >= _routeCalc->_destinations.size()-1) {
        _car->tellCarToStop();
    }

    Position* newDestination = getCurrentDestination();
    if(newDestination != nullptr) {
        std::cout << "NEW DESTINATION!!! " << newDestination->x << "|" << newDestination->y << std::endl;
        tellCarToDrive(newDestination);
    }
}


// PRIVATE FUNCTIONS

short CarSystem::initMap() {

    // check if we already have a map
    if(_map == nullptr) {
        _map = new Map();
    }
    return SUCCESS;
}

short CarSystem::initCar() {

    // check if we already have a car
    if(_car == nullptr) {
        _car = new Car();
    }
    return SUCCESS;
}

short CarSystem::initRouteCalculation() {

    // check if we already have a routecalcluation
    if(_routeCalc == nullptr) {
        _routeCalc = new RouteCalculation();
    }
    return SUCCESS;
}

short CarSystem::initLaserSensor(){

    // check if we already have a laser object
    if(_laser == nullptr) {
        _laser = new LaserSensor(_map,_car);
    }
    return SUCCESS;
}

short CarSystem::calculateRoute() {

    // define the destination - therefore use the Define in DeviceConfiguration or a I/O interface
    if(USE_CONSOLE_FOR_DESTINATION_INPUT) askUserForDefinedDestination();
    else {
        _finalDestinaion.x = DESTINATION_X_COORDINATE;
        _finalDestinaion.y = DESTINATION_Y_COORDINATE;
    }

    // this needs to be done after initialization of all modules
    short res = _routeCalc->calculate(_map, &_car->_currentPosition, &_finalDestinaion);

    if(PRINT_ERROR_CODE) std::cout << "##CARSYSTEM## RouteCalculation =" << res << std::endl;

    // optimize the calculated route every time, to get just main destination points
    _routeCalc->optimizeRouteDestinations();

    return SUCCESS;
}

void CarSystem::askUserForDefinedDestination() {

    // Create a CMD UI
    std::cout << "\n\n########### ENTER THE DESTINATION ###########" << std::endl;
    bool correctValue = false;
    string confirmation = "";
    while(!correctValue) {

        std::cout << "Enter the X-Coordinate: ";
        std::cin >> _finalDestinaion.x;
        std::cout << "The entered X-Coordinate is " << _finalDestinaion.x << " right (y/n)? ";
        std::cin >> confirmation;
        if(confirmation == "y" || confirmation == "Y") correctValue = true;
    }

    correctValue = false;
    while(!correctValue) {

        std::cout << "Enter the Y-Coordinate: ";
        std::cin >> _finalDestinaion.y;
        std::cout << "The entered Y-Coordinate is " << _finalDestinaion.y << " right (y/n)? ";
        std::cin >> confirmation;
        if(confirmation == "y" || confirmation == "Y") correctValue = true;
    }

    std::cout << "########### THANK YOU FOR ENTERING A DESTINATION ###########" << std::endl;

    std::cout << "########### The calculation to Position (" << _finalDestinaion.x << "|" << _finalDestinaion.y << ") will start now! ###########\n\n" << std::endl;

}