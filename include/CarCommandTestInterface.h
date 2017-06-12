//
// Created by Andreas Zinkl on 09.06.17.
//

#ifndef DT2017_CARCOMMANDTESTINTERFACE_H
#define DT2017_CARCOMMANDTESTINTERFACE_H

#include "CarCommandInterface.h"
#include <iostream>
#include <unistd.h>

class CarCommandTestInterface {

public:
    // CONSTRUCTOR / DECONSTRUCTOR
    CarCommandTestInterface();
    ~CarCommandTestInterface();

    // MENU
    void runTest();

private:

    // CONTENT
    CarCommandInterface* _cmdInterface;
    bool _runTestsystem;

    // MENU
    void showTestCommands();
    void startRemoteControl();


};

#endif //DT2017_CARCOMMANDTESTINTERFACE_H
