//
// Created by Andreas Zinkl on 08.06.17.
//

#ifndef DT2017_CARSYSTEM_H
#define DT2017_CARSYSTEM_H

#include "Map.h"
#include "RouteCalculation.h"
#include "Car.h"
#include "ErrorHandling.h"
#include <iostream>
#include <pthread.h>

class CarSystem {

public:

    // Constructor and Deconstructor
    CarSystem();
    ~CarSystem();

    // Public Functionality
    short initialize();
    short run();
    short finishedCurrentDestination();
    Position* getCurrentDestination();


private:

    // Contents
    Map* _map;
    Car* _car;
    RouteCalculation* _routeCalc;

    // Routing Content
    Position _finalDestinaion;
    int _reachedDestinationsIndex;
    bool _carFinishedDestination;

    // System initializations
    short initMap();
    short initCar();
    short initRouteCalculation();

    // System Tasks
    short calculateRoute();
    void tellCarToDrive(Position *position);

    // I/O methods
    void askUserForDefinedDestination();

};


#endif //DT2017_CARSYSTEM_H
