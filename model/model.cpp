//
// Created by saeid on 5/13/20.
//

#include "model.hpp"

using namespace model;

void Node::addEdge(Edge *edge) {
    _edges->push_back(edge);
    auto other = edge->getOther(this);
    addNeigh(other);
    _edge_map->insert({other, edge});
}

Edge *Node::connect(Node *other) {
    Edge *edge = new Edge(this, other);
    addEdge(edge);
    return edge;
}

Node *Node::connect(Edge *existingEdge) {
    addEdge(existingEdge);
    return existingEdge->getOther(this);
}

void Edge::connectFromTo() {
    _from->addNeigh(_to);
    _to->addNeigh(_from);

    _from->addEdge(this);
    _to->addEdge(this);
}
