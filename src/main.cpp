#include <iostream>
#include "RouteCalculation.h"
#include "Map.h"


int main() {

    // Init alle Sachen
    Map* map = new Map();
    RouteCalculation* routing = new RouteCalculation(map,2,5);
    routing->calculate();

    // Lösche das mal hier!
    delete map;
    map = nullptr;
    delete routing;
    routing = nullptr;

    return 0;
}