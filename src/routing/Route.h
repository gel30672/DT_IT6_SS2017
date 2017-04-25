//
// Created by Andreas Zinkl on 23.04.17.
//

#ifndef ROUTING_ROUTE_H
#define ROUTING_ROUTE_H

#define StraightFieldCost 10
#define DiagonalFieldCost 14

#include "Node.h"
#include <list>
#include "PriorityQueue.h"
#include <set>

using namespace std;


class Route {

private:
    Node* _start;
    Node* _destination;

    list<Node> _route;
    PriorityQueue* _openlistPQ;
    list<Node> _closedlist;


public:
    Route(Node* from, Node* to);

    bool calculate();

};


#endif //ROUTING_ROUTE_H
