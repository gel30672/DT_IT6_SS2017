//
// Created by pfm on 4/20/17.
//

#ifndef LOCSRV_PYTHONEXECUTER_H
#define LOCSRV_PYTHONEXECUTER_H


class PythonExecuter {
public:
    PythonExecuter();
    ~PythonExecuter();
    long executeWithCharPointer(int argc, char** argv, char** buf);

private:

};


#endif //LOCSRV_PYTHONEXECUTER_H
