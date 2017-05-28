//
// Created by Andreas Zinkl on 11.05.17.
//

#ifndef DT2017_VECTOR_H
#define DT2017_VECTOR_H

#define quad(x) ((x)*(x))

// Degree calculation
#define DEGTORAD(D) ((D * M_PI) / 180.0) // Converts Degrees to radians
#define RADTODEG(R) ((180.0 * R) / M_PI)//Converts Radians to Degrees

#include <math.h>
#include "DeviceConfiguration.h"

struct Position {
    short x;
    short y;
};

class Vector {

private:
    Position head;
    Position foot;

public:
    Vector(Position head, Position foot);
    ~Vector();

    short getX();
    short getY();

    Position* getHead();
    Position* getFoot();

    float getAngleTo(Vector *v);
    short getSideOf(Vector *v);
    short rotate(float degrees);
    bool isOnLineTo(struct Position* p);
    void changeDirection();
};


#endif //ALGORITHMS_CPP_VECTOR_H
