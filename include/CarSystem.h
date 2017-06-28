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
    void emergencystop(bool shouldstop);


private:

    // Contents
    Map* _map;
    Car* _car;
    LaserSensor* _laser;
    RouteCalculation* _routeCalc;
    LocDet* _localization;

    // Routing Content
    Position _finalDestinaion;
    int _reachedDestinationsIndex;
    bool _carFinishedDestination;
    int _needEmergencyStop;

    // System initializations
    short initMap();
    short initCar();
    short initLaserSensor();
    short initRouteCalculation();
    short initUWBSensor();

    // System Tasks
    short calculateRoute();
    void tellCarToDrive(Position *position);

    // Threading methods
    static void* runLaser(void* laser);
    static void* runCar(void* car);

    // I/O methods
    void askUserForDefinedDestination();

};


#endif //DT2017_CARSYSTEM_H
