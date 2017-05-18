
#include "../include/Map.h"
#include "../include/Route.h"

int main() {

    // Init alle Sachen
    Map* map = new Map();
    Route* r = new Route(map);
    r->startCalculation();

    // Lösche das mal hier!
    delete map;
    map = nullptr;
    delete r;
    r = nullptr;

    return 0;
}