#include <iostream>
#include "Route.h"


int main() {

    Route* routing = new Route(new Node(2,2), new Node(2,9));
    routing->calculate();

    return 0;
}