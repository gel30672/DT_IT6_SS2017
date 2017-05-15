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
    //Write Anchor Data in Buffer. MeanAmount = Count of measurements. Typ = MC or MR
    int GetData(input * Buffer, int MeanAmount,short Typ);
    //Testdata DEBUG
    int GetTestData(input * Buffer, int MeanAmount);

private:
    //Init all Data of Class
    int InitData();
    //Reads line of SerialPort (/tty/ACM0)
    int ReadFromSerial(std::string *buffer);
    //Collect Anchordata of given Serialline
    int GetAnchorData(std::string SerialOutput,int* Buffer);
    //Checks if AnchorData is valid TODO: Implement
    int ValidateData(int* AnchorData);
    //Set Read Mode (MR or MC)
    int SetReadTypMode(short Typ);
    //
    int ValidSerialLine(char* response);


    /* Class Variables */
    int AnchorData[4];
    long AnchorDataOverall[4];
    char RawOrCorr[3];
    char NotRawOrCorr[3];

};


#endif //READDATA_READDATAFROMSERIAL_H
