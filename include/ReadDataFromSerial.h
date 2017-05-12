//
// Created by Kevin Krämer on 27.04.17.
//

#ifndef READDATA_READDATAFROMSERIAL_H
#define READDATA_READDATAFROMSERIAL_H
#include <iostream>
#include <string>
#include "ErrorCodes.h"

struct input
{
    short A;
    short B;
    short C;
};

class ReadDataFromSerial {
public:
    ReadDataFromSerial(){};
    ~ReadDataFromSerial(){};
    int GetData(input * Buffer, int MeanAmount,int Typ);
    int GetTestData(input * Buffer, int MeanAmount);

private:
    int InitData(int Typ);
    int ReadFromSerial(std::string *buffer);
    int GetAnchorData(std::string SerialOutput,int* Buffer);

    int AnchorData[4];
    long AnchorDataOverall[4];
    char RawOrCorr[2];

};


#endif //READDATA_READDATAFROMSERIAL_H
