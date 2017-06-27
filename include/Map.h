//
// Created by Andreas Zinkl on 25.04.17.
//

#ifndef DT2017_MAP_H
#define DT2017_MAP_H

#include "Node.h"
#include <stdlib.h>
#include "MapConfiguration.h"
#include "LocDet.h"
#include <iostream>
#include <unistd.h>
#include <vector>
#include <xls/libxl.h>

/*
 * The map will be saved this way:
 *
 * (read this from right to left)
 * 0000000000111000100010010000110011100010
 * Mapping:
 * 1 = obstacle
 * 0 = free
 */

class Map {

public:
    Map();
    ~Map();
    void print();

    Node* getNode(short x, short y);

    short getNeighbours(Node* nodelist, short x, short y);
    short updateField(short x, short y, bool isObstacle);
    short saveRouteInMap(vector<Node> route);
    bool isObstacleInRoute();
    bool isObstacleInFront(Position* current);
    void getmap(unsigned short * map) const;
    short getsize() const;
    void writexls();


private:
    unsigned short* nodelist;
    unsigned short* routeTrack;
    short _size;
    bool isFree(short x, short y);
    short updateField(short x, short y, bool isObstacle, unsigned short* map);

    void initTestMap(char* map); // Just for testing purpose

    void init();



};

#endif //ROUTING_MAP_H
