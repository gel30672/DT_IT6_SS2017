//
// Created by pfm on 5/11/17.
//



#include "../include/datacollector.h"
#include <string>
#include "../include/ReadDataFromSerial.h"


datacollector::datacollector() {

}

datacollector::~datacollector() {

}

void datacollector::run() {
    libxl::Book* book = xlCreateBookA();
    std::string label = "Data";
    ReadDataFromSerial *reader = new ReadDataFromSerial();
    input buf;


    book->load("/home/pfm/Documents/data.xls");
    if(book){
        libxl::Sheet* sheet = book->addSheet(label.c_str(), 0);

        for(int i = 3; i < 20; i++){
            reader->GetTestData(&buf, 5);
            //std::cout << buf.A << " " << buf.B << " " << buf.C << std::endl;
            sheet->writeStr(i,3, std::to_string(buf.A).c_str());
            sheet->writeStr(i,4, std::to_string(buf.B).c_str());
            sheet->writeStr(i,5, std::to_string(buf.C).c_str());

        }


        book->save("/home/pfm/Documents/data.xls");
        book->release();
    }
}

