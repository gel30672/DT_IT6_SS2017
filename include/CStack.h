//
// Created by Andreas Zinkl on 12.05.17.
//

#ifndef DT2017_STACK_H
#define DT2017_STACK_H

#include <stdlib.h>

template <class T>
class CStack {

public:
    CStack();
    ~CStack();

    void push (const T& object);
    T pop();
    const T& topElement();
    bool isEmpty();

private:
    struct StackNode {              // linked list node
        T data;                     // data at this node
        StackNode *next;            // next node in list

        // StackNode constructor initializes both fields
        StackNode(const T& newData, StackNode *nextNode)
                : data(newData), next(nextNode) {}
    };

    StackNode *top;                 // top of stack
};


#endif //ALGORITHMS_CPP_STACK_H
