//
// Created by saeid on 5/13/20.
//

#include "model.hpp"

using namespace model;

void Node::addEdge(Node *other) {
    Edge *e = new Edge(this, other);
    _edges->push_back(e);
    _neighs->insert({other->getId(), other});
}

void Edge::connectFromTo() {
    _from->addNeigh(_to);
    _to->addNeigh(_from);

    _from->addEdge(this);
    _to->addEdge(this);
}
