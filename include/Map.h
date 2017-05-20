//
// Created by Andreas Zinkl on 25.04.17.
//

#ifndef DT2017_MAP_H
#define DT2017_MAP_H

// Physical Dimensions of the Map
#define MapRasterWidth_cm 15
#define MapEnvWidth_cm 120
#define MapEnvHeight_cm 75

// Bit Size of a short data type
#define MapBitsInRow 16

// SW-Array Dimensions of the Map
#define MapColumnsCount MapEnvWidth_cm/MapRasterWidth_cm
#define MapRowsCount MapEnvHeight_cm/MapRasterWidth_cm

// Define if a TestMap should be used
#define useTestMap true

#include "Node.h"
#include <stdlib.h>
#include "Vector.h"

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

    struct Position currentPosition;
    struct Position lastKnownPosition;

    void initTestMap(char* map); // Just for testing purpose
    void print();

public:
    Map();
    ~Map();

    Node* getNode(short x, short y);

    Position* getLastKnownPosition();
    Position* getCarPosition(); //todo this needs to save the last position!!

    Node* getCarPositionNode();
    void getNeighbours(Node* nodelist, short x, short y);
    int updateField(short x, short y, bool isObstacle);
};

#endif //ROUTING_MAP_H
