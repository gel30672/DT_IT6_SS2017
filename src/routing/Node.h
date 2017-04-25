//
// Created by Andreas Zinkl on 23.04.17.
//

#ifndef ROUTING_NODE_H
#define ROUTING_NODE_H

#include <vector>

using namespace std;

class Node {

private:
    int _x;
    int _y;
    int _costs;

    Node *_predessesor;
    vector<Node*> _successors;

public:
    Node(int x, int y);

    friend bool operator<(const Node& lmp, const Node& hmp);

    int X() const;
    int Y() const;

    void setCosts(int costs);
    int getCosts();
    bool equals(Node* comp);

    void setPredessesor(Node* node);
    vector<Node*> getSuccessors();
};


#endif //ROUTING_NODE_H
