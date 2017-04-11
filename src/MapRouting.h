//
// Created by Andreas Zinkl on 04.04.17.
//

#pragma once

#ifndef SRC_MAPROUTING_H
#define SRC_MAPROUTING_H


#include <vector>
#include <MacTypes.h>
#include "MapVector.h"
#include "Map.h"

class MapRouting {

private:
    std::vector<MapVector> _route;
    MapPoint* _actPoint;
    MapPoint* _destPoint;

    // Test Map
    Map *map;

    MapVector calculate(MapPoint *start, MapPoint *destination) {

        // Create the Vector
        MapVector v = MapVector(destination, start);
        MapPoint* vPoint = v.getVectorCoordinates();


        // Get all Obstacles from the Map
        std::vector<MapPoint> listOfObstacles = map->GetObstacles();

        /*
         * Check if the Obstacles are on the route vector!
         * -----------------------------------------------
         * If obstacles are on the route then calculate with the end-points of this obstacle.
         * If there are no obstacles on the route, directly drive to the destination!
         */

        for (auto &obstacle : listOfObstacles) {

            // Point Check -> ToDo Optimize this by checking if there's a vector which crosses the route direction vector
            int obX = (obstacle.get_x()-start->get_x())/vPoint->get_x();
            int obY = (obstacle.get_y()-start->get_y())/vPoint->get_y();

            // Check if the Point is on the route
            if (obX == obY) {

                // The Point is on the route -> use the endpoints of the obstacle
                std::vector<MapPoint> obstacleEndPoints;

                // Separate the vector into short vectors around the obstacles
                for (auto &end : obstacleEndPoints) {
                    _route.push_back(calculate(start, &end));
                    _route.push_back(calculate(&end, destination));
                }
            }
        }

        // No obstacle in the route anymore! The route is calculated!
        return v;
    }

public:

    MapRouting(int posX, int posY) {

        _actPoint = new MapPoint(posX, posY);
        map = new Map(nil, 10, 10);
    }

    void setDestination(int posX, int posY) {

        _destPoint = new MapPoint(posX, posY);
    }

    void calculateShortestRoute() {

        // Route calculation is only with a destination possible
        if (_destPoint == nil) {
            return;
        }

        // Start calculating
        MapVector fullDistance = calculate(_actPoint, _destPoint);
        if(_route.capacity() <= 0) {
            _route.push_back(fullDistance);
        }
    }

};


#endif //SRC_MAPROUTING_H
