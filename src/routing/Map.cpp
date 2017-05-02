//
// Created by Andreas Zinkl on 25.04.17.
//
#include <string>
#include "Map.h"

Map::Map() {

    // create the map on the size
    _size = (MapEnvHeight_cm*MapEnvWidth_cm)/MapRasterWidth_cm/MapListElementBitsCount;
    nodelist = new unsigned int[_size];

    // set every field to 0
    for(int i = 0; i < _size; i++) {
        nodelist[i] = 0;
    }

    // TODO test - just test with a random map
    initTestMap();
}

void Map::initTestMap() {
    // This inits a test map!
    char* map = "0000000000111000100010010000110011100010";
    for(int i = 0; i < 40; i++) {
        short t = i/32;
        unsigned int akt = nodelist[t];
        unsigned int in = map[i] == '0' ? 0 : 1;
        akt = akt << 1;
        akt |= in;
        nodelist[t] = akt;
    }
}

Map::~Map() {}

// Returns a node with the given coordinates
Node* Map::getNode(int x, int y) {

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
bool Map::isFree(int x, int y) {

    // There are only positive fields! no fields with negative indexes
    if(x < 0) return 0;
    if(y < 0) return 0;

    //bit pattern of the map
    //bit pattern of the pos and-ing!
    short fieldbit = (_size*y)+x;
    short mapindex = fieldbit/MapListElementBitsCount;

    unsigned int mapbits = nodelist[mapindex];
    printf("%d", mapbits);
    fieldbit -= (MapListElementBitsCount*mapindex);
    printf("%d", fieldbit);
    unsigned int fieldbits = 1;
    fieldbits = fieldbits << fieldbit;
    printf("%d", fieldbits);

    return !(bool)(mapbits&fieldbits); // if not 0 then free.. if 1 then obstacle
}

void Map::getNeighbours(Node* nodelist, int x, int y) {

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
    obstacle = obstacle << pos-(cntIndex*MapListElementBitsCount);

    // save the new obstacle in the map
    nodelist[cntIndex] |= obstacle;
    //Todo update auch wenns jetzt ein leeres feld ist

    // Everything went through successfully!
    return 0;
}

// returns the car position given from the localization
Node* Map::getCarPosition() {

    // TODO Wait till the localization implementation, then implement the car position getter!
    _carX = 0;//Test with this data
    _carY = 0;

    return new Node(_carX, _carY);
}