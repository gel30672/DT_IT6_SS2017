//
// Created by Andreas Zinkl on 23.04.17.
//

#ifndef ROUTING_ROUTE_H
#define ROUTING_ROUTE_H

#define StraightFieldCost 10
#define DiagonalFieldCost 14

#include "Node.h"
#include <vector>
#include "PriorityQueue.h"
#include "Map.h"
#include <stdlib.h>
#include <stack>

using namespace std;


class RouteCalculation {

private:
    Node _start;
    Node _destination;
    Map* _map;

    stack<Node *> _route; //todo hier noch anders speichern!
    PriorityQueue* _openlistPQ;
    vector<Node *> _closedlist;


public:
    RouteCalculation(Map *map, int xDestination, int yDestination);
    ~RouteCalculation();

    bool calculate();
};


#endif //ROUTING_ROUTE_H
