//
// Created by Andreas Zinkl on 16.05.17.
//

#include "../include/Route.h"

Route::Route(Map* map) : map(map) {

    // todo Define the coordinates
    short xDes = 2;
    short yDes = 4;

    routeCalculater = new RouteCalculation(map, xDes, yDes);
    driveCalculater = new DriveCalculation(drivingCommandStack);
}

Route::~Route() {
    delete routeCalculater;
    delete driveCalculater;
    //delete &drivingCommandStack;
    //delete &destinationStack;
}

void Route::optimizeRoute() {

    // check values
    bool lastWasDiagonal = false;
    Position prev;
    short xDiff;
    short yDiff;

    // This method always tries to optimize the route by searching for main nodes
    CStack<Node *> calculatedRoute = routeCalculater->getRouteStack();
    while(!calculatedRoute.isEmpty()) {

        // Get the node from the calculatedRouteStack
        Node *n = calculatedRoute.topElement();

        // create a position
        Position nodePos;
        nodePos.x = n->getX();
        nodePos.y = n->getY();

        // First checked Node can always be added to the destinations (this is always the starting point)
        if(destinationStack.isEmpty()) {

            // save this position for next checks
            prev.x = nodePos.x;
            prev.y = nodePos.y;

            // push the position to the stack
            destinationStack.push(nodePos);
            continue;
        }

        // So now we already added a node to the stack
        // We need to check the last position and the new node position

        short xDiffOld = xDiff;
        short yDiffOld = yDiff;

        xDiff = nodePos.x - prev.x;
        yDiff = nodePos.y - nodePos.y;

        // Check if the old and new one are on the same diagonal line
        if((quad(xDiff+1) == quad(xDiffOld) && quad(yDiff+1) == quad(yDiffOld))
            || (quad(xDiff-1) == quad(xDiffOld) && quad(yDiff-1) == quad(yDiffOld))){

            lastWasDiagonal = true;
            continue;
        }

        // Check if the positions are on the same horizontal line
        if(yDiff == yDiffOld && (xDiff+1 == xDiffOld ||xDiff-1 == xDiffOld) && !lastWasDiagonal) {
            continue;
        }

        // Check if the position is on the same vertical line
        if((xDiff == xDiffOld && (yDiff+1 == yDiffOld || yDiff-1 == yDiffOld)) && !lastWasDiagonal) {
            continue;
        }

        //todo check this here! we need to safe the last position in row or line - not the first one!!

        // Now we are on a field which changes from a line to a diagonal or from a diagonal to a straight line

        // save this position for next checks
        prev.x = nodePos.x;
        prev.y = nodePos.y;
        lastWasDiagonal = false;

        // push the position to the stack
        destinationStack.push(nodePos);
    }
}

short Route::startCalculation() {

    // first calculate the route
    routeCalculater->calculate();

    // now we need to optimize the route
    optimizeRoute();

    // remember the last positions;
    Position lastPosition;

    // run through the fields and look for straight lines!
    while(!destinationStack.isEmpty()) {

        // Get the top element!
        Position destination = destinationStack.pop();

        /*
         * DriveCalculater:
         *  - calculates the route by the main nodes
         *  - generates the commands for each way from one to another main node
         */
        driveCalculater->calculate(&lastPosition, &destination);

        // remeber the destination
        lastPosition = destination;
    }

    // We finished the command generation successfully
    return 0;
}