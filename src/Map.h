//
// Created by Andreas Zinkl on 04.04.17.
//

#pragma once

#ifndef SRC_MAP_H
#define SRC_MAP_H


#include <vector>
#include <MacTypes.h>
#include "MapPoint.h"

class Map {

private:
    int _height;
    int _width;
    int *_map;

public:
    Map(int* map, int height, int width) : _map(map), _height(height), _width(width) {}

    int* GetMap() { return _map; }
    int GetHeight() { return _height; }
    int GetWidth() { return _width; }

    std::vector<MapPoint> GetObstacles() {
        return nil;
    }


};


#endif //SRC_MAP_H
