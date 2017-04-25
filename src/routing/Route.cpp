//
// Created by Andreas Zinkl on 23.04.17.
//

#include <stdlib.h>
#include "Route.h"

Route::Route(Node *from, Node *to) : _start(from), _destination(to) {}

bool Route::calculate() {

    //initialize the open list - priorityQueue
    _openlistPQ->insert(_start, 0);

    while(!_openlistPQ->isEmpty()) {
        Element currentElement = *_openlistPQ->extractMin();
        Node *currentNode = currentElement.getNode();
        // check if we found a route
        if(currentNode->equals(_destination)) {

            Node *tmp = currentNode;
            while(!currentNode->equals(_start)) {
                printf("Node=(%d,%d)", currentNode->X(), currentNode->Y());
            }
            return 0;
        }

        // no route found - continue work on finding a route!
        _closedlist.push_back(*currentNode);

        for (Node* next : currentNode->getSuccessors()) {

            // search for the point in the closed list
            for(Node n : _closedlist) {
                if (n.equals(next)) {
                    continue;
                }
            }

            // calculate the possible new costs
            int costs = 0;
            int destCosts = abs(_destination->X()-next->X()) + abs(_destination->Y()-next->Y());
            if(next->X() != currentNode->X() && next->Y() != currentNode->Y()) {
                // Diagonal Field
                costs = DiagonalFieldCost * destCosts;
            } else {
                // Straight Field
                costs = StraightFieldCost * destCosts;
            }
            costs += currentNode->getCosts();

            // check if we got the field already and if the costs are less or equal than the new costs
            Element *e = _openlistPQ->search(next);
            if(e != NULL && e->getNode()->getCosts() <= costs) { // todo checken auf die kosten nicht vergessen!!
                continue;
            }

            // save the field with its costs
            next->setCosts(costs);
            currentNode->setPredessesor(next);

            // add the new node to the openlist;
            _openlistPQ->insert(next, costs);
        }
    }

    return 1; // not path found
}