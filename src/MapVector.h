//
// Created by Andreas Zinkl on 04.04.17.
//

#pragma once

#ifndef SRC_MAPVECTOR_H
#define SRC_MAPVECTOR_H

#include <cmath>
#include "MapPoint.h"
#include "math.h"

using std::round;

class MapVector {

private:
    MapPoint *_headPoint;
    MapPoint *_footPoint;
    MapPoint *_vectorCoordinates;

public:
    MapVector(MapPoint *head, MapPoint *foot) : _headPoint(head), _footPoint(foot) {

        int vX = _headPoint->get_x()-_footPoint->get_x();
        int vY = _headPoint->get_y()-_footPoint->get_y();
        _vectorCoordinates = new MapPoint(vX, vY);
    }

    /*
     * Returns the Vector-Length multiplied by 100 (reduce rounding problems!!
     */
    int GetLength() {

        int vX2 = _vectorCoordinates->get_x()*_vectorCoordinates->get_x();
        int vY2 = _vectorCoordinates->get_y()*_vectorCoordinates->get_y();

        float sqrtVec = sqrt(vX2+vY2);
        int result = static_cast<int>(std::round(sqrtVec*100));

        return result;
    }

    /*
     * Return the coordinates as a MapPoint-Type of the entered vector
     */
    MapPoint* getVectorCoordinates() {
        return _vectorCoordinates;
    }

};


#endif //SRC_MAPVECTOR_H
