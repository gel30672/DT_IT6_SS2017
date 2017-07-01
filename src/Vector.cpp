//
// Created by Andreas Zinkl on 11.05.17.
//

#include <iostream>
#include "../include/Vector.h"

Vector::Vector(Position head, Position foot) : head(head), foot(foot), regHead(head), regFoot(foot), regXSum(0), regYSum(0) {}

Vector::~Vector() {
    //delete &head;
    //delete &foot;
}

short Vector::getX() {
    return (regHead.x-regFoot.x) * MapRasterWidth_cm;
    //return (head.x-foot.x) * MapRasterWidth_cm;
}

short Vector::getY() {
    return (regHead.y-regFoot.y) * MapRasterWidth_cm;
    //return (head.y-foot.y) * MapRasterWidth_cm;
}

Position* Vector::getHead() {
    return &head;
}

Position* Vector::getFoot() {
    return &foot;
}

void Vector::setFoot(Position* foot) {
    foot = foot;
}

float Vector::getAngleTo(Vector* v) {
    //calculate the angle between the this vector and the given vector v
    /*
     * a = angle alpha
     * u = self
     * v = vector v
     *
     * cos(a) = u*v / |u|*|v|
     * a = arccos(u*v / |u|*|v|)
     */
    float dot = getX()*v->getX()+getY()*v->getY();
    float det = sqrt(quad(getX())+quad(getY())) * sqrt(quad(v->getX())+quad(v->getY()));

    if(dot/det == 0) {

        // We need to check if the angle is 90 degrees!
        short side = getSideOf(v);
        if(side == DIRECTION_LEFT || side == DIRECTION_RIGHT) return 90;

        // no it is on the straight line!
        return 0;
    }

    float alpha = acosf(dot/det);
    alpha = RADTODEG(alpha);

    if(alpha != alpha) alpha = 0;

    return alpha;
}

short Vector::getSideOf(Vector* v) {

    // Check depending on the driving direction, if the destination is to it's left or right side
    /*
     * calculate currentposition-lastpositionknown (from 2d to 3d) -> vector A
     * calculate destinationposition-lastpositionknown (from 2d to 3d) -> vector B
     *
     * vector A         vector B
     * a1               b1
     * a2               b2
     * a3 = 0           b3 = 0
     *
     *
     * the z coordinate of the cross product of both vectors is where it lies. (a1*b2-a2*b1)
     * - positive means it is to the left side
     * - negative means it is to the right side
     */
    short position = getX()*v->getY() - getY()*v->getX();

    // return the direction
    if(getX() > 0) return position < 0 ? DIRECTION_RIGHT : DIRECTION_LEFT;
    else return position < 0 ? DIRECTION_LEFT : DIRECTION_RIGHT;
}

bool Vector::isOnLineTo(struct Position *p) {

    // Generate the vector from this foot part to the destination
    //struct Position pFoot;
    //pFoot.x = foot.x;
    //pFoot.y = foot.y;
    Vector* tmpVector = new Vector(*p, foot);

    float angle = getAngleTo(tmpVector);

    if(angle<=POSITION_PRECISION) return true;

    return false;
}

short Vector::rotate(float degrees) {

    // First calculate the values for the rotation matrix
    // we don't need to generate a matrix, just note the values!
    double cosDeg = cos(degrees);
    double msinDeg = -sin(degrees);
    double sindDeg = sin(degrees);

    // rotate the x and y coordinates
    float newX = (float) (cosDeg * getX() + msinDeg * getY());
    float newY = (float) (sindDeg * getX() + cosDeg * getY());

    // save the new values to the head point
    head.x = foot.x+newX;
    head.y = foot.y+newY;
    regHead.x = regFoot.x+newX;
    regHead.y = regFoot.y+newY;

    return 0;
}

void Vector::changeDirection() {

    // Change the direction
    Position tmp = head;
    head = foot;
    foot = tmp;

    // change also the regression direction
    tmp = regHead;
    regHead = regFoot;
    regFoot = tmp;
}

double Vector::getLength() {
    return sqrt(quad(head.x)+quad(head.y))*MapRasterWidth_cm;
}

double Vector::getRegressionVectorLength() {
    return sqrt(quad(getX())+quad(getY()))*MapRasterWidth_cm;
}

void Vector::addPosition(Position *pos) {
    regressionPositions.push_back(*pos);
    regXSum += pos->x;
    regYSum += pos->y;

    calculateRegression();
}

// PRIVATE METHODS

void Vector::calculateRegression() {

    // calculate the regression
    int numOfPositions = regressionPositions.size();
    double sumDot = 0.0;
    double sumDet = 0.0;
    double xMiddle = regXSum/numOfPositions;
    double yMiddle = regYSum/numOfPositions;
    for(int i=0; i < numOfPositions; i++) {
        sumDot = (regressionPositions[i].x - xMiddle) * (regressionPositions[i].y - yMiddle);
        sumDet = (regressionPositions[i].x - xMiddle) * (regressionPositions[i].x - xMiddle);
    }

    int b = sumDot/sumDet;
    int a= yMiddle - b*xMiddle;

    std::cout << "Geradengleichung: y=" << b << "*x + " << a << std::endl;

    // calculate the regression head
    regHead.x = 5;
    regHead.y = a+b*regHead.x;

    regFoot.x = 1;
    regFoot.y = a+b*regFoot.x;
}

void Vector::testFunctionality() {

    // check if the regression calcluation works
    Position pos01;
    pos01.x = 1;
    pos01.y = 1;
    addPosition(&pos01);

    Position pos02;
    pos02.x = 2;
    pos02.y = 1;
    addPosition(&pos02);

    Position pos03;
    pos03.x = 3;
    pos03.y = 2;
    addPosition(&pos03);

    Position pos031;
    pos031.x = 3;
    pos031.y = 2;
    addPosition(&pos031);

    Position pos032;
    pos032.x = 3;
    pos032.y = 2;
    addPosition(&pos032);

    Position pos04;
    pos04.x = 4;
    pos04.y = 2;
    addPosition(&pos04);

    Position pos05;
    pos05.x = 5;
    pos05.y = 3;
    addPosition(&pos05);

    Position pos06;
    pos06.x = 5;
    pos06.y = 4;
    addPosition(&pos06);

    Position pos07;
    pos07.x = 4;
    pos07.y = 2;
    addPosition(&pos07);

    Position pos08;
    pos08.x = 1;
    pos08.y = 2;
    addPosition(&pos08);
}
