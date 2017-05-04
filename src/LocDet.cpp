//
// Created by pfm on 4/20/17.
//

#include "LocDet.h"
#include "ReadDataFromSerial.h"
#define _USE_MATH_DEFINES
#include <iostream>
#include <cmath>

LocDet::LocDet() {
    distance = 5000;
}

LocDet::~LocDet() {}

void LocDet::get_position(position *pos){
    Pos_x = 0;
    Pos_y = 0;
    execute();
    pos->x = Pos_x;
    pos->y = Pos_y;

}

void LocDet::calculate_angle(int Side_A, int Side_B, int Side_C, double* angle){
    double numerator;
    double denominator;
    numerator = (Side_A * Side_A) + (Side_B * Side_B) - (Side_C * Side_C);
    denominator = (Side_A * Side_B);
    *angle = std::acos(0.5*(numerator / denominator))*(180/M_PI);
}

void LocDet::calc_xpos(){
    double angle = 0.0;
    calculate_angle(Dist_A, distance, Dist_C, &angle);
    Pos_x = Dist_A* std::sin((angle / 180 )* M_PI);
    //std::cout << "XPos: " << Pos_x << std::endl;
    calc_ypos();
}

void LocDet::calc_ypos(){
    double angle = 0.0;
    calculate_angle(Dist_A, distance, Dist_B, &angle);
    Pos_y = Dist_A* std::sin((angle / 180 )* M_PI);
    //std::cout << "YPos: " << Pos_y << std::endl;
}

void LocDet::execute() {
    Dist_A = 0;
    Dist_B = 0;
    Dist_C = 0;
    ReadDataFromSerial *Reader = new ReadDataFromSerial();
    input inp;
    Reader->GetData(&inp,10);


    Dist_A = inp.A;
    Dist_B = inp.B;
    Dist_C = inp.C;


    //Reader->GetData(&inp,5);
    calc_xpos();

}