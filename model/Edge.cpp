//
// Created by saeid on 5/13/20.
//

#include "Edge.hpp"

using namespace model;

Edge::Edge(Node *from, Node *to) : Edge(from, to, 0) {}

Edge::Edge(Node *from, Node *to, int id) : Edge(from, to, id, 0, 0) {}

Edge::Edge(Node *from, Node *to, double maxSpeed, double eta) : Edge(from, to, 0, maxSpeed, eta, nullptr) {}

Edge::Edge(Node *from, Node *to, int id, double maxSpeed, double eta) : Edge(from, to, id, maxSpeed, eta, nullptr) {}

Edge::Edge(Node *from, Node *to, int id, double maxSpeed, double eta, std::string *name) : _id(id),
                                                                                           _max_speed(maxSpeed),
                                                                                           _eta(eta), _name(name),
                                                                                           _from(from), _to(to) {}

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

