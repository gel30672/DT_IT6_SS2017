//
// Created by Andreas Zinkl on 19.05.17.
//

#include "../include/RouteDriver.h"
#include "./libmdrv/SENSING/current_sensing.h"

double distanceSinceStart = 0;

RouteDriver::RouteDriver(Map *map) : map(map), alreadyInitialized(false) { }

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
    short calcRes = routeCalculater->calculate();

    if(PRINT_ERROR_CODE) std::cout << "ROUTEDRIVER: initRouteCalculation(" << calcRes << ") to destination(" << xDestination << "|" << yDestination << ")" << std::endl;

    // check if the calculation went well
    if(calcRes != SUCCESS) return COULD_NOT_CALCULATE_ROUTE_ERROR;

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

        if(PRINT_ERROR_CODE) std::cout << "ROUTEDRIVER: calculateDrive(" << calcError << ")" << std::endl;

        // check calcRes if there's a error message
        if(calcError != SUCCESS) return COULD_NOT_CALCULATE_DRIVECOMMAND_ERROR;

        // save the destination as the lastPosition for the next calculation
        lastPosition = destination;
    }

    // we successfully calculated the commands
    return SUCCESS;
}

short RouteDriver::startInitDrive() {

    if(PRINT_ERROR_CODE) std::cout << "ROUTEDRIVER: start init drive for around " << INIT_CONFIG_TIME << " seconds" << std::endl;

    current = map->getCarPosition();

    // start driving the initialization way
    Command(INIT_CONFIG_DISTANCE, current, nullptr, DIRECTION_FORWARD).execute();

    return 0;
}

short RouteDriver::endInitDrive() {

    // Stop the configuration drive
    Command(INIT_CONFIG_DISTANCE, current, nullptr, DIRECTION_STOP).execute();

    current = map->getCarPosition();

    if(PRINT_ERROR_CODE) {
        std::cout << "ROUTEDRIVER: init drive ended" << std::endl;
        std::cout << "###### Current Position is (" << current->x << "|" << current->y << ")" << std::endl;
    }
    sleep(10);
    return 0;
}

short RouteDriver::init() {

    // First drive the init track
    startInitDrive();

    // wait till we got the
    while(distanceSinceStart < INIT_CONFIG_DISTANCE) {
        if(PRINT_ERROR_CODE) std::cout << "ROUTEDRIVER: DROVEN INIT DISTANCE (cm) = " << distanceSinceStart << std::endl;
    }

    // Okay we did the initialization drive
    endInitDrive();

    // Now we need to initialize the route
    if(PRINT_ERROR_CODE) std::cout << "ROUTEDRIVER: STARTED ROUTE DRIVER INIT" << std::endl;

    // ADD-IN! Get the DESTINATION BY INPUT!
    if(USE_CONSOLE_FOR_DESTINATION_INPUT) askUserForDestination();
    else {
        driveDestination.x = DESTINATION_X_COORDINATE;
        driveDestination.y = DESTINATION_Y_COORDINATE;
    }

    // initialize the route calculation
    short initResRoute = initRouteCalculation(driveDestination.x, driveDestination.y);

    // optimize the current route to get just main nodes
    optimizeRoute();

    // initialize the drive calculator and generate all commands
    short initResDrive = initDriveCalculation(map->getLastKnownPosition(), current);

    // check for errors
    if(initResRoute != SUCCESS) return ERROR_WHILE_ROUTECALC_INIT;
    if(initResDrive != SUCCESS) return ERROR_WHILE_DRIVECALC_INIT;

    alreadyInitialized = true;

    if(PRINT_ERROR_CODE) std::cout << "ROUTEDRIVER: SUCCESS" << std::endl;

    // everything went well! go for it!
    return SUCCESS;
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
            if(needSave) {
                saveToDestination(node.getX(), node.getY());
            }

            // save as predecessor details
            predecessor->x = node.getX();
            predecessor->y = node.getY();
        }
    }
}

bool RouteDriver::currentCommandIsFinished() {

    //if(PRINT_ERROR_CODE) std::cout << "ROUTEDRIVER: started command check" << std::endl;

    // first get the current command
    Command currentCMD = driveCalculater->drivingCommands.top();

    // check if we already did move the whole way
    if(distanceSinceStart >= currentCMD.getDistance()) return true;

    // check if we are near by the destination
    Position* destinationPos = currentCMD.getDestinationPosition();
    Position* uwbPos = map->getCarPosition();

    // prevent the check from errors
    if(destinationPos != nullptr && uwbPos != nullptr) {

        short x_diff = quad(destinationPos->x) - quad(uwbPos->x);
        short y_diff = quad(destinationPos->y) - quad(uwbPos->y);

        if(PRINT_ERROR_CODE) std::cout << "ROUTEDRIVER: calcPosDiff(" << x_diff << "|" << y_diff << ")" << std::endl;

        // check if the current position is nearby our calculated destination
        if((x_diff <= POSITION_PRECISION) && (y_diff <= POSITION_PRECISION)) return true;
    }

    //if(PRINT_ERROR_CODE) std::cout << "ROUTEDRIVER: ended command check" << std::endl;

    // the current command is finished
    return false;
}

bool RouteDriver::directionChangeIsNeeded() {

    //if(PRINT_ERROR_CODE) std::cout << "ROUTEDRIVER: started direction check" << std::endl;

    // define the result value
    bool checkResult = false;

    // check if we have a command left
    if(driveCalculater->drivingCommands.size() <= 0) return NO_DRIVING_COMMANDS_LEFT;

    // get the current command
    Command currentCMD = driveCalculater->drivingCommands.top();

    // check if we got an command destination
    Position* destination = currentCMD.getDestinationPosition();

    if(destination != nullptr) {

        // save droven distance
        double currentDistance = distanceSinceStart;

        // we got a destination, so check if we need a direction change
        Position *current = map->getCarPosition();
        Position *lastKnown = map->getLastKnownPosition();

        // now calculate the "current position"
        Position *calculated = currentCMD.getPredictedPositionBy(currentDistance);

        // check if we got a big difference between calculated and uwb position
        if(calculated != nullptr && current != nullptr) {

            short x_diff = quad(calculated->x) - quad(current->x);
            short y_diff = quad(calculated->y) - quad(current->y);

            // check if the current position is nearby our calculated destination
            if(x_diff > POSITION_PRECISION && y_diff > POSITION_PRECISION) checkResult = true;
        }

        // check the current direction by a vector calculation
        Vector* direction = new Vector(*current, *lastKnown);

        // check if the destination is on the current droven direction
        if(!direction->isOnLineTo(destination)) checkResult = true;
    }

    //if(PRINT_ERROR_CODE) std::cout << "ROUTEDRIVER: ended direction check" << std::endl;

    // the direction change is not needed
    return checkResult;
}


short RouteDriver::checkDrive() {

    // check if we are initialized
    if(!alreadyInitialized) {
        return -1;
    }

    // check if we moved to another place already
    if(distanceSinceStart <= 0) return DROVEN_DISTANCE_IS_ZERO;

    // How far did we already drive since the start pos?
    if(PRINT_ERROR_CODE) std::cout << "ROUTEDRIVER: distanceSinceStart(" << distanceSinceStart << ")" << std::endl;

    // we need to check, if the driveCalculater is initialized
    if(driveCalculater == nullptr) return DRIVECALCULATER_NOT_INITIALIZED;

    // now check if we may need a new route
    if(!map->isRouteAvailable()) {
        Command(0, nullptr, nullptr, DIRECTION_STOP).execute(); // Execute the Stop command!
        init(); // init the whole route driving again
        return ROUTE_OBSTACLE_CROSSING;
    }

    // Start the checkdrive with this debug information
    //if(PRINT_ERROR_CODE) std::cout << "ROUTEDRIVER: perform checkdrive" << std::endl;

    // check if we got an command
    if(driveCalculater->drivingCommands.size() <= 0) {
        Command(0, nullptr, nullptr, DIRECTION_STOP).execute(); // Execute the Stop command!
        return NO_DRIVING_COMMANDS_LEFT;
    }

    // check if we are at the destination
    Position* currentPosition = map->getCarPosition();
    short x_enddiff = currentPosition->x - driveDestination.x;
    short y_enddiff = currentPosition->y - driveDestination.y;
    if(x_enddiff <= POSITION_PRECISION && y_enddiff <= POSITION_PRECISION) {
        Command(0, nullptr, nullptr, DIRECTION_STOP).execute(); // Execute the Stop command!
        std::cout << ">>>>> WE ARE AT THE DESTINATION <<<<<" << std::endl;
        while(driveCalculater->drivingCommands.size() > 0) driveCalculater->drivingCommands.pop(); //delete all commands
        return SUCCESS;
    }

    // check if we can delete the current command
    if(currentCommandIsFinished()) {
        driveCalculater->drivingCommands.pop();
        driveCalculater->drivingCommands.top().execute();
        distanceSinceStart = 0;
    }

    // check if we still have an new command
    if(driveCalculater->drivingCommands.size() <= 0) {
        Command(0, nullptr, nullptr, DIRECTION_STOP).execute(); // Execute the Stop command!
        return NO_DRIVING_COMMANDS_LEFT;
    }

    // check if we are still on our way to the destination and correct the movement if necessary
    if(directionChangeIsNeeded()) {

        Position* dest = driveCalculater->drivingCommands.top().getDestinationPosition();
        Position* current = map->getCarPosition();
        if(dest != nullptr) {

            // check if the current command is active, if it is, then remove it
            if(driveCalculater->drivingCommands.top().isActive()) driveCalculater->drivingCommands.pop();

            // calculate the direction change and save it as a new command
            driveCalculater->calculate(current, dest);
        }
    }

    // check if the current command is active, if not, activate the command
    if(!driveCalculater->drivingCommands.top().isActive()) {
        if(PRINT_ERROR_CODE) std::cout << "ROUTEDRIVER: execute the current command for the first time" << std::endl;
        driveCalculater->drivingCommands.top().execute();
        distanceSinceStart = 0;
    }

    //if(PRINT_ERROR_CODE) std::cout << "ROUTEDRIVER: success checkdrive" << std::endl;

    // we successfully checked the current command
    return SUCCESS;
}

void RouteDriver::askUserForDestination() {

    // Create a CMD UI
    std::cout << "\n\n########### ENTER THE DESTINATION ###########" << std::endl;
    bool correctValue = false;
    string confirmation = "";
    while(!correctValue) {

        std::cout << "Enter the X-Coordinate: ";
        std::cin >> driveDestination.x;
        std::cout << "The entered X-Coordinate is " << driveDestination.x << " right (y/n)? ";
        std::cin >> confirmation;
        if(confirmation == "y" || confirmation == "Y") correctValue = true;
    }

    correctValue = false;
    while(!correctValue) {

        std::cout << "Enter the Y-Coordinate: ";
        std::cin >> driveDestination.y;
        std::cout << "The entered Y-Coordinate is " << driveDestination.y << " right (y/n)? ";
        std::cin >> confirmation;
        if(confirmation == "y" || confirmation == "Y") correctValue = true;
    }

    std::cout << "########### THANK YOU FOR ENTERING A DESTINATION ###########" << std::endl;

    std::cout << "########### The calculation to Position (" << driveDestination.x << "|" << driveDestination.y << ") will start now! ###########\n\n" << std::endl;

}
