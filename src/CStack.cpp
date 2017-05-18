//
// Created by Andreas Zinkl on 12.05.17.
//

#include "../include/CStack.h"

template <class T>
CStack<T>::CStack() : top(0) {}

template <class T>
CStack<T>::~CStack() {
    while ( !isEmpty() ) {
        pop();
    }
    isEmpty();
}

template <class T>
void CStack<T>::push(const T& obj) {
    top = new StackNode(obj, top);
}

template <class T>
T CStack<T>::pop() {
    if ( !isEmpty() ) {
        StackNode *topNode = top;
        top = top->next;
        T data = topNode->data;
        delete topNode;
        return data;
    }
}

template <class T>
const T& CStack<T>::topElement() {
    if ( !isEmpty() ) {
        return top->data;
    }
}

template <class T>
bool CStack<T>::isEmpty() {
    return top == 0;
}