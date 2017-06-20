//
// Created by Andreas Zinkl on 08.06.17.
//

#ifndef DT2017_CARSYSTEM_H
#define DT2017_CARSYSTEM_H

#include "Map.h"
#include "RouteCalculation.h"
#include "Car.h"
#include "ErrorHandling.h"
#include "LaserSensor.h"
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
    int checkSensor();
    void emergencystop(bool shouldstop);


private:

    // Contents
    Map* _map;
    Car* _car;
    LaserSensor* _laser;
    RouteCalculation* _routeCalc;

    // Routing Content
    Position _finalDestinaion;
    int _reachedDestinationsIndex;
    bool _carFinishedDestination;

    // System initializations
    short initMap();
    short initCar();
    short initLaserSensor();
    short initRouteCalculation();

    // System Tasks
    short calculateRoute();
    void tellCarToDrive(Position *position);

    // I/O methods
    void askUserForDefinedDestination();

};


#endif //DT2017_CARSYSTEM_H
