//
// Created by Kevin Krämer on 27.04.17.
//

#include "ReadDataFromSerial.h"
#include <iostream>
#include <string>
#include <stdio.h>   /* Standard input/output definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <termios.h> /* POSIX terminal control definitions */

#define OK 1
#define Err -1
#define TcgetattrErr -2
#define ReadErr -3
#define ReadNothingErr -4


//Public Functions

int ReadDataFromSerial::GetData(input *Buffer, int MeanAmount)
{
    std::string SerialLine; // Stores the complete string from serial Port

    InitData();

    int runs = 0;
    //Mean the Data

    for(runs = 0; runs < MeanAmount; runs++)
    {
        if(ReadFromSerial(&SerialLine) != OK) //Error
        {
            //Try Again
            if(ReadFromSerial(&SerialLine) != OK)
            {
                return ReadErr;
            }
        }
        else
        {
            if(GetAnchorData(SerialLine, AnchorData) != OK)
            {
                return Err;
            }
            else
            {
                //Add new Values
                for (int j = 0; j < 4; j++) {
                    //std::cout << "Adding Value for Index: " << j << " Value: " << AnchorData[j] << std::endl;
                    AnchorDataOverall[j] += AnchorData[j];
                    //std::cout << "Value for Index  : " << j << " Value: " << AnchorDataOverall[j] << std::endl;
                }
            }
        }
    }



    for(int i = 0; i < 4; i++)
    {
        if(AnchorDataOverall[i] > 0)
        {
            AnchorDataOverall[i] /= runs;
        }
        else
        {
            AnchorDataOverall[i] = 0;
        }
    }

    Buffer->A = (short)AnchorDataOverall[0];
    Buffer->B = (short)AnchorDataOverall[1];
    Buffer->C = (short)AnchorDataOverall[2];

    return OK;
}


//Private Functions

//Sets all Variables to 0 - Just to be sure in the n+1 call
void ReadDataFromSerial::InitData() {

    //Just set Values for Anchor Array to 0
    for(int i = 0; i < 4; i++)
    {
        AnchorData[i] = 0;
        AnchorDataOverall[i] = 0;
    }


}

//Opens The Port and saves the Serial line in the given buffer
int ReadDataFromSerial::ReadFromSerial(std::string *buffer)
{
    //Open Port
    int Port = open( "/dev/ttyACM0", O_RDWR| O_NOCTTY );
    struct termios tty;

    //memset (&tty, 0, sizeof tty);

/* Error Handling */
    if ( tcgetattr ( Port, &tty ) != 0 ) {
        return TcgetattrErr;
    }

/* Setting Port Stuff
    Set Baud Rate */
    cfsetospeed (&tty, (speed_t)B9600);
    cfsetispeed (&tty, (speed_t)B9600);

    tty.c_cflag     &=  ~PARENB;            // Make 8n1
    tty.c_cflag     &=  ~CSTOPB;
    tty.c_cflag     &=  ~CSIZE;
    tty.c_cflag     |=  CS8;
    tty.c_cflag     &=  ~CRTSCTS;           // no flow control
    tty.c_cc[VMIN]   =  1;                  // read doesn't block
    tty.c_cc[VTIME]  =  5;                  // 0.5 seconds read timeout
    tty.c_cflag     |=  CREAD | CLOCAL;     // turn on READ & ignore ctrl lines

/* Make raw */
    cfmakeraw(&tty); //Sets the structure specified by termios_p to raw mode

/* Flush Port, then applies attributes */
    tcflush( Port, TCIFLUSH );
    if ( tcsetattr ( Port, TCSANOW, &tty ) != 0) {
        return TcgetattrErr;
    }

    int n = 0,
    spot = 0;
    char buf = '\0';

/* Whole response*/
    char response[1024];
    //memset(&buffer, '\0', sizeof buffer);

    do {
        n = read( Port, &buf, 1 );
        sprintf(&response[spot], "%c", buf );
        spot += n;
    } while( buf != '\r' && n > 0);

    if (n < 0) {
       return ReadErr;
    }
    else if (n == 0) {
        return ReadNothingErr;
    }
    else {
        //Save response in given Buffer
        *buffer = response;
        return OK;
    }

    return Err;

}


//Gets the Anchor Data from the given Serial string.
//Needed unmodified version of Serial string
int ReadDataFromSerial::GetAnchorData(std::string SerialOutput,int* Buffer) {

    //checking if in correct form
    unsigned long pos = SerialOutput.find("mc");

    if(pos == -1) // if start not found !
    {
        return Err;
    }
    else
    {
        pos += 6; // Jump to first Char of Anchor Data

        //Coping Anchor Data in Buffer. Max 4 Anchors
        //If one Anchor is not used Value = 0
        for(int Index = 0; Index < 4; Index++)
        {
            //Get data of Anchor Index and convert char to integer (base hex)
            Buffer[Index] = (int)std::stoul(SerialOutput.substr(pos, 8), nullptr, 16);
            pos += 9; // Next Anchor data
        }

        return OK;
    }

}