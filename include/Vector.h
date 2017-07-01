//
// Created by Andreas Zinkl on 11.05.17.
//

#ifndef DT2017_VECTOR_H
#define DT2017_VECTOR_H

// Degree calculation
#define DEGTORAD(D) ((D * M_PI) / 180.0) // Converts Degrees to radians
#define RADTODEG(R) ((180.0 * R) / M_PI)//Converts Radians to Degrees

#include <math.h>
#include "DeviceConfiguration.h"
#include "MapConfiguration.h"
#include "LocDet.h"
#include <vector>

class Vector {

private:
    Position head;
    Position foot;
    Position regHead;
    Position regFoot;

    std::vector<Position> regressionPositions;
    int regXSum;
    int regYSum;

    void calculateRegression();

public:
    Vector(Position head, Position foot);
    ~Vector();

    short getX();
    short getY();

    Position* getHead();
    Position* getFoot();

    double getLength();
    double getRegressionVectorLength();

    void addPosition(Position* pos);

    float getAngleTo(Vector *v);
    short getSideOf(Vector *v);
    short rotate(float degrees);
    bool isOnLineTo(struct Position* p);
    void changeDirection();
    void setFoot(Position* foot);

    // only a test function
    void testFunctionality();
};


#endif //ALGORITHMS_CPP_VECTOR_H
