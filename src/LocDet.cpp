//
// Created by pfm on 4/20/17.
//

#include "../include/LocDet.h"
#include "../include/ReadDataFromSerial.h"
//#include "vector.h"
#include <iostream>


LocDet::LocDet() {
    Anchor_B.x = 5000;
    Anchor_B.y = 0000;
    Anchor_C.x = 4000;
    Anchor_C.y = 4000;
}

LocDet::~LocDet() {
    delete &Anchor_B, &Anchor_C;
}

void LocDet::get_position(position *pos){
    Pos_x = 0;
    Pos_y = 0;
    execute();
    //executewithVector();
    pos->x = Pos_x;
    pos->y = Pos_y;

}

void LocDet::calc_xpos(){
    double x;
    double numerator, demoniator;
    numerator = quad(Dist_A) - quad(Dist_B) + quad(Anchor_B.x);
    demoniator = 2 *Anchor_B.x;
    x = numerator / demoniator;
    Pos_x = x;

    calc_ypos(x);
}

void LocDet::calc_ypos(int x){
    double y;
    double numerator, demoniator;
    numerator = quad(Dist_A) - quad(Dist_C) + quad(Anchor_C.x) + quad(Anchor_C.y);
    demoniator = 2 *Anchor_C.y;
    y = (numerator / demoniator) - ((double)Anchor_C.x /(double) Anchor_C.y)*x;
    Pos_y = y;
}

void LocDet::executewithVector() {
    Dist_A = 0;
    Dist_B = 0;
    Dist_C = 0;
    ReadDataFromSerial *Reader = new ReadDataFromSerial();
    input inp;

    Reader->GetTestData(&inp,10);

}

int LocDet::execute() {
    Dist_A = 0;
    Dist_B = 0;
    Dist_C = 0;
    int ERROR = 0;
    int count = 0;
    ReadDataFromSerial *Reader = new ReadDataFromSerial();
    input inp;
    do{
        ERROR = Reader->GetData(&inp,10,MC);
        if(count++ > 10)
            return TIMEOUTERR;

    }while (ERROR == !OK);

    Dist_A = inp.A;
    Dist_B = inp.B;
    Dist_C = inp.C;

    std::cout << "Dis: " << inp.A << std::endl;

    calc_xpos();

    return OK;
}