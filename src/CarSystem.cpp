//
// Created by Andreas Zinkl on 08.06.17.
//

#include "../include/CarSystem.h"

// CONSTRUCTOR AND DECONSTRUCTOR

CarSystem::CarSystem() {
    _map = nullptr;
    _car = nullptr;
    _routeCalc = nullptr;
    _reachedDestinationsIndex = 0;
    _carFinishedDestination = true;
}

CarSystem::~CarSystem() {
    delete _map;
    delete _car;
    delete _routeCalc;
}


// PUBLIC FUNCTIONS

short CarSystem::initialize() {

    // Initialize all internal modules
    short mapRes = initMap();
    short carRes = initCar();
    short routeRes = initRouteCalculation();

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

Position* CarSystem::getCurrentDestination() {

    // check if the routing is intialized
    if(_routeCalc == nullptr) return nullptr;

    //check if we have destinations
    if(_routeCalc->_destinations.size() <= 0) return nullptr;
    if((_routeCalc->_destinations.size()-1) == _reachedDestinationsIndex) return nullptr;

    // now return the current destination‚
    return &_routeCalc->_destinations[_reachedDestinationsIndex];
}

short CarSystem::finishedCurrentDestination() {

    // this finishes the current destination and increases the counter to get the next destinatino
    _reachedDestinationsIndex++;
    _carFinishedDestination = true;

    return SUCCESS;
}

short CarSystem::run() {

    // This needs to check the result of the car
    if(!_carFinishedDestination) return SUCCESS; // We need to wait, till the car finished the drive

    // We can drive to a destination!
    // check if we got a destination
    if(_routeCalc == nullptr) return ERROR_WHILE_ROUTECALC_INIT;
    if(_routeCalc->_destinations.size() <= 0) return ERROR_WHILE_DRIVECALC_INIT;
    if(_routeCalc->_destinations.size()-1 == _reachedDestinationsIndex) return SUCCESS;

    // create a new thread for the car drive
    pthread_t driveThread[1];

    // get the current destination
    Position *destination = getCurrentDestination();

    // okay so now tell the car to drive to a new destination
    _carFinishedDestination = false;
    int drive = pthread_create(&driveThread[0], NULL, &CarSystem::sendDriveCommandToCar, (void*)this);

    return SUCCESS;
}

void* CarSystem::sendDriveCommandToCar(void *carsystem) {

    CarSystem* carsys = (CarSystem *) carsystem;
    carsys->tellCarToDrive(carsys->getCurrentDestination());
}

void* CarSystem::tellCarToDrive(void *position) {

    // define the destination and tell it the car
    Position* dest = (struct Position *) position;
    _car->driveTo(dest);

    // the car has finished it's drive
    finishedCurrentDestination();
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

short CarSystem::calculateRoute() {

    // define the destination - therefore use the Define in DeviceConfiguration or a I/O interface
    if(USE_CONSOLE_FOR_DESTINATION_INPUT) askUserForDefinedDestination();
    else {
        _finalDestinaion.x = DESTINATION_X_COORDINATE;
        _finalDestinaion.y = DESTINATION_Y_COORDINATE;
    }

    // this needs to be done after initialization of all modules
    short res = _routeCalc->calculate(_map, _car->getCurrentPosition(), &_finalDestinaion);

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