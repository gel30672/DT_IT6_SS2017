#include <iostream>
//#include"Typs.h"
//#inlude"ErrorCodes.h"

#include "ReadDataFromSerial.h"
#include "LocDet.h"




int main(int argc, char* argv[]){

   while(1)
   {
       LocDet *locsrv = new LocDet();

       position akt_pos;
       //pyexe->execute(argc, argv);


       locsrv->get_position(&akt_pos);

       std::cout <<"-----------" << std::endl;
       std::cout <<"x: " <<(float) akt_pos.x / 1000 << " m" << std::endl;
       std::cout <<"y: " <<(float) akt_pos.y / 1000 << " m" << std::endl;


   }
    return 0;
}