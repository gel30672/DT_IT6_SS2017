//
// Created by Andreas Zinkl on 25.04.17.
//
#include <iostream>
#include "../include/Map.h"

Map::Map() {

    // create the map on the size
    _size = (MapColumnsCount*MapRowsCount)/MapBitsInRow;
    _size += ((MapColumnsCount*MapRowsCount)%MapBitsInRow > 0) ? 1 : 0;
    nodelist = new unsigned short[_size];

    // set every field to 0
    for(int i = 0; i < _size; i++) {
        nodelist[i] = 0;
    }

    // If a test map should
    if(useTestMap) initTestMap("000010000011100010001001000011100000000011100010");
}

/*
 * The testmap looks like this example:
 *
 * 0000000000111000100010010000110011100010
 * Mapping:
 * 1 = obstacle
 * 0 = free
 *
 * This displays e.g.: (line 1 till 4 = 948505826 and line 5 = 0)
 * 5| 0 0 0 0 0 0 0 0
 * 4| 0 1 0 0 0 1 1 1
 * 3| 1 0 0 1 0 0 0 1
 * 2| 0 1 1 1 0 0 0 0
 * 1| 0 0 0 1 0 0 0 0
 * 0| 0 0 0 1 1 1 0 0
 *  |_________________
 *    0 1 2 3 4 5 6 7
 *
 * split: (read each element from right to left)
 *    map[0] = 0000 1000 0011 1000
 *    map[1] = 1000 1001 0000 1110
 *    map[2] = 0000 0000 1110 0010
 */
void Map::initTestMap(char* map) {

    // This inits a given test map!
    std::cout << "Karte:" << std::endl;

    // map values
    unsigned short free = 0; //free
    unsigned short obstacle = 1; //obstacle

    // set values into the map array
    unsigned short value = 0;
    for(int s = 0; s < _size; s++) {

        value = 0; // next short value start with 0

        for(int i = 0; i < MapBitsInRow; i++) {
            short mapindex = s*MapBitsInRow+i;

            unsigned short field = map[mapindex] == '0' ? free : obstacle;
            value |= field;

            // just do a shift, till the short value is full
            if(i != MapBitsInRow-1) value = value << 1;

            //printout the map
            std::cout << field << " ";
            if(i == (MapBitsInRow/2)-1) std::cout << endl;
        }
        std::cout << "         - value=" << value;
        std::cout << std::endl;
        nodelist[s] = value;
    }

    //print();
}

Map::~Map() {}

// Returns a node with the given coordinates
Node* Map::getNode(short x, short y) {

    int pos = (MapEnvWidth_cm/MapRasterWidth_cm)*y+x;

    Node* npointer = nullptr;

    if(pos > _size) {
        x = -1;
        y = -1;
    }

    Node n = Node(x, y);
    npointer = &n;

    return npointer;
}

// Check the map if the given field is free or if there's a obstacle
bool Map::isFree(short x, short y) {

    // There are only positive fields! no fields with negative indexes
    if(x < 0) return 0;
    if(y < 0) return 0;
    if(x >= MapColumnsCount) return 0;

    //bit pattern of the map
    //bit pattern of the pos and-ing!
    short bitInMap = (MapBitsInRow/2*y)+x;
    short mapIndex = bitInMap/MapBitsInRow;
    short bitInMapIndex = bitInMap-(mapIndex*MapBitsInRow);

    // get the bit pattern value from the map
    unsigned short mapbits = nodelist[mapIndex];

    // create the checking bitmap
    unsigned short checkbits = 1;
    checkbits = checkbits << bitInMapIndex;

    short isFree = mapbits&checkbits;

    return !(bool)isFree; // if not 0 then free.. if 1 then obstacle
}

void Map::getNeighbours(Node* nodelist, short x, short y) {

    // define the neighbours list index
    int index = 0;

    // Check each field, if it is a obstacle or free
    for (int i = -1; i <= 1; i++) {

        for (int j = -1; j <= 1; j++) {
            int m = x+i;
            int n = y+j;

            if(i == 0 && j == 0) {
                continue;
            }

            if(isFree(m,n)) nodelist[index] = Node(m,n);
            else nodelist[index] = Node(-1,-1);
            index++;
        }
    }
}

int Map::updateField(short x, short y, bool isObstacle) {
    // calculate the bit position
    int pos = x+y*MapEnvWidth_cm;

    // calculate the array index from the bit position
    short cntIndex = pos/32;

    // generate the mapvalue
    unsigned int obstacle = isObstacle == true ? 1 : 0;
    obstacle = obstacle << (pos - (cntIndex*MapBitsInRow));

    // save the new obstacle in the map
    nodelist[cntIndex] |= obstacle;
    //Todo update auch wenns jetzt ein leeres feld ist

    // Everything went through successfully!
    return 0;
}

// returns the car position node
// this one does not check the current position!!!!!
Node* Map::getCarPosition() {

    _carX = 7;//Test with this data
    _carY = 0;

    // todo this will not calculate the position ! it will just return a node with the position coordinates!!
    // todo the calculation will be done at the driving part

    return new Node(_carX, _carY);
}

void Map::print() {

    short check = 1;
    for(int i = 0; i < _size; i++) {

        short item = nodelist[i];
        for(int j = 0; j < MapColumnsCount; j++) {

            short res = item&check;
            item = item >> 1;
            std::cout << res << " ";
        }
        std::cout << std::endl;
    }

    std::cout << std::endl;
}