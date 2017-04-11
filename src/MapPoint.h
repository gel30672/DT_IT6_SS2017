//
// Created by Andreas Zinkl on 04.04.17.
//

#pragma once

#ifndef SRC_MAPPOINT_H
#define SRC_MAPPOINT_H


class MapPoint {

private:
    int _x;

private:
    int _y;

public:
    MapPoint(int xCoord, int yCoord) : _x(xCoord), _y(yCoord) { }

    int get_x() const {
        return _x;
    }

    int get_y() const {
        return _y;
    }

};


#endif //SRC_MAPPOINT_H
