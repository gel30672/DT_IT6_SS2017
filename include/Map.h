//
// Created by Andreas Zinkl on 25.04.17.
//

#ifndef DT2017_MAP_H
#define DT2017_MAP_H

#include "Node.h"
#include <stdlib.h>
#include "Vector.h"
#include "ErrorHandling.h"
#include "DeviceConfiguration.h"
#include "MapConfiguration.h"
//#include "LocDet.h" //todo uncomment after merging

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

private:
    unsigned short* nodelist;
    short _size;
    bool isFree(short x, short y);
    //LocDet* locsrv; // This is the UWB Localization Sensor
//todo uncomment after merging
    struct Position currentPosition;
    struct Position lastKnownPosition;

    void initTestMap(char* map); // Just for testing purpose
    void print();

public:
    Map();
    ~Map();

    Position* getLastKnownPosition();
    Position* getCarPosition();

    Node* getNode(short x, short y);
    Node* getCarPositionNode();

    void getNeighbours(Node* nodelist, short x, short y);
    int updateField(short x, short y, bool isObstacle);
};

#endif //ROUTING_MAP_H
