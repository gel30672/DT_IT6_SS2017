//
// Created by Andreas Zinkl on 25.04.17.
//

#ifndef DT2017_PRIORITYQUEUE_H
#define DT2017_PRIORITYQUEUE_H

#include <iostream>
#include "Element.h"
#include "Map.h"
#include <vector>
#include <stdio.h>

using namespace std;

class PriorityQueue {

private:

    Element* elements;

    int _size;

    void heapify(Element *e, int first, int last, int root);

    void swap(Element &a, Element &b);

    void buildHeap(Element *e, int first, int last);

    void sort(Element* e, int first, int last);

public:

    void insert(Node *e, int p);

    Element* getMin();

    Element* extractMin();

    void decreaseKey(Element e, int k);

    bool isEmpty();

    Element* search(Node* n);

    void print();
};

#endif //ALGORITHMS_CPP_PRIORITYQUEUE_H
