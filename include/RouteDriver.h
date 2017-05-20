//
// Created by Andreas Zinkl on 19.05.17.
//

#ifndef DT2017_ROUTEDRIVER_H
#define DT2017_ROUTEDRIVER_H


class RouteDriver {

private:
    Map* map;
    RouteCalculation* routeCalculater;
    DriveCalculation* driveCalculater;

    std::stack<Position> destinations;
    std::stack<Command> driverCommands;

    // This initializes the RouteDriver - will be called by the Constructor
    short initCalculation();

    // This optimizes the calculated route by removing destinations which are on the same track
    void optimizeRoute();

public:
    RouteDriver(Map* map);
    ~RouteDriver();

    // Checks the current command and if a new command is needed there are 2 options available
    // 1. we just return the next command, because we are still on the way to the destination
    // 2. we need to recalculate the old route to get a new route
    Command checkCommands();

};


#endif //DT2017_ROUTEDRIVER_H
