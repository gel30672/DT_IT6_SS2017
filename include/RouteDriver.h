//
// Created by Andreas Zinkl on 19.05.17.
//

#ifndef DT2017_ROUTEDRIVER_H
#define DT2017_ROUTEDRIVER_H

#include "RouteCalculation.h"
#include "DriveCalculation.h"
#include "Map.h"
#include "DeviceConfiguration.h"
#include <vector>
#include <stack>

class RouteDriver {

private:
    Map* map;
    RouteCalculation* routeCalculater;
    DriveCalculation* driveCalculater;

    std::vector<Position> destinations;
    std::stack<Command> driverCommands;

    // This initializes the RouteDriver - will be called by the Constructor
    short initRouteCalculation();
    short initDriveCalculation();
    short initRouterDriver();

    // This optimizes the calculated route by removing destinations which are on the same track
    void optimizeRoute();
    void saveNodeAsDestination(Node *node);

    // this executes the current command
    short executeCurrentCommand();



public:
    RouteDriver(Map* map);
    ~RouteDriver();

    // Check the Route / Drive information
    // If a new command is needed there are 2 options available
    //  1. we just return the next command, because we are still on the way to the destination
    //  2. we need to recalculate the old route to get a new route
    bool checkDrive();

};


#endif //DT2017_ROUTEDRIVER_H
