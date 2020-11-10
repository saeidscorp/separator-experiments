//
// Created by saeid on 5/13/20.
//

#include "Edge.hpp"

#include <stdexcept>

using namespace model;

Edge::Edge(Node *from, Node *to, bool directed) : Edge(from, to, 0, directed) {}

Edge::Edge(Node *from, Node *to, int id, bool directed) : Edge(from, to, id, 0, 0, directed) {}

Edge::Edge(Node *from, Node *to, double maxSpeed, double eta, bool directed) :
        Edge(from, to, 0, maxSpeed, eta, nullptr, directed) {}

Edge::Edge(Node *from, Node *to, int id, double maxSpeed, double eta, bool directed) :
        Edge(from, to, id, maxSpeed, eta, nullptr, directed) {}

Edge::Edge(Node *from, Node *to, int id, double maxSpeed, double eta, std::string *name, bool directed) :
        _id(id), _max_speed(maxSpeed), _eta(eta), _name(name), _from(from), _to(to), directed(directed) {}

int Edge::getId() const {
    return _id;
}

void Edge::setId(int id) {
    _id = id;
}

double Edge::getMaxSpeed() const {
    return _max_speed;
}

void Edge::setMaxSpeed(double maxSpeed) {
    _max_speed = maxSpeed;
}

double Edge::getEta() const {
    return _eta;
}

void Edge::setEta(double eta) {
    _eta = eta;
}

std::string *Edge::getName() const {
    return _name;
}

void Edge::setName(std::string *name) {
    _name = name;
}

Node *Edge::getFrom() const {
    return _from;
}

void Edge::setFrom(Node *from) {
    _from = from;
}

Node *Edge::getTo() const {
    return _to;
}

void Edge::setTo(Node *to) {
    _to = to;
}

Node *Edge::getOther(Node *thisOne) {
    if (_from == thisOne)
        return _to;
    if (_to == thisOne)
        return _from;
    throw std::runtime_error(std::string("given node is not part of the edge."));
}

