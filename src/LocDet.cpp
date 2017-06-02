//
// Created by pfm on 4/20/17.
//

#include "../include/LocDet.h"
#include "../include/ReadDataFromSerial.h"
//#include <xls/libxl.h>

#include <iostream>


LocDet::LocDet() {
    Anchor_B.x = 2985;
    Anchor_B.y = 0000;
    Anchor_C.x = 0463;
    Anchor_C.y = 5045;
}

LocDet::~LocDet() {
    delete &Anchor_B, &Anchor_C;
}

int LocDet::get_position(Position *pos){
    Pos_x = 0;
    Pos_y = 0;
    int ret = execute();
    //executewithVector();
    pos->x = Pos_x;
    pos->y = Pos_y;

	std::cout << pos->x << " " << pos->y << std::endl;
    return ret;
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
    y = (numerator / demoniator) - ((double)Anchor_C.x /(double) Anchor_C.y)*(double)x;
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

/*

void LocDet::collectTestData() {
    libxl::Book* book = xlCreateBookA();
    std::string label = "Data";
    ReadDataFromSerial *reader = new ReadDataFromSerial();
    input buf;

    book->load("/home/pfm/Documents/data.xls");
    if(book){
        libxl::Sheet* sheet = book->addSheet(label.c_str(), 0);

        for(int i = 0; i < 501; i++) {
            std::cout << i << std::endl;
            execute();
            sheet->writeStr(i, 3, std::to_string(Dist_A).c_str());
            sheet->writeStr(i, 4, std::to_string(Dist_B).c_str());
            sheet->writeStr(i, 5, std::to_string(Dist_C).c_str());
            sheet->writeStr(i, 6, std::to_string(Pos_x).c_str());
            sheet->writeStr(i, 7, std::to_string(Pos_y).c_str());
        }

        book->save("/home/pfm/Documents/data.xls");
        book->release();
    }

}

 */

int LocDet::execute() {
    Dist_A = 0;
    Dist_B = 0;
    Dist_C = 0;
    int ERROR = 0;
    int count = 0;
    ReadDataFromSerial *Reader = new ReadDataFromSerial();
    input inp;
    do{
        ERROR = Reader->GetData(&inp,1,MR);
        if(!inp.A || !inp.B || !inp.C){
            ERROR = !OK;
        }
        if(count++ > 10)
            return TIMEOUTERR;

    }while (ERROR == !OK);

    Dist_A = inp.A;
    Dist_B = inp.B;
    Dist_C = inp.C;

    calc_xpos();
    return OK;
}
