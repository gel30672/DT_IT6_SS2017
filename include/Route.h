//
// Created by Andreas Zinkl on 16.05.17.
//

#ifndef DT2017_ROUTE_H
#define DT2017_ROUTE_H

#include "RouteCalculation.h"
#include "CStack.h"
#include "DriveCalculation.h"
#include "Map.h"
#include "Command.h"

class Route {

private:
    Map* map;
    RouteCalculation* routeCalculater;
    DriveCalculation* driveCalculater;

    CStack<Command> drivingCommandStack;
    CStack<Position> destinationStack;

    void optimizeRoute();

public:
    Route(Map* map);
    ~Route();

    short startCalculation();

};


#endif //DT2017_ROUTE_H
