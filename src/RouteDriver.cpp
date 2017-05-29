//
// Created by Andreas Zinkl on 19.05.17.
//

#include "../include/RouteDriver.h"

RouteDriver::RouteDriver(Map *map) : map(map){

    //initialize the route calculation
    initRouterDriver();
}

RouteDriver::~RouteDriver() {

    // free space
    delete &routeCalculater;
    delete &driveCalculater;
    delete &destinations;
}

short RouteDriver::initRouteCalculation(short xDestination, short yDestination) {

    // init the route calculater
    routeCalculater = new RouteCalculation(map, xDestination, yDestination);

    // calculate the route
    bool calcRes = routeCalculater->calculate();

    // check if the calculation went well
    if(calcRes != NO_PATH_FOUND_ERROR) return COULD_NOT_CALCULATE_ROUTE_ERROR;

    return calcRes;
}

short RouteDriver::initDriveCalculation(Position *last, Position *current) {

    // init the drive calculater
    driveCalculater = new DriveCalculation(last, current);

    // start generation of commands
    Position* lastPosition = &destinations[0]; // at beginning it is the start position
    for(int i = 1; i < destinations.size(); i++) {

        // get the current destination
        Position* destination = &destinations[i];

        // calculate the command for this drive
        int calcError = driveCalculater->calculate(lastPosition, destination);

        // check calcRes if there's a error message
        if(calcError) return COULD_NOT_CALCULATE_DRIVECOMMAND_ERROR;

        // save the destination as the lastPosition for the next calculation
        lastPosition = destination;
    }

    // we successfully calculated the commands
    return 0;
}

short RouteDriver::initRouterDriver() {

    // get and save the current position as lastpositionknown before driving
    Position *current = map->getCarPosition();

    // drive the initialization way and stop the car again
    Command initCMD = Command(INIT_CONFIG_DISTANCE, current, nullptr, 0);
    initCMD.execute();

    // get the new position and save it as currentposition
    current = map->getCarPosition();

    // initialize the route calculation
    short initResRoute = initRouteCalculation(DESTINATION_X_COORDINATE, DESTINATION_Y_COORDINATE);

    // optimize the current route to get just main nodes
    optimizeRoute();

    // initialize the drive calculator and generate all commands
    short initResDrive = initDriveCalculation(map->getLastKnownPosition(), current);

    // check for errors
    if(!initResRoute) return ERROR_WHILE_ROUTECALC_INIT;
    if(!initResDrive) return ERROR_WHILE_DRIVECALC_INIT;

    // everything went well! go for it!
    return 0;
}

void RouteDriver::saveToDestination(short x, short y) {

    // transform node to position
    Position pos;
    pos.x = x;
    pos.y = y;

    // save the position
    destinations.push_back(pos);
}

void RouteDriver::optimizeRoute() {

    // Now start optimizing the route by going through the whole route
    Position* predecessor = nullptr;
    while(routeCalculater->getRouteNodeCount() > 0) {

        // Get the top node from the route stack
        Node node = routeCalculater->popNodeFromRouteStack();

        // check if it is the first node
        if(destinations.size() == 0) {

            // always save the first node
            saveToDestination(node.getX(), node.getY());
        } else {

            // not the first destination
            bool needSave = false;
            bool lastWasDiagonal = false;

            // check if we can save the destination
            short indexPredecessor = destinations.size()-1;
            if(predecessor == nullptr) {
                Position pre;
                pre.x = destinations[indexPredecessor].x;
                pre.y = destinations[indexPredecessor].y;
                predecessor = &pre;
            }

            // Check if the old and new one are on the same diagonal line
            if((quad(node.getX()+1) == quad(predecessor->x) && quad(node.getY()+1) == quad(predecessor->y))
               || (quad(node.getX()-1) == quad(predecessor->x) && quad(node.getY()-1) == quad(predecessor->y))){

                lastWasDiagonal = true;
                needSave = true;
            }

            // Check if the positions are on the same horizontal line
            if(node.getY() == predecessor->y
               && (node.getX()+1 == predecessor->x || node.getX()-1 == predecessor->x)
               && lastWasDiagonal) {

                lastWasDiagonal = false;
                needSave = true;
            }

            // Check if the position is on the same vertical line
            if((node.getX() == predecessor->x
                && (node.getY()+1 == predecessor->y || node.getY()-1 == predecessor->y))
                && lastWasDiagonal) {

                lastWasDiagonal = false;
                needSave = true;
            }

            // Check if it is the last position
            if(routeCalculater->getRouteNodeCount() == 0) needSave = true;

            // That one is not in the line, save the predecessor
            if(needSave) saveToDestination(node.getX(), node.getY());

            // save as predecessor details
            predecessor->x = node.getX();
            predecessor->y = node.getY();
        }
    }

}

bool RouteDriver::checkDrive() {

    // get the droven distance
    double drovenDistance = GET_CURRENT_DRIVEN_DISTANCE;

    // calculate the current position
    Command currentCommand = driveCalculater->drivingCommands.top();
    Position* calculatedCurrentPosition = currentCommand.getPredictedPositionBy(drovenDistance);

    // check if we're still on the correct way?
    Position* last = map->getLastKnownPosition();
    Position* currentUWB = map->getCarPosition();

    // now compare the positions!
    short xRes = calculatedCurrentPosition->x - currentUWB->x;
    short yRes = calculatedCurrentPosition->y - currentUWB->y;

    // compare it
    bool allGood = true;
    if(quad(xRes) > quad(POSITIONCOMPAREQUALITY) || quad(yRes) > (POSITIONCOMPAREQUALITY)) allGood = false;

    // we don't need to check if we're on the route -> the position seems correct
    if(allGood) return false;

    // we compared the positions and now we need to check the direction
    Vector* way = new Vector(*currentUWB, *last);
    bool correctWay = way->isOnLineTo(&destinations[destinations.size()-1]);

    // do we need a new command? - remove the old one and check if the new one is correct / if not recalculate
    if(correctWay) return false;

    //create command for a "turn to the destination"
    driveCalculater->calculate(currentUWB, currentCommand.getDestinationPosition());

    // execute if necessary the current command
    if(!driveCalculater->drivingCommands.top().isActive()) {
        driveCalculater->drivingCommands.top().execute();
    }

    return true;
}