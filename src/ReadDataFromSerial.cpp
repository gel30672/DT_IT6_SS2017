//
// Created by Kevin Krämer on 27.04.17.
//

#include "../include/ReadDataFromSerial.h"
#include <iostream>
#include <stdio.h>
#include <string.h>   /* Standard input/output definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <termios.h> /* POSIX terminal control definitions */

#define PORT "/dev/ttyACM0"



//Public Functions

int ReadDataFromSerial::GetData(input *Buffer, int MeanAmount, short Typ)
{
    std::string SerialLine; // Stores the complete string from serial Port

    InitData();

    if(SetReadTypMode(Typ) != OK)
    {
        return ERR;
    }

    int runs = 0;
    //Mean the Data
    for(runs = 0; runs < MeanAmount; runs++)
    {
        if(ReadFromSerial(&SerialLine) != OK) //Error
        {

                return ERR;
        }
        else
        {
            if(GetAnchorData(SerialLine, AnchorData) != OK)
            {

                return ERR;
            }
            else
            {
                //Add new Values
                for (int j = 0; j < 4; j++) {

                    //std::cout << "Adding Value for Index: " << j << " Value: " << AnchorData[j] << std::endl;

                    //ValidateData(AnchorData);
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

int ReadDataFromSerial::SetReadTypMode(short Typ)
{
    switch(Typ)
    {
        case(MR):
            strncpy(RawOrCorr,"mr",sizeof(RawOrCorr));
            strncpy(NotRawOrCorr,"mc", sizeof(NotRawOrCorr));
            break;

        case (MC):
            strncpy(RawOrCorr,"mc",sizeof(RawOrCorr));
            strncpy(NotRawOrCorr,"mr",sizeof(NotRawOrCorr));
            break;

        default:
            return WRONGTYPERR;

    }
    return OK;
}



//Sets all Variables to 0 - Just to be sure in the n+1 call
int ReadDataFromSerial::InitData() {


    //Just set Values for Anchor Array to 0
    for(int i = 0; i < 4; i++)
    {
        AnchorData[i] = 0;
        AnchorDataOverall[i] = 0;
    }

    return OK;

}

//Opens The Port and saves the Serial line in the given buffer
int ReadDataFromSerial::ReadFromSerial(std::string *buffer)
{

    //Open Port
    int Port = open( PORT , O_RDWR| O_NOCTTY );
    struct termios tty;

/* Error Handling */
    if ( tcgetattr ( Port, &tty ) != 0 ) {
        return TCGETATTRERR;
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
        return TCGETATTRERR;
    }

    int n = 0, spot = 0, cnt = 0;
    char buf = '\0';

/* Whole response*/
    char response[1024];

    //Read until RawOrCorr (mr or mc) found
    do
    {
        //Reset values
        spot = 0;
        n = 0;
        buf = '\0';

        //Read one line Char for Char
        do {
                n = (int)read(Port, &buf, 1 );
                //Drop Whitespaces
                if(buf != ' ')
                {
                    sprintf(&response[spot], "%c", buf);
                    spot += n;
                }

            } while( buf != '\r' && n > 0);


    }while(ValidSerialLine(response) != OK);


    //Close Port
    close(Port);

    if (n < 0) {
       return READERR;
    }
    else if (n == 0) {
        return READNOTHINGERR;
    }
    else {
        //Save response in given Buffer
        *buffer = response;
        return OK;
    }

    return ERR;

}

int ReadDataFromSerial::ValidSerialLine(char* response)
{
    if(strstr(response,RawOrCorr) != NULL) //Check if RawOrCorr in Line
    {
        if(strstr(response,NotRawOrCorr) != NULL) //Check if NotRawOrCorr in Line
        {
            //std::cout << "Found" << std::endl;
            return ERR;

        }

        return OK;
    }

    return ERR;
}


//Gets the Anchor Data from the given Serial string.
//Needed unmodified version of Serial string
int ReadDataFromSerial::GetAnchorData(std::string SerialOutput,int* Buffer) {

    //std::cout << SerialOutput << std::endl;
    std::string::size_type pos = 0;


      //  std::cout << "GetAnchorData" << std::endl;
      //  std::cout << SerialOutput << std::endl;


    pos = SerialOutput.find(RawOrCorr,0);


    if(pos == std::string::npos)
    {
        return ERR;
    }

    pos += 4;
    //Coping Anchor Data in Buffer. Max 4 Anchors
    //If one Anchor is not used Value = 0
    for(int Index = 0; Index < 4; Index++)
    {
        //Get data of Anchor Index and convert char to integer (base hex)
        Buffer[Index] = (int)std::stoul(SerialOutput.substr(pos, 8), nullptr, 16);
        pos += 8; // Next Anchor data
    }

    return OK;
}


int ReadDataFromSerial::GetTestData(input *Buffer, int MeanAmount) {
    Buffer->A = 5400;
    Buffer->B = 10200;
    Buffer->C = 10900;

    return OK;
}


int ValidateData(int* AnchorData)
{
    return OK;
}
