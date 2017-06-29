//
// Created by Andreas Zinkl on 09.06.17.
//

#include "../include/CarCommandTestInterface.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

CarCommandTestInterface::CarCommandTestInterface() {
    _cmdInterface = new CarCommandInterface();
    _runTestsystem = true;
}

CarCommandTestInterface::~CarCommandTestInterface() {
    //delete _cmdInterface;
    //_cmdInterface = nullptr;
}

void CarCommandTestInterface::runTest() {

    //showTestCommands();
    startRemoteControl();
}

void CarCommandTestInterface::startRemoteControl() {

    struct termios oldSettings, newSettings;

    tcgetattr( fileno( stdin ), &oldSettings );
    newSettings = oldSettings;
    newSettings.c_lflag &= (~ICANON & ~ECHO);
    tcsetattr( fileno( stdin ), TCSANOW, &newSettings );

    int remote_direction = 0;
    int remote_directionold = 0;
    while ( 1 ) {
        fd_set set;
        struct timeval tv;

        tv.tv_sec = 10;
        tv.tv_usec = 0;

        FD_ZERO( &set );
        FD_SET( fileno( stdin ), &set );

        int res = select( fileno( stdin )+1, &set, NULL, NULL, &tv );

        if( res > 0 )
        {
            char c;
            read( fileno( stdin ), &c, 1 );
            if(c == 'w' || c == 'W') {
                _cmdInterface->sendStopCommand();
                _cmdInterface->sendForwardDrive(0, nullptr);
            }
            if(c == 'a' || c == 'A') {
                if(remote_direction > -20) remote_direction -= 5;
            }
            if(c == 'd' || c == 'D') {
                if(remote_direction < 20) remote_direction += 5;
            }
            if(c == 's' || c == 's') {
                _cmdInterface->sendStopCommand();
                _cmdInterface->sendBackwardDrive(0, nullptr);
            }
            if(c == 'f' || c == 'F') _cmdInterface->sendStopCommand();
            if(c == 'r' || c == 'R') {
                _cmdInterface->sendTurnAroundDrive(40, nullptr, DIRECTION_LEFT); int i = 0;
                while(!_cmdInterface->isCurrentDriveFinished()) { std::cout << "fwd" << i << std::endl; i++; }
                _cmdInterface->sendStopCommand();
                sleep(2);
                _cmdInterface->sendBackwardDrive(40, nullptr, DIRECTION_LEFT);
                i = 0;
                while(!_cmdInterface->isCurrentDriveFinished()) { std::cout << "bwd" << i << std::endl; i++; }
                _cmdInterface->sendStopCommand();
            }

            if(remote_direction != remote_directionold) {
                SteerDegrees(remote_direction);
            }
        }
        else if( res < 0 )
        {
            perror( "select error" );
            break;
        }
        else
        {
            printf( "Select timeout\n" );
        }
    }
}

void CarCommandTestInterface::showTestCommands() {

    // Print the title
    std::cout << "############# TEST SYSTEM INTERFACE #############" << std::endl;
    std::cout << "THIS IS TO TEST THE CAR AND THE COMMAND INTERFACE" << std::endl;
    std::cout << "#################################################" << std::endl;
    std::cout << std::endl;

    // Print all commands
    std::cout << "############# CHOSE ONE COMMAND BY ENTER THE NUMBER #############" << std::endl;
    std::cout << "1 - Drive Forward" << std::endl;
    std::cout << "2 - Drive Backward" << std::endl;
    std::cout << "3 - Drive Left Turn" << std::endl;
    std::cout << "4 - Drive Right Turn" << std::endl;
    std::cout << "5 - Stop " << std::endl;
    std::cout << "6 - Exit Test System" << std::endl;
    std::cout << std::endl;

    while(1) {
        // Let the user enter the command
        char num = '0';
        std::cout << "ENTER COMMAND NUMBER: ";
        std::cin >> num;

        // Only execute commands
        switch(num) {

            case '1' :
                _cmdInterface->sendForwardDrive(20, nullptr);
                break;

            case '2':
                _cmdInterface->sendBackwardDrive(20, nullptr);
                break;

            case '3':
                _cmdInterface->sendTurnAroundAndBackDrive(10, nullptr, DIRECTION_LEFT);
                break;

            case '4':
                _cmdInterface->sendTurnAroundAndBackDrive(10, nullptr, DIRECTION_RIGHT);
                break;

            case '5':
                _cmdInterface->sendStopCommand();
                break;

            default :
                exit(-1);
                break;
        }
    }

}