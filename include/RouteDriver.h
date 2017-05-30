//
// Created by Andreas Zinkl on 19.05.17.
//

#ifndef DT2017_ROUTEDRIVER_H
#define DT2017_ROUTEDRIVER_H

//this defines just an integer value which defines the max difference of the position comparison
#define POSITIONCOMPAREQUALITY 0

#include "RouteCalculation.h"
#include "DriveCalculation.h"
#include "Map.h"
#include "DeviceConfiguration.h"
#include <vector>
#include <stack>
#include "ErrorHandling.h"

class RouteDriver {

private:
    Map* map;
    RouteCalculation* routeCalculater;
    DriveCalculation* driveCalculater;

    std::vector<Position> destinations;

    // This initializes the RouteDriver - will be called by the Constructor
    short initRouteCalculation(short xDestination, short yDestination);
    short initDriveCalculation(Position *last, Position *current);
    short initRouterDriver();

    // This optimizes the calculated route by removing destinations which are on the same track
    void optimizeRoute();
    void saveToDestination(short x, short y);

    // Private check methods for the current command and movement
    bool currentCommandIsFinished();
    bool directionChangeIsNeeded();


public:
    RouteDriver(Map* map);
    ~RouteDriver();

    // Check the Route / Drive information
    // If a new command is needed there are 2 options available
    //  1. we just return the next command, because we are still on the way to the destination
    //  2. we need to recalculate the old route to get a new route
    short checkDrive();

};


#endif //DT2017_ROUTEDRIVER_H
