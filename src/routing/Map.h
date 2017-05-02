//
// Created by Andreas Zinkl on 25.04.17.
//

#ifndef ROUTING_MAP_H
#define ROUTING_MAP_H

#define MapRasterWidth_cm 15
#define MapEnvWidth_cm 500
#define MapEnvHeight_cm 500
#define MapListElementBitsCount 32

#include "Node.h"
#include <stdlib.h>

/*
 * The map will be saved this way:
 *
 * (read this from right to left)
 * 0000000000111000100010010000110011100010
 * Mapping:
 * 1 = obstacle
 * 0 = free
 *
 * This displays e.g.: (line 1 till 4 = 948505826 and line 5 = 0)
 * 4| 0 0 0 0 0 0 0 0
 * 3| 0 0 0 1 1 1 0 0
 * 2| 1 0 0 1 0 0 0 1
 * 1| 0 0 1 1 0 0 0 0
 * 0| 0 1 0 0 0 1 1 1
 *  |________________
 *    0 1 2 3 4 5 6 7
 */

class Map {

private:
    unsigned int* nodelist;
    int _size;
    bool isFree(int x, int y);

    short _carX;
    short _carY;

    void initTestMap(); // Just for testing purpose

public:
    Map();
    ~Map();

    Node* getNode(int x, int y);
    Node* getCarPosition();
    void getNeighbours(Node* nodelist, int x, int y);
    int updateField(short x, short y, bool isObstacle);
};

#endif //ROUTING_MAP_H
