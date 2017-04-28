#include <iostream>
#include "../include/PythonExecuter.h"
#include "../include/LocDet.h"




int main(int argc, char* argv[]){
    PythonExecuter *pyexe = new PythonExecuter();
    LocDet *locsrv = new LocDet();

    position akt_pos;
    //pyexe->execute(argc, argv);





    locsrv->get_position(&akt_pos);

    std::cout << (float)akt_pos.x / 1000 << " cm" << std::endl;
    std::cout << (float)akt_pos.y / 1000 << " cm" <<std::endl;

    return 0;
}