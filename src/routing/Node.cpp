//
// Created by Andreas Zinkl on 23.04.17.
//

#include "Node.h"

Node::Node(int x, int y) : _x(x), _y(y), _costs(0) {

    _predessesor = NULL;
}

bool operator<(const Node& lmp, const Node& hmp) {
    return lmp._costs < hmp._costs;
}

int Node::X() const {
    return _x;
}

int Node::Y() const {
    return _y;
}

void Node::setCosts(int costs) {
    _costs = costs;
}

int Node::getCosts() {
    return _costs;
}

bool Node::equals(Node* comp) {
    if(comp->X() == _x && comp->Y() == _y) {
        return true;
    }
    else false;
}

void Node::setPredessesor(Node* node) {
    _predessesor = node;
}

vector<Node*> Node::getSuccessors() {

    if(_successors.size() > 0) {
        return _successors;
    }

    // init successors of the mappoint
    for (int i = -1; i <= 1; i++) {

        if(_x <= 0 || _y <= 0) {
            continue;
        }

        // new mappoint (m|n) which is successor of currentNode
        int m = _x;
        int n = _y;

        _successors.push_back(new Node(m+i, n+i));
        _successors.push_back(new Node(m+i, n-i));
        if(i != 0) { // don't add the own point!!
            _successors.push_back(new Node(m+i, n));
        }
    }

    return _successors;
}